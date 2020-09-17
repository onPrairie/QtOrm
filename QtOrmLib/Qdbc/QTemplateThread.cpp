#include "QTemplateThread.h"
#include <QDebug>
#include <QSettings>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QStringBuilder>
#include <QSqlRecord>
#include "QdbcFactory.h"
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
int QTmeplate::qdbc_mod = -1;
int QTmeplate::loglevel;
QTmeplate::QTmeplate(QObject *parent)
	: QObject(parent)
{
}

QTmeplate::~QTmeplate()
{
}

bool QTmeplate::read_date(db_struct& db)
{
	QSettings *ini = new QSettings("qjbctemplate.ini", QSettings::IniFormat);
	loglevel = ini->value("TEMPLATE/Loglevel", 0).toInt();
	QStringList ls = ini->childGroups();
	bool flag = false;
	for (int i = 0; i < ls.size();i++)
	{
		if (ls[i] == "MYSQL") {
			db.ip = ini->value("MYSQL/host", "127.0.0.1").toString();
			db.dbName = ini->value("MYSQL/dbname", "").toString();
			//QString ip = "127.0.0.1";//settings->value("mysqldatabase/ip").toString();
			db.port = ini->value("MYSQL/port", 3306).toInt();
			db.userName = ini->value("MYSQL/dbuser", "root").toString();
			db.passWord = ini->value("MYSQL/dbpwd", "").toString();
			db.timeout = ini->value("MYSQL/timeout", 2).toString();
			QString isPool = ini->value("MYSQL/isPool","false").toString();
			if (isPool == "false") {
				qWarning() << "[QDBC]" << "如果开启了多线程模式，则推荐开启数据库连接池";	
				qdbc_mod =1;
				
			}
			else if(isPool == "true")
			{
				qInfo() << "[QDBC]" << "一旦打开了线程池，打印会有一定的乱序，可根据线程id进行筛选";
				qdbc_mod = 0;
			}
			else
			{
				qCritical("[QDBC] 配置文件参数不对。。。。");
				qdbc_mod = -1;
			}
			db.dbdriver = ls[i];
			flag = true;
			break;
		}
	}
	delete ini;

	return flag;
}

bool QTmeplate::set_database_isopen(QSqlDatabase* database)
{
	if (database->isOpen() == false) {
		this->flag = 4;
		return false;
	}
	else
	{
		return true;
	}
	
}

QSqlDatabase* QTmeplate::get_connect()
{
	if (databasetrans == NULL) {
		databasetrans = QdbcFactory::createDataSource(qdbc_mod)->openConnection();
	}
	return databasetrans;
}

void QTmeplate::close_connect(QSqlDatabase * db,const QString& errcoe)
{
	if (errcoe == "2006") {
		db->open();
	}
	/*if (db != databasetrans) {
		QdbcFactory::createDataSource(qdbc_mod)->closeConnection(db);
	}*/
	
}
#include "Object_qdbc.h"
void QTmeplate::do_sql(QVariantList m_data, int falg)
{
	
	switch (falg)
	{
	case  0:
	{
		if (this->QDBC_id == "QDBC_1") {
			db_struct db;
			bool s = read_date(db);
			if (loglevel != 0) qInfo() << "[QDBC]" << "准备sql语句以及初始化";
			if (s == false) {
				qFatal("[error:] 读取参数失败");
			}
			QString options = "MYSQL_OPT_CONNECT_TIMEOUT=" % db.timeout;
			bool b = QdbcFactory::createDataSource(qdbc_mod)->LoadDatabase(db.dbdriver, db.ip, db.dbName, db.port, db.userName, db.passWord, options);
			if (b == false) {
				QMap<int, QVariantList>  map_data;
				do_result(map_data, b, 8);
			}
			else
			{
				if (loglevel == 0) {
					this->detection_drive(get_connect());
				}
			}
			this->is_init_done = true;
			qInfo() << this->QDBC_id << "数据库初始化完成@@@";
			//线程分发
		}
		else
		{
			QdbcTemplate* tmep1 = NULL;
			while (true)
			{
					//线程加入
				QMap<int, QdbcTemplate*> t1 = QdbcTemplate::Allinstance();
				for each (QdbcTemplate* tmep in t1)
				{
					QMap<int, QVariantList>  map_data;
					if (tmep->getselfthread()->QDBC_id == "QDBC_1") {
						tmep1 = tmep;
						break;
					}
				}
				if (tmep1 == NULL) {
					continue;
				}
				if (tmep1->getselfthread()->is_init_done == true) {
					int b = tmep1->getselfthread()->sqlres;
					do_result(b);
					qInfo() << this->QDBC_id << "数据库初始化完成@@@";
					break;
				}
			}
		}
	}
		break;
	case  1:  //select 
	{
		QSqlDatabase* database = get_connect();
		QString errcode;
		{
			QSqlDatabase* database = get_connect();
			QSqlQuery queryMysql = QSqlQuery(*database);
			bool s = queryMysql.prepare(m_data[0].toString());
			for (int i = 1; i < m_data.size(); i++) {
				queryMysql.bindValue(i - 1, m_data[i]);
			}
			if (loglevel == 0) {
				qInfo() << QDBC_id << " prepare sql:" << m_data[0].toString();
				if (m_data.size() > 0) {
					QString str;
					for (int i = 1; i < m_data.size(); i++) {
						str.append("  arg:" % m_data[i].toString());
					}
					qInfo() << QDBC_id << " prepare args:" << str;
				}
			}
			bool b = queryMysql.exec();
			int flag = 3;
			if (!b)
			{
				flag = 8;
				errcode = queryMysql.lastError().nativeErrorCode();
				qWarning() << QDBC_id << errcode << ":" << queryMysql.lastError().text();
			}
			m_data.clear();
			QMap<int, QVariantList>  map_data;
			int i = 0;
			int row = queryMysql.record().count();
			while (queryMysql.next()) {
				for (int j = 0; j < row; j++) {
					QString str = queryMysql.record().fieldName(j);
					//str =  str[0].toLower() + str.right(str.length() - 1);
					map_data[i].append(str);	//列名
					map_data[i].append(queryMysql.value(j));
				}
				i++;
			}
			qInfo() << QDBC_id << " Total:" << map_data.size();
			do_result(map_data, b, flag);
		}
		close_connect(database, errcode);
	}
		break;
	case  2:  //update && delete && insert 
	{
		QSqlDatabase* database = get_connect();
		QString errcode;
		{
			QSqlQuery queryMysql = QSqlQuery(*database);
			bool s = queryMysql.prepare(m_data[0].toString());
			for (int i = 1; i < m_data.size(); i++) {
				queryMysql.bindValue(i - 1, m_data[i]);
			}
			if (loglevel == 0) {
				qInfo() << QDBC_id << " prepare sql:" << m_data[0].toString();
				if (m_data.size() > 0) {
					QString str;
					for (int i = 1; i < m_data.size(); i++) {
						str.append("  arg:" % m_data[i].toString());
					}
					qInfo() << QDBC_id << " prepare args:" << str;
				}
			}
			bool b = queryMysql.exec();
			int flag = 3;
			if (!b)
			{
				flag = 8;
				errcode = queryMysql.lastError().nativeErrorCode();
				qWarning() << QDBC_id << errcode << ":" << queryMysql.lastError().text();
			}
			m_data.clear();
			int i = 0;
			int rows = queryMysql.numRowsAffected();
			QMap<int, QVariantList>  map_data;
			map_data[0].append("rows");	//列名
			map_data[0].append(rows);
			qInfo() << QDBC_id << " Updates:" << rows;
			do_result(map_data, b, flag);
		}
		close_connect(database, errcode);
	}
		break;
	case 5: //transaction open
	{
		if (databasetrans == NULL) {
			databasetrans = QdbcFactory::createDataSource(qdbc_mod)->openConnection();
		}
		bool s = databasetrans->transaction();
		QMap<int, QVariantList>  map_data;
		int falg = 3;
		if (s == false) {
			flag = 8;
			qWarning() << "Transaction is not supported";
		}
		do_result(map_data, s, flag);
		//QdbcFactory::createDataSource(qdbc_mod)->closeConnection(database);
	}
		
		
		break;
	case 6: //transaction close
	{
		bool s =  databasetrans->commit();
		QMap<int, QVariantList>  map_data;
		int falg = 3;
		if (s == false) {
			flag = 8;
		}
		do_result(map_data, s, falg);
		QdbcFactory::createDataSource(qdbc_mod)->closeConnection(databasetrans);
	}
	
		break;
	case 7://transaction rollback
	{
		bool s = databasetrans->rollback();
		QMap<int, QVariantList>  map_data;
		int falg = 3;
		if (s == false) {
			flag = 8;
		}
		QdbcFactory::createDataSource(qdbc_mod)->closeConnection(databasetrans);
		do_result(map_data, s,flag);
	}
	
		break;
	case 8:
	{
		qInfo() << QDBC_id << " close the databases";
		QdbcFactory::createDataSource(qdbc_mod)->removeConnection();

	}
	default:
		break;
	}
	
}

void QTmeplate::do_result(QMap<int, QVariantList> &m_data, bool sqlres,int flag)
{
	if (sqlres == false) {
		this->issqltransactionResult = false;
	}
	this->res_data = m_data;
	this->sqlres = sqlres;
	this->flag = flag;
}

void QTmeplate::do_result(bool sqlres)
{
	sync.lock();
	if (sqlres == false) {
		this->issqltransactionResult = false;
	}
	this->sqlres = sqlres;
	this->flag = 0;
	sync.unlock();
}
#include <QSqlDriver>
void QTmeplate::detection_drive(QSqlDatabase * database)
{
	qInfo() << "start detection drive=========================》";
	if (database->driver()->hasFeature(QSqlDriver::Transactions)) {
		qInfo() << "++++++have Transactions";
	}
	else
	{
		qInfo() << "------no such Transactions";
	}
	if (database->driver()->hasFeature(QSqlDriver::BLOB)) {
		qInfo() << "++++++have BLOB";
	}
	else
	{
		qInfo() << "------no such BLOB";
	}
	if (database->driver()->hasFeature(QSqlDriver::Unicode)) {
		qInfo() << "++++++have Unicode";
	}
	else
	{
		qInfo() << "------no such Unicode";
	}
	if (database->driver()->hasFeature(QSqlDriver::PreparedQueries)) {
		qInfo() << "++++++have PreparedQueries";
	}
	else
	{
		qInfo() << "------no such PreparedQueries";
	}
	if (database->driver()->hasFeature(QSqlDriver::NamedPlaceholders)) {
		qInfo() << "++++++have NamedPlaceholders";
	}
	else
	{
		qInfo() << "------no such NamedPlaceholders";
	}
	if (database->driver()->hasFeature(QSqlDriver::PositionalPlaceholders)) {
		qInfo() << "++++++have PositionalPlaceholders";
	}
	else
	{
		qInfo() << "------no such PositionalPlaceholders";
	}
	if (database->driver()->hasFeature(QSqlDriver::LastInsertId)) {
		qInfo() << "++++++have LastInsertId";
	}
	else
	{
		qInfo() << "------no such LastInsertId";
	}
	if (database->driver()->hasFeature(QSqlDriver::BatchOperations)) {
		qInfo() << "++++++have BatchOperations";
	}
	else
	{
		qInfo() << "------no such BatchOperations";
	}
	if (database->driver()->hasFeature(QSqlDriver::SimpleLocking)) {
		qInfo() << "++++++have SimpleLocking";
	}
	else
	{
		qInfo() << "------no such SimpleLocking";
	}
	if (database->driver()->hasFeature(QSqlDriver::LowPrecisionNumbers)) {
		qInfo() << "++++++have LowPrecisionNumbers";
	}
	else
	{
		qInfo() << "------no such LowPrecisionNumbers";
	}
	if (database->driver()->hasFeature(QSqlDriver::EventNotifications)) {
		qInfo() << "++++++have EventNotifications";
	}
	else
	{
		qInfo() << "------no such EventNotifications";
	}
	if (database->driver()->hasFeature(QSqlDriver::FinishQuery)) {
		qInfo() << "++++++have FinishQuery";
	}
	else
	{
		qInfo() << "------no such FinishQuery";
	}
	if (database->driver()->hasFeature(QSqlDriver::MultipleResultSets)) {
		qInfo() << "++++++have MultipleResultSets";
	}
	else
	{
		qInfo() << "------no such MultipleResultSets";
	}
	if (database->driver()->hasFeature(QSqlDriver::CancelQuery)) {
		qInfo() << "++++++have CancelQuery";
	}
	else
	{
		qInfo() << "------no such CancelQuery";
	}
	qInfo() << "end detection drive=========================》";
	
}

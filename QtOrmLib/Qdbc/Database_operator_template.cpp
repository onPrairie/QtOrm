#include "../include/Database_operator_template.h"
#include <QSettings>
#include <QDebug>
#include<QSqlError>


QAtomicPointer<Database_operator_template> Database_operator_template::_instance = 0;
QMutex Database_operator_template::_mutex;
Database_operator_template::Database_operator_template(QObject *parent)
	: IDatabase(parent)
{
	
}

Database_operator_template::~Database_operator_template()
{
}

bool Database_operator_template::LoadDatabase(const QString& db, const QString& host, const QString& dbname, int port, const QString& username, const QString& password, const QString& options,const QString& parms)
{
	//simpleTelnet(host,port,1000);
	bool flag = true;
	QSqlDatabase* dbDataMysql = new QSqlDatabase;
	if ((!dbDataMysql->isOpen())) {
		*dbDataMysql = QSqlDatabase::addDatabase("QMYSQL", "connectionName");///////////////
		dbDataMysql->setConnectOptions(options);
		dbDataMysql->setHostName(host);
		dbDataMysql->setPassword(password);
		dbDataMysql->setPort(port);
		dbDataMysql->setUserName(username);
		dbDataMysql->setDatabaseName(dbname);
		if (!dbDataMysql->open()) {
			flag = false;
			qWarning() << "[QDBC:]" << dbDataMysql->lastError().nativeErrorCode() << ":" << dbDataMysql->lastError().text();
			//qDebug() << "mysql itsdata info : ip:" << ip << ",port:" << port << ",username:" << userName << ",password:" << passWord << ".";
		}
		else {
			qDebug() << "[QDBC:]" << " Mysql database successfully";
		}
	}
	if (flag == false) {
		//delete dbDataMysql;
		//dbDataMysql = NULL;
	}
	db_g = dbDataMysql;
	return flag;
}

QSqlDatabase* Database_operator_template::openConnection()
{
	if (!this->db_g->isOpen()) {
		this->db_g->open();
	}
	return this->db_g;
}

bool Database_operator_template::closeConnection(QSqlDatabase* db)
{
	//delete db;
	return true;
}

bool Database_operator_template::removeConnection()
{
	if (db_g == NULL) {
		return false;
	}
	QString str = db_g->connectionName();
	db_g->close();
	delete db_g;
	
	QSqlDatabase::removeDatabase(str);//É¾³ýÄ¬ÈÏÁ¬½Ó¡£
	return true;
}

Database_operator_template * Database_operator_template::createDataSource()
{
	if (!QAtomicPointer<Database_operator_template>::isTestAndSetNative()) {
		qWarning() << "Error:isTestAndSetNative is not supported";
	}
	if (_instance.testAndSetOrdered(0, 0)) {
		QMutexLocker lock(&_mutex);
		_instance.testAndSetOrdered(0, new Database_operator_template(0));
	}
	return _instance;
}

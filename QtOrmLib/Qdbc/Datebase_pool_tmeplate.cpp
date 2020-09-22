#include "../include/Datebase_pool_tmeplate.h"
#include <QSettings>
#include <QScopedPointer>
#include <QDebug>
#include<QSqlError>
QAtomicPointer<Datebase_pool_tmeplate> Datebase_pool_tmeplate::_instance = 0;
QMutex Datebase_pool_tmeplate::_mutex;
Datebase_pool_tmeplate::Datebase_pool_tmeplate(QObject *parent)
	: IDatabase(parent)
{
	//QScopedPointer<QSettings> ini(new QSettings("qjbctemplate.ini", QSettings::IniFormat));
	//int initialPoolSize = ini->value("pool/initialPoolSize", 1).toInt();
	//for (int i = 0; i < initialPoolSize; i++) {
	//	QSqlDatabase* dbDataMysql = new QSqlDatabase;
	//	db_g.append(dbDataMysql;
	//}
}

bool Datebase_pool_tmeplate::LoadDatabase(const QString& db, const QString& host, const QString& dbname, int port, const QString& username, const QString& password, const QString& options, const QString& parms)
{
	//QSettings *ini = new QSettings("qjbctemplate.ini", QSettings::IniFormat);
	//int initialPoolSize = ini->value("pool/initialPoolSize", 1).toInt();
	//delete ini;
	bool flag = true;

	QString initialPoolSize = "initialPoolSize=";
	QStringList Poollist = parms.split("&");
	bool t = false;
	for (int i = 0; i < Poollist.size(); i++) {
		if (Poollist[i].indexOf(initialPoolSize) == 0) {
			initialPoolSize  = Poollist[i].right(Poollist[i].size() - initialPoolSize.size());
			t = true;
		}
	}
	if (t == false) {
		qFatal("not find pool/initialPoolSize");
	}
	//¼ÓËø
	_mutex.lock();
	for (int i = 0; i < initialPoolSize.toInt(); i++) {
		QSqlDatabase* dbDataMysql = new QSqlDatabase;
		if ((!dbDataMysql->isOpen())) {
			*dbDataMysql = QSqlDatabase::addDatabase("QMYSQL","template" + QString::number(i));///////////////
			dbDataMysql->setConnectOptions("MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3;MYSQL_OPT_CONNECT_TIMEOUT = 3;MYSQL_OPT_RECONNECT=1");
			dbDataMysql->setHostName(host);
			dbDataMysql->setPassword(password);
			dbDataMysql->setPort(port);
			dbDataMysql->setUserName(username);
			dbDataMysql->setDatabaseName(dbname);
			dbDataMysql->close();
			db_manger db;
			db.db = dbDataMysql;
			db.isUse = false;
			g_db_manger.append(db);
		}
		if (flag == false) {
			/*delete dbDataMysql;
			dbDataMysql = NULL;*/
		}
	}
	_mutex.unlock();
	for (int i = 0; i < initialPoolSize.toInt(); i++) {
		if (!g_db_manger[i].db->open()) {
			flag = false;
			qInfo() << "[QDBC:]" << g_db_manger[i].db->lastError().number() << ":" << g_db_manger[i].db->lastError().text();
			//qDebug() << "mysql itsdata info : ip:" << ip << ",port:" << port << ",username:" << userName << ",password:" << passWord << ".";
		}
		else {
			qInfo() << "[QDBC:]" << " Mysql database successfully" << "[pool muns" << i + 1 << "]";
		}
	}
	return flag;
}

QSqlDatabase * Datebase_pool_tmeplate::openConnection()
{
	QSqlDatabase* db = NULL;
	for (int i = 0; i < g_db_manger.size(); i++) {
		if (g_db_manger[i].isUse == false) {
			_mutex.lock();
			g_db_manger[i].isUse = true;
			_mutex.unlock();
			db =  g_db_manger[i].db;
			break;
		}
	}
	return db;
}

bool Datebase_pool_tmeplate::closeConnection(QSqlDatabase * db)
{
	for (int i = 0; i < g_db_manger.size(); i++) {
		if (g_db_manger[i].db == db) {
			_mutex.lock();
			g_db_manger[i].isUse = false;
			_mutex.unlock();
			break;
		}
	}
	return true;
}

bool Datebase_pool_tmeplate::removeConnection()
{
	return false;
}

Datebase_pool_tmeplate * Datebase_pool_tmeplate::createDataSource()
{
	if (!QAtomicPointer<Datebase_pool_tmeplate>::isTestAndSetNative()) {
		qWarning() << "Error:isTestAndSetNative is not supported";
	}
	if (_instance.testAndSetOrdered(0, 0)) {
		QMutexLocker lock(&_mutex);
		_instance.testAndSetOrdered(0, new Datebase_pool_tmeplate(0));
	}
	return _instance;
}

Datebase_pool_tmeplate::~Datebase_pool_tmeplate()
{
}

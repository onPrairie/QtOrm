#pragma once


#include  <QVariantList>
#include <QSqlDatabase>
#include "IDatabase.h"
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include "qtormlib_global.h"
struct db_struct
{
	QString ip;
	QString dbName;
	int port;
	QString userName;
	QString passWord;
	QString dbdriver;
	QString timeout;
	QString querystring;  //pool
};
class QTORMLIB_EXPORT QTmeplate : public QObject
{
	Q_OBJECT

public:
	QTmeplate(QObject *parent);
	QWaitCondition pauseCond;
	QMutex sync;
	bool is_pause;

	static bool is_init_done;

	//继续
	void resume()
	{
		sync.lock();
		is_pause = false;
		sync.unlock();
		pauseCond.wakeAll();

	}
	//暂停
	void pause()
	{
		sync.lock();
		is_pause = true;
		sync.unlock();
	}
	QString QDBC_id;	//用来表示sql操作的id

	QString get_QDBC_id() {
		return QDBC_id;
	}
	QVariantList m_data;
	int   flag = 0; 		//0表示初始化 1表示查询 2表示更新（增，删，改）3,表示结果 4，表示数据库延迟 
							//5，表示事务 6,事务提交 7，事务回滚 8,执行sql语句报错
						//-1 不满足参数要求，要求为 (Qselect >> t << t) 格式...

	//res
	bool issqltransactionResult = false;	//执行事务sql的返回值
	QMap<int, QVariantList> res_data;
	bool sqlres;


	
	
	~QTmeplate();
	//QSqlDatabase* database;
	QSqlDatabase* databasetrans = NULL;//连接句柄

	static int qdbc_mod;  //-1 表示未开启多线程  0表示开单线程数据库连接 1表示开启数据库连接池
	static int loglevel;	//0 默认不打卡info日志
private:
	//采取策略模式
	//设置flag为4
	bool set_database_isopen(QSqlDatabase* database);
	QSqlDatabase* get_connect();
	void close_connect(QSqlDatabase* db,const QString& errcoe);
public:
	bool read_date(db_struct & db, const QString & path);
	//void run();
	void do_result(QMap<int, QVariantList>  &m_data, bool sqlres, int flag);

	void do_result(bool sqlres);
	void detection_drive(QSqlDatabase* database);
	void do_sql(int falg);
};
#include <QSettings>
class mysettings : public QSettings
{
public:
	mysettings(const QString & fileName, Format format);

	QString  getchildrensfromgrop(const QString group);
	~mysettings()
	{
	}

private:

};

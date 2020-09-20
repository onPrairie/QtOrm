#pragma once

#include <QObject>
#include <QSqlDatabase>
class  IDatabase : public QObject
{
	Q_OBJECT

public:
	IDatabase(QObject *parent);

	~IDatabase();

public:
	//若开启多线程，只能在主线程初始化且初始化一次
	virtual bool LoadDatabase(const QString& db, const QString& host, const QString& dbname, int port,
		const QString& username, const QString& password,const QString& options,const QString& parms) = 0;
	virtual QSqlDatabase* openConnection() = 0;
	//关闭数据连接则为
	virtual bool closeConnection(QSqlDatabase* db) = 0;
	//删除数据连接则为
	virtual bool removeConnection() = 0;
};

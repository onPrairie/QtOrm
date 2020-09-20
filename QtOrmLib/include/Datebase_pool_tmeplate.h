#pragma once

#include <QObject>
#include "IDatabase.h"
#include <QMutexLocker>
#include <QMutex>
#include <QAtomicPointer>
#include <QAtomicInt>
class Datebase_pool_tmeplate : public IDatabase
{
	Q_OBJECT

public:
	QList<QSqlDatabase*> db_g;
	QAtomicInt count = 0;
	virtual bool LoadDatabase(const QString& db, const QString& host, const QString& dbname, int port,
		const QString& username, const QString& password, const QString& options,const QString& parms) override;
	virtual QSqlDatabase* openConnection() override;


	virtual bool closeConnection(QSqlDatabase* db) override;


	virtual bool removeConnection() override;

	static	Datebase_pool_tmeplate*  createDataSource();
	~Datebase_pool_tmeplate();
private:
	static QAtomicPointer<Datebase_pool_tmeplate> _instance;
	static QMutex _mutex;
	Datebase_pool_tmeplate(QObject *parent);
};

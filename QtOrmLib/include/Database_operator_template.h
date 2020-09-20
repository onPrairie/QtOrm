#pragma once

#include <QObject>
#include <QSqlDatabase>
#include "IDatabase.h"
#include <QMutexLocker>
#include <QMutex>
#include <QAtomicPointer>
class Database_operator_template : public IDatabase
{
	//Q_OBJECT
public:

	~Database_operator_template();


	QSqlDatabase* db_g = NULL;
	virtual bool LoadDatabase(const QString& db, const QString& host, const QString& dbname, int port,
		const QString& username, const QString& password, const QString& options, const QString& parms) override;
	virtual QSqlDatabase* openConnection() override;


	virtual bool closeConnection(QSqlDatabase* db) override;


	virtual bool removeConnection() override;

	static	Database_operator_template*  createDataSource();
	Database_operator_template(QObject *parent);
private:
	static QAtomicPointer<Database_operator_template> _instance;
	static QMutex _mutex;
	
};

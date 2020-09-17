#include "../include/QdbcFactory.h"
#include "../include/Datebase_pool_tmeplate.h"
#include "../include/Database_operator_template.h"
IDatabase* QdbcFactory::createDataSource(int id)
{
	IDatabase * iDatabase = NULL;
	
	switch (id)
	{
	case 0:
		iDatabase = Datebase_pool_tmeplate::createDataSource();
		break;
	case 1:
		iDatabase = Database_operator_template::createDataSource();
		break;
	default:
		break;
	}
	return iDatabase;

}

QdbcFactory::QdbcFactory(QObject *parent)
	: QObject(parent)
{
}

QdbcFactory::~QdbcFactory()
{
}

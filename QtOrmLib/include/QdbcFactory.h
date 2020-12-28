#pragma once

#include <QObject>
#include "IDatabase.h"

class  QdbcFactory : public QObject 
{
	Q_OBJECT
public:
	//0表示初始化模式线程池 	1表示初始化单线程数据库
	static	IDatabase*  createDataSource(int id);
	QdbcFactory(QObject *parent);
	~QdbcFactory();
};

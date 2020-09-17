#ifndef ANALYSIS_CONTAINER_H
#define ANALYSIS_CONTAINER_H
#include <QVariantList>
#include <QObject>
#include <stdarg.h>
#include <QStringList>
#include "qtormlib_global.h"
/*
 tags:set if range
*/
class QTORMLIB_EXPORT Analysis_container
{
	//Q_OBJECT
public:
	enum tags
	{
		tag_if,
		tag_foreach,
	};
	tags tag_each;
	QString endlres = "";
	
	//int res = 0;
	QString res = "";
	QString getif(bool t,int arg, char* p);
	QStringList const_ls;
	int flag;
	QString getswitch(QString& arg,char* value,int args);

	QString getswitch(char* arg, char* value, int args);

	QString getswitch(int arg, char* value, int args);
	QString getforeach(char* begin, char* separator, char* end, QString& arryls);
	QString getforeach(char* begin, char* separator, char* end, QList<int>& arryls);
	QString getforeach(char* begin, char* separator, char* end, QStringList& arryls);
	Analysis_container() {

	}
	~Analysis_container();
	int get_attrs(QVariantList& ls);
	QString findchild(QString tags, QObject * value);

	QString findchild(QString tags, int value);

	bool invokefuncAndappend(QObject * value, QByteArray name, QVariant & t);

	
private:
	QVariantList attr_ls;
};

#endif // ANALYSIS_CONTAINER_H

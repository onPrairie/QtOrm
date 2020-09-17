#include "../include/analysis_container.h"
#include <QMetaMethod>
#include <QStringBuilder>
#include <QDateTime>
#include "../include/Object_qdbc.h"
QString Analysis_container::getif(bool t,int arg, char* p)
{
	if (arg == 3) {
		endlres = " "%QString(p)% " ";
	}
	QString str;
	if (t == true) 
	{
		res = " "% QString(p) % " ";
	}
	
	if (arg == 1) {
		if (this->res == "") {
			str = this->endlres;
			this->res = "";
			this->endlres = "";
			return str;
		}
		str = this->res;
		this->res = "";
		this->endlres = "";
		return str;
	}
	else
	{
		return "";
	}
}

QString Analysis_container::getswitch(QString & arg, char * value, int args)
{
	if (args == 10) {
		bool t = false;
		int i;
		QString argstr = arg;
		for (i = 0; i < const_ls.size(); i += 2) {
			if (const_ls[i] == argstr) {
				t = true;
				break;
			}
		}
		QString res;
		if (t == false) {
			res = this->endlres;
			this->endlres = "";
			const_ls.clear();
			return  res;
		}
		else
		{
			res = const_ls[i + 1];
			this->endlres = "";
			const_ls.clear();
			return  res;
		}
	}
	if (args == 12) {   //'default'
		this->endlres = QString(value);
	}
	if (args == 11) {	//'case'
		const_ls.append(arg);
		const_ls.append(QString(value));
	}
	return "";
}
QString Analysis_container::getswitch(char * arg, char * value, int args)
{
	if (args == 10) {
		bool t = false;
		int i;
		QString argstr = QString(arg);
		for (i = 0; i < const_ls.size(); i += 2) {
			if (const_ls[i] == argstr) {
				t = true;
				break;
			}
		}
		QString res;
		if (t == false) {
			res = this->endlres;
			this->endlres = "";
			const_ls.clear();
			return res ;
		}
		else
		{
			res = const_ls[i + 1];
			this->endlres = "";
			const_ls.clear();
			return  res;
		}
	}
	if (args == 12) {   //'default'
		this->endlres = QString(value);
	}
	if (args == 11) {	//'case'
		const_ls.append(QString(arg));
		const_ls.append(QString(value));
	}
	return "";
}
QString Analysis_container::getswitch(int arg, char * value, int args)
{
	if (args == 10) {
		bool t = false;
		int i;
		int argstr = arg;
		for (i = 0; i < const_ls.size(); i += 2) {
			if (const_ls[i].toInt() == argstr) {
				t = true;
				break;
			}
		}
		QString res;
		if (t == false) {
			res = this->endlres;
			this->endlres = "";
			const_ls.clear();
			return  res;
		}
		else
		{
			res = const_ls[i + 1];
			this->endlres = "";
			const_ls.clear();
			return  res ;
		}
	}
	if (args == 12) {   //'default'
		this->endlres = QString(value);
	}
	if (args == 11) {	//'case'
		const_ls.append(QString::number(arg));
		const_ls.append(QString(value));
	}
	return "";
}
QString Analysis_container::getforeach(char * begin, char * separator, char * end, QString & arryls)
{
	QString qbegin = QString(begin);
	QString qend = QString(end);
	QString qseparator = QString(separator);
	QString res;
	int len = arryls.size();
	if (len == 0) {
		return "";
	}
	res.append(qbegin);
	for (int i = 0; i < arryls.size(); i++) {
		if (i == arryls.size() - 1) {
			res.append("'"%arryls[i]%"'");
		}
		else
		{
			res.append("'"%arryls[i] % "'" % qseparator);
		}
	}
	res.append(qend);
	return res;
}
QString Analysis_container::getforeach(char * begin, char * separator, char * end, QList<int>& arryls)
{
	QString qbegin = QString(begin);
	QString qend = QString(end);
	QString qseparator = QString(separator);
	QString res;
	int len = arryls.size();
	if (len == 0) {
		return "";
	}
	res.append(" ");
	res.append(qbegin);
	for (int i = 0; i < arryls.size(); i++) {
		if (i == arryls.size() - 1) {
			res.append(QString::number(arryls[i])) ;
		}
		else
		{
			res.append(QString::number(arryls[i])  % qseparator);
		}
	}
	res.append(qend);
	res.append(" ");
	return res;
}
QString Analysis_container::getforeach(char * begin, char * separator, char * end, QStringList & arryls)
{
	QString qbegin = QString(begin);
	QString qend = QString(end);
	QString qseparator = QString(separator);
	QString res;
	int len = arryls.size();
	if (len == 0) {
		return "";
	}
	res.append(qbegin);
	for (int i = 0; i < arryls.size(); i++) {
		if (i == arryls.size() - 1) {
			res.append("'"%arryls[i]%"'");
		}
		else
		{
			res.append("'"%arryls[i] % "'" % qseparator);
		}
	}
	res.append(qend);
	return res;
}
Analysis_container::~Analysis_container()
{

}
int Analysis_container::get_attrs(QVariantList& ls)
{
	return 0;
}
QString Analysis_container::findchild(QString condition, QObject * value)
{
	int index_str = 0;
	while (1)
	{
		int res = condition.indexOf("#{", index_str + 2);
		if (res == index_str || res == -1) {
			break;
		}
		index_str = res;
		int lastindex = condition.indexOf('}', index_str);
		QString str = condition.mid(index_str + 2, lastindex - index_str - 2);


	}
	return condition;
}
QString Analysis_container::findchild(QString tags, int value)
{
	return QString();
}
bool Analysis_container::invokefuncAndappend(QObject * value, QByteArray name, QVariant & t)
{
	QByteArray gets = "get" % name;
	QByteArray getss = "get" %name % "()";
	char* gets_ptr = getss.data();
	const QMetaObject* theMetaObject = value->metaObject();
	for (int i = 5; i < theMetaObject->methodCount(); i++)
	{
		QMetaMethod oneMethod = theMetaObject->method(i);
		if (strcmp(gets_ptr, oneMethod.methodSignature()) == 0) {
			gets_ptr = (char*)oneMethod.typeName();
			break;
		}

	}
	QString str = QString(gets_ptr);

	bool resfunc;
	if (str == "int") {
		int retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(int, retVal));
		t.setValue(retVal);
	}
	else if (str == "bool")
	{
		bool retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(bool, retVal));
		t.setValue(retVal);
	}
	else if (str == "QString")
	{
		QString retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(QString, retVal));
		t.setValue(retVal);
	}
	else if (str == "QDateTime")
	{
		QDateTime retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(QDateTime, retVal));
		t.setValue(retVal);
	}
	else
	{
		//assert_args("undef type!!!");
		resfunc = false;
	}
	return resfunc;
}
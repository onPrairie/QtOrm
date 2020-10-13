#include "../include/Object_qdbc.h"
#include <QDebug>
#include <QTime>
#include <QSettings>
#include <QMetaType>
#include <QMetaProperty>
#include "../include/Qdbc.h"
int qdbc_stringindex(QString& str,int pos) {
	int index = -1;
	for (int i = pos; i < str.length(); i++) {
		if (str[i] == ',' || str[i] == ' ') {
			index = i;
			break;
		}
	}
	return index;
}
int qdbc_stringresverindex(QString& str, int pos) {
	int index = -1;
	for (int i = pos; i > 1; i--) {
		if (str[i] == ',' || str[i] == ' ') {
			index = i;
			break;
		}
	}
	return index;
}
void qdbc_stringformat(QString& str) {
	if (str[0] == '`') {
		str.remove('`');
	}
	str = str.toLower();
}
QHash<int, QdbcTemplate*> QdbcTemplate::__instance__;
QdbcTemplate::QdbcTemplate(QObject *parent)
	: QObject(parent)
{
	Qconfig config;
	QString path;
	bool b =config.fullfilepath(path);
	if (b == false) {
		this->assert_args("can not find this config file");
	}
	int a = ++atom;
	//暂时不使用线程处理，虽然此处看起来位线程
	//fullfilepath(Qconfig::filename, Qconfig::fliepath);
	mythread = new QTmeplate;
	QSettings *ini = new QSettings(path, QSettings::IniFormat);
	Loglevel = ini->value("TEMPLATE/Loglevel", 0).toInt();
	QString memory =  ini->value("TEMPLATE/automemory").toString();
	if (memory == "false") {
		isautomemory = false;
	}
	else if(memory == "true")
	{
		isautomemory = true;
	}
	else
	{
		assert_args("TEMPLATE/automemory config error!");
	}
	ini->setValue("TEMPLATE/version", QString(Qversion));
	delete ini;

	mythread->QDBC_id = "QDBC_" % QString::number(a);
	mythread->m_data.append(path);
	mythread->do_sql(0);
}
int QdbcTemplate::countString(QString str)
{
	int count = 0;
	for (int i =0; i < str.length();i++)
	{
		if (str[i] == "#") {
			count++;
		}

	}
	return count;
}
void QdbcTemplate::querytable(QString & str)
{
	this->tablenames.clear();
	isUseUnion = false;
	//from ... where
	int n  = str.indexOf("from", Qt::CaseInsensitive);
	if (n < 0) {
		return;
	}
	n += 4;
	int end = str.indexOf("where", n, Qt::CaseInsensitive);
	if (end < 0) {
		return;
	}
	QStringList ls;
	if (end > n) {
		QString strs = str.mid(n,end - n).trimmed();
		ls = strs.split(",");
		if (ls.size() > 1) {
			isUseUnion = true;
			for (int i = 0; i < ls.size(); i++) {
				for (int j = 0; j < ls[i].size(); j++) {
					if (ls[i].at(j) == ' ') {
						QString str = ls[i].left(j);
						QString str1 = ls[i].right(ls[i].size() - j).trimmed();
						qdbc_stringformat(str);
						qdbc_stringformat(str1);
						if (str1 != "") {
							tablenames[str1] = str;
						}
						break;
					}
				}
			}
		}
		ls.clear();
		if (isUseUnion == true) {
			int t = 0, t1 = 0;
			while (true)
			{
				t = str.indexOf(".", t1);
				if (t > n || t < 0) {
					break;
				}
				t1 = qdbc_stringresverindex(str, t - 1);
				QString parent = str.mid(t1 + 1, t - t1 - 1);
				t1 = qdbc_stringindex(str, t + 1);
				QString child = str.mid(t + 1, t1 - t - 1);
				qdbc_stringformat(parent);
				qdbc_stringformat(child);
				//子-》父
				tablenames[child] = parent;

			}
		}
	}
	else
	{
		return;
	}
	//jion ... on
}
void QdbcTemplate::qtransactional()
{
	
	mythread->flag = 5;
	mythread->do_sql(mythread->flag);
	if (getdata_res()) {
		mythread->issqltransactionResult = true;
	}
	else
	{
		mythread->issqltransactionResult = false;
	}
	qInfo() << mythread->QDBC_id << "open the transaction!" << mythread->issqltransactionResult;
	//需要阻塞查询结果
}

void QdbcTemplate::qtransactional_clear()
{
	if (mythread->issqltransactionResult == false) {
		mythread->flag = 7;
		mythread->do_sql(mythread->flag);
		qDebug() << mythread->QDBC_id % "rollback the transaction!" << getdata_res();		
	}
	else {
		mythread->flag = 6;
		mythread->do_sql(mythread->flag);
		qDebug() << mythread->QDBC_id %  "commit the transaction!" << getdata_res();
	}
	
}

bool QdbcTemplate::getdata_res()
{
	bool t = true;
	if (mythread->flag == 8) {
		t = false;
	}
	return t;
}
QdbcTemplate::QdbcTemplate(QString flags)
{
	if (flags == "5") {
		QdbcTemplate::singleinstance()->qtransactional();
	}
	else
	{
		this->assert_args("can not use this  class!");
	}
}
void QdbcTemplate::args(QString value)
{
	//第一个输入参数
	Count_arg = this->countString(value);
	this->querytable(value);
	mythread->m_data.append(value);
	this->Out_count = this->In_count =  0;
}
void QdbcTemplate::assert_args(const QString& str)
{
	qCritical(str.toUtf8());
	{
		QMutexLocker lock(&__mutex);
		mythread->m_data.clear();
		mythread->res_data.clear();
		mythread->do_sql(8);
	}
	qFatal(str.toUtf8());
}
QdbcTemplate & QdbcTemplate::operator <(QObject*  value)
{
	QString sql = mythread->m_data[0].toString();

	const QMetaObject *metaObject = value->metaObject();
	int count = metaObject->propertyCount();
	//先算预处理
	int index_str = 0;
	while (true)
	{
			int res  = sql.indexOf("#{", index_str + 2);
			if (res == index_str || res == -1) {
				break;
			}
			index_str = res;
			int lastindex = sql.indexOf('}', index_str);
			QString str = sql.mid(index_str + 2, lastindex - index_str -2);

			QVariant t1;
			bool resfunc = invokefunc(value, str.toUtf8(), t1);
			if (resfunc == false) {
				QString strs = "There is no such function : get" + str;
				assert_args(strs);
			}
			mythread->m_data.append(t1);
			sql.replace("#{"%str%"}", "?");
			
			In_count++;
	}

	//再算字符串拼接
	index_str = 0;
	while (true)
	{
			int res = sql.indexOf("${", index_str + 1);
			if (res == index_str || res == -1) {
				break;
			}
			index_str = res;
			int lastindex = sql.indexOf('}', index_str);
			QString str = sql.mid(index_str + 2, lastindex - index_str - 2);

			QVariant t1;
			bool resfunc = invokefunc(value, str.toUtf8(), t1);
			if (resfunc == false) {
				QString strs = "There is no such function : get" + str;
				assert_args(strs);
			}
			mythread->m_data.append(t1);
			sql.replace("#{"%str%"}", t1.toString());
	}
	mythread->m_data[0] = sql;
	return *this;

}
QdbcTemplate & QdbcTemplate::operator<(QObject & value)
{
	QString sql = mythread->m_data[0].toString();

	const QMetaObject *metaObject = value.metaObject();
	int count = metaObject->propertyCount();
	//先算预处理
	int index_str = 0;
	while (true)
	{
		int res = sql.indexOf("#{", index_str + 2);
		if (res == index_str || res == -1) {
			break;
		}
		index_str = res;
		int lastindex = sql.indexOf('}', index_str);
		QString str = sql.mid(index_str + 2, lastindex - index_str - 2);

		QVariant t1;
		bool resfunc = invokefunc(&value, str.toUtf8(), t1);
		if (resfunc == false) {
			QString strs = "There is no such function : get" + str;
			assert_args(strs);
		}
		mythread->m_data.append(t1);
		sql.replace("#{"%str%"}", "?");

		In_count++;
	}

	//再算字符串拼接
	index_str = 0;
	while (true)
	{
		int res = sql.indexOf("${", index_str + 1);
		if (res == index_str || res == -1) {
			break;
		}
		index_str = res;
		int lastindex = sql.indexOf('}', index_str);
		QString str = sql.mid(index_str + 2, lastindex - index_str - 2);

		QVariant t1;
		bool resfunc = invokefunc(&value, str.toUtf8(), t1);
		if (resfunc == false) {
			QString strs = "There is no such function : get" + str;
			assert_args(strs);
		}
		mythread->m_data.append(t1);
		sql.replace("#{"%str%"}", t1.toString());
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator <(const QString& value)
{
	args(value);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value);
		}
	}
	else
	{
		int right =  sql.indexOf("}", res);
		sql.replace(res,right - res + 1, "?");
		mythread->m_data.append(value);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator <(int value)
{
	QString value_str = QString::number(value);
	this->getselfAnalysis()->findchild(value_str,value);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value_str);
		}
	}
	else
	{
		int right = sql.indexOf("}", res);
		sql.replace(res, right - res + 1, "?");
		mythread->m_data.append(value);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator<(bool value)
{
	QString value_str = QString::number(value);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value_str);
		}
	}
	else
	{
		int right = sql.indexOf("}", res);
		sql.replace(res, right - res + 1, "?");
		mythread->m_data.append(value);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator<(QDateTime & date)
{
	QString value_str = date.toString(Qt::ISODate);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value_str);
		}
	}
	else
	{
		int right = sql.indexOf("}", res);
		sql.replace(res, right - res + 1, "?");
		mythread->m_data.append(date);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator >(QString & value)
{
	//第一个输出参数
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id %"[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count - 1) % "sql语句参数：" % QString::number(Count_arg - 2);
			assert_args(str);
			return *this;
		}
		qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		mythread->do_sql(mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "只能有一个输出";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id %  "数据库遇到了错误...";
		return *this;
	}
	value = "";
	if (mythread->flag == 3) {
		if (this->Out_count < mythread->m_data.size()) {
			value = mythread->res_data[this->Out_count][1].toString();
		}
	}
	qInfo() << mythread->QDBC_id << "Qdbc perpare end>";
	this->Out_count++;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator > (int & value)
{
	//第一个输出参数
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id %"[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count - 1) % "sql语句参数：" % QString::number(Count_arg - 2);
			assert_args(str);
			return *this;
		}
		qInfo() << mythread->QDBC_id  <<  "[QDBC] Qdbc perpare start<...";
		mythread->do_sql(mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "只能有一个输出";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id  % "数据库遇到了错误...";
		value = -1;
		return *this;
	}
	bool t = false;
	if (this->Out_count < mythread->res_data.size()) {
		value = mythread->res_data[this->Out_count][1].toInt();
	}
	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
	this->Out_count++;
	return *this;
}
QdbcTemplate& QdbcTemplate::operator > (bool& value) {
	//第一个输出参数
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id %  "[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 2);
			assert_args(str);
			return *this;
		}
		qInfo() << mythread->QDBC_id << "[QDBC] Qdbc perpare start<...";
		mythread->do_sql(mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "只能有一个输出";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id  % "数据库遇到了错误...";
		value = false;
		return *this;
	}
	bool t = false;
	if (this->Out_count < mythread->res_data.size()) {
		value = mythread->res_data[this->Out_count][1].toInt();
	}
	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
	Out_count++;
	return *this;
}
bool QdbcTemplate::getDabaseConnect()
{
	this->select("select 1");
	bool t = false;
	if (mythread->flag == 8) {
		t = false;
	}
	else
	{
		mythread->flag = 1;
		mythread->do_sql(mythread->flag);
		t = getdata_res();
	}
	clear();
	return t;
	
}

bool QdbcTemplate::invokefunc(QObject * value, QByteArray name, QVariant & t)
{
	QByteArray gets = "get" % name;
	QByteArray getss = "get" %name % "()";
	char* gets_ptr = getss.data();
	const QMetaObject* theMetaObject = value->metaObject();
	for (int i = 5; i < theMetaObject->methodCount(); i++)
	{
		QMetaMethod oneMethod = theMetaObject->method(i);
		if (strcmp(gets_ptr, oneMethod.methodSignature()) == 0) {
			/*	qDebug() << "typeName: " << oneMethod.typeName();
				qDebug() << "signature: " << oneMethod.methodSignature();
				qDebug() << "methodType: " << oneMethod.methodType();;
				qDebug() << "parameterNames: " << oneMethod.parameterNames() << "\n";*/
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
	else if(str == "QString")
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
		assert_args( "undef type!!!");
		resfunc = false;
	}
	return resfunc;
}
int QdbcTemplate::invokefunc(QObject * value, QByteArray& name,QByteArray& keyname)
{
	QString tbname = name;
	
	int flag = 0;
	const QMetaObject* theMetaObject = value->metaObject();
	QByteArray funcName;
	for (int i = 5; i < theMetaObject->methodCount(); i++)
	{
		QMetaMethod oneMethod = theMetaObject->method(i);
		funcName = oneMethod.methodSignature();
		if (funcName.indexOf("__getmember__"%tbname) > 0) {
			break;
		}
	}
	
	int length = funcName.size();
	keyname = funcName.mid(13, length - 13 - 2 - 3);
	int num = 13 + keyname.size() + 2;
	QByteArray arr =  funcName.mid(num,1);
	flag = arr.toInt();

	QByteArray  tablenamefunc = "__set"%keyname;
	bool resfunc = QMetaObject::invokeMethod(value, tablenamefunc);
	flag = 1;
	return flag;
}
QObject * QdbcTemplate::invokefunc(QObject * value,QByteArray& keyname, int index)
{
	QObject* retVal = NULL; 
	if (index == -1) {
		QByteArray tablename = keyname;
		QByteArray tablenamefunc = "__get"%tablename;

		bool resfunc = QMetaObject::invokeMethod(value, tablenamefunc, Q_RETURN_ARG(QObject*, retVal));
	}
	return retVal;
	//return resfunc;
}
void QdbcTemplate::QdbcTemplateClear()
{
	mythread->m_data.clear();
	mythread->do_sql(9);
	delete mythread;
	//if (analysis != NULL) {
	//	delete analysis;
	//}
	ponit_clear();
	
	int id = this->thread_id;
	QdbcTemplate* t = __instance__[this->thread_id];
	
	__mutex.lock();
	__instance__.remove(id);
	__mutex.unlock();
	delete t;
}
void QdbcTemplate::thread_dispatch_flag4()
{
	for each (QdbcTemplate* tmep in __instance__)
	{
		tmep->getselfthread()->flag = 4;
	}
}

QdbcTemplate::~QdbcTemplate()
{
	//QdbcTemplate::singleinstance()->qtransactional_clear();
	
}

void Qconfig::setPath(char* path, char* name)
{
	if (strlen(path) == '0') {
		path = NULL;
	}
	if (strlen(name) == 0) {
		name = NULL;
	}
	fliepath = path;
	filename = name;
}

bool Qconfig::fullfilepath(QString& path)
{
	QString names;
	if (filename == NULL) {
		names = "qjbctemplate.ini";
	}
	else
	{
		names = QString(filename);
	}
	if (fliepath == NULL) {
		fliepath = ".";
	}
	path = QString(fliepath) % "/" %  names;
	QFileInfo t(path);
	bool b =  t.isFile();
	return b;
}

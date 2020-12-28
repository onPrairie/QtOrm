#pragma once

#include <QObject>
#include <QList>
#include "QTemplateThread.h"
#include <QThread>
#include <QCoreApplication>
#include <QAtomicInt>
#include <QMutex>
#include <QStringBuilder>
#include <QMetaMethod>
#include <QFileInfo>
#include <QMultiHash>
#include "analysis_container.h"
#include "qtormlib_global.h"
#include <QStringList>
#include "../include/QdbcConfig.h"
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
class QTORMLIB_EXPORT Object_utils
{
public:
	Object_utils() {

	}
	~Object_utils() {

	}
	template <typename T>
	static QString toString(T* src) {
		QString str = "[ ";
		const QMetaObject *metaObject = src->metaObject();
		int count = metaObject->propertyCount();
		for (int i = 1; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			QString strs = QString(qmeta.name());
			str.append(strs % ":" %  src->property(qmeta.name()).toString() % " ");
		}
		str.append("]");
		return str;
	}
	template <typename T>
	static QString toString(QList<T*>& value) {
		QString str = "[ ";
		for (int i = 0; i < value.size(); i++) {
			const QMetaObject *metaObject = value[i]->metaObject();
			int count = metaObject->propertyCount();
			for (int j = 1; j < count; j++) {
				QMetaProperty qmeta = metaObject->property(j);
				QString strs = QString(qmeta.name());
				str.append(strs % ":" %  value[i]->property(qmeta.name()).toString() % " ");
			}
			str.append(" && ");
		}
		str.append("]");
		return str;
	}
	template <typename T>
	static void copy(T* src,T* dec) {
		const QMetaObject *metaObject = src->metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			src->setProperty(qmeta.name(),dec->property(qmeta.name()));
		}
	}
	template <typename T>
	static void clear(T* & data) {
		const QMetaObject *metaObject = data->metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			data->setProperty(qmeta.name(), NULL);
		}
	}

	template <typename T>
	static void clear(T& data) {
		const QMetaObject *metaObject = data.metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			data.setProperty(qmeta.name(), NULL);
		}
	}
	template <typename T>
	static bool isNULL(T& data) {
		return !isClear(data);
	}

	template <typename T>
	static bool isClear(T& data) {
		const QMetaObject *metaObject = data.metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			QVariant t = data.property(qmeta.name());
			if (QMetaType::QDateTime == t.type()) {
				QDateTime time = t.toDateTime();
				bool s = time.isNull();
				if (s == true)  return false;
			}
			else
			{
				if (t != NULL) {
					return false;
				}
			}
		}
		return true;
	}
private:

};
class QTORMLIB_EXPORT Qconfig {
public:
	void setPath(char* path, char* name);
	bool fullfilepath(QString& path);
};
struct  ElemsXmlObj  //对象的属性
{
	int type; // object 0x01 list 0x02	
	int n; //线性结构分配
	QObject* value; //存值
	QStringList parents; //指向父辈们的链表
};
struct  ElemsXml	//属性的属性
{
	QString property_name;
	int level;  //属于的层级
	int flag;  //输入1:list 还是 2:object
	int n; //线性结构数据的大小
	bool isId; //是否为id
	QString id_value; //存id
	//QObject* value; //存值
	QString parent; //指向父辈们的链表
};

/*当前的一下配置信息*/
static int atom;
static QMutex __mutex;
static char* fliepath;
static char* filename;

class QTORMLIB_EXPORT QdbcTemplate : public QObject
{
	Q_OBJECT
private:
	int thread_id;

	bool isMyclass = true; //是否为自定义类，对于自定义类，置为true 需要继承Object_jdbc。默认为false。

	int Out_count_row = 0;	//行
	int	Out_count = 0;		//列

	int In_count = 0;		//有多个个参数列表
	int Count_arg = 0; //有多个是？
	
	//int change_in; //1,2  判断 (Qselect < t > t) 格式
	int countString(QString str);
	QHash<QString,ElemsXml> tablenames;
	QHash<QString, ElemsXmlObj*> tablenameobjs;
	//QHash<QString, ElemsXmlObj*> tablename_objs;
	void analysis_macro (char* str);
	bool isUseUnion = false;
	//对于关联查询，不支持select *
	void querytable(QString& str);
	//********************
	int Loglevel = 0;
	//int timeout = 4;
	QTmeplate* mythread;
	Analysis_container* analysis = NULL;
	
	bool isautomemory;

	QString Va_data = ""; 

	void clear() {
		this->In_count =  0;
		mythread->res_data.clear();
		mythread->res_data.clear();
	}
public:
	QdbcTemplate& select(const QString& sql) {
		ponit_clear();
		mythread->flag = 1;
		mythread->m_data.clear();
		args(sql);
		return *this;
	}
	QdbcTemplate& update(const QString& sql) {
		ponit_clear();
		mythread->flag = 2;
		mythread->m_data.clear();
		args(sql);
		return *this;
	}
	QdbcTemplate& Validation_collection(char* sql);
	void  wait_initdatabase() {

	}
	QTmeplate* getselfthread() {
		return mythread;
	}
	Analysis_container* getselfAnalysis() {
		if (analysis == NULL) {
			analysis = new Analysis_container;
		}
		return analysis;
	}
	void deleteselfAnalysis() {
		if (analysis != NULL) {
			delete analysis;
			analysis = NULL;
		}
	}
	//只能检查数据是否连接正常
	bool getDabaseConnect();


	static	QdbcTemplate*  singleinstance() {
		int threadid =  quintptr(QThread::currentThreadId());
		if (!__instance__.contains(threadid)) {
			QMutexLocker lock(&__mutex);
			QObject * o = NULL;
			QdbcTemplate* t = new QdbcTemplate(o);
			t->thread_id = threadid;
			__instance__[threadid] = t;
			//检陋，未初始化的线程
		}
		return __instance__[threadid];
	}
	static	QHash<int, QdbcTemplate*>  Allinstance() {
		return __instance__;
	}

public:
	void qtransactional();
	void qtransactional_clear();
	bool getdata_res();		
	QdbcTemplate(QObject *parent);

	QdbcTemplate(QString falgs);
	QdbcTemplate& arg(int a) {
		return *this;
	}
	//mysql查看当前连接  show FULL PROCESSLIST
	void args(QString value = "");
	void assert_args(const QString &str);
	//参数输入**********************************************
	QdbcTemplate& operator < (QObject* value);
	QdbcTemplate& operator < (QObject& value);
	QdbcTemplate& operator < (QDateTime& date);
	QdbcTemplate& operator < (const QString& value);
	QdbcTemplate& operator < (int value);
	QdbcTemplate& operator < (bool value);
	//参数输出**********************************************
	QdbcTemplate& operator > (int& value);
	QdbcTemplate& operator > (bool& value);
	QdbcTemplate& operator > (QString& value);

	template <typename T>
	QdbcTemplate& operator > (T* &value);

	template <typename T>
	QdbcTemplate& operator > (T &value);

	template <typename T>
	QdbcTemplate& operator > (QList<T*>& value);

	bool invokefunc(QObject*  value,QByteArray name,QVariant& t);
	//retrun 0: 对象  return 1：QList<对象> 并分配内存
	//name 为 key	keyname 为定义的class name 
	int invokefunc(QObject* value, QByteArray& name, QByteArray& keyname);
	int get_invokefunc_type(QObject * value, QByteArray & keyname);
	QObject* get_invokefunc(QObject * value, QByteArray& keyname, int index);
	void set_invokefunc(QObject * value, QByteArray & keyname);
	//线程分发
	void thread_dispatch_flag4();

	QHash<int,QObject*> adress;

	/****c code****/
	

	template <typename T>
	T* New_adress() {
		T* t1 = new T;
		adress[(int)t1] = t1;
		return t1;
	}
	template <typename T>
	void delete_address(T* t1) {
		delete t1;
		this->adress.remove((int)t1);
	}
	void hashclear() {
		if (isautomemory == true) {
			QHash<int, QObject*>::const_iterator i;
			for (i = adress.constBegin(); i != adress.constEnd(); ++i) {
				QObject* t = i.value();
				delete t;
			}
		}
		adress.clear();
	}
	void ponit_clear() {
		hashclear();
		deleteselfAnalysis();
	}
	void QdbcTemplateClear();
	~QdbcTemplate();
private:
	static QHash<int,QdbcTemplate*> __instance__;
};
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator>(T & value)
{
	Object_utils::clear(value);
	//第一个输出参数
	if (isUseUnion == true) {
		//analysis_macro(sql);
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
				qFatal(str.toUtf8());
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
			qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
			return *this;
		}
		int ids[QDeep_ASSOCIATION][QDeep_ASSOCIATION]; //最大能关联5条	
		int ids_length[QDeep_ASSOCIATION]; //col count
		memset(&ids, -1, sizeof(ids)); //clear
		memset(&ids_length,0, sizeof(ids_length)); //clear
		
		int deep_ass = tablenameobjs.size();
		if (deep_ass > QDeep_ASSOCIATION -1) {
			assert_args("out of range:" + QString::number(tablenameobjs.size())  + QString::number(QDeep_ASSOCIATION - 1));
		}
		while (this->Out_count < mythread->res_data.size()) {
			QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
			if (!tablenames.contains(key)) {
				assert_args("Duplicate fields " % QString(key));
			}
			ElemsXml el = tablenames[key];
			ElemsXmlObj* objxx,* objx;
			objxx = objx = tablenameobjs[el.parent];
			if(Out_count == 0)
			{
				if (el.isId == true) {
					for (int i = 0; i < deep_ass; i++) {
						if (ids[el.level][i] == -1) {
							ids[el.level][i] = this->Out_count_row;
							ids_length[el.level] ++;
							break;
						}
					}
				}

					QObject* t1 = &value;
					if (el.level == 0) {
						ElemsXmlObj* objx = tablenameobjs[el.parent];
						if (objx->value == NULL) {
							objx->value = t1;
						}
					}
					for (int i = 1; i < objxx->parents.size(); i++) {
						QString partentanme = objxx->parents[i];
						objx = tablenameobjs[partentanme];
						if (objx->value == NULL) {
							int type = this->get_invokefunc_type(t1, partentanme.toUtf8());
							objx->type = type;
							this->set_invokefunc(t1, partentanme.toUtf8());
							QObject * obj;
							if (objx->type == 2) {
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), 0);
								objx->n = 0;
							}
							else
							{
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
								objx->n = -1;
							}
							Object_utils::clear(obj);
							adress[(int)obj] = obj;
							t1 = obj;
							objx->value = t1;
						}
						else
						{
							QObject * obj;
							if (objx->type == 2) {
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), 0);
							}
							else
							{
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
							}
							t1 = obj;
						}
					}
			}
			bool b = objx->value->setProperty(el.property_name.toUtf8(), mythread->res_data[this->Out_count][this->Out_count_row + 1]);
			if (b == false) {
				assert_args("col error:" + el.property_name);
			}
			this->Out_count_row += 2;
			if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
				this->Out_count_row = 0;
				this->Out_count++;
				ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] = 1;

				if (ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] == 1) {
					int rows = 0;
					for (int i = 0; i < QDeep_ASSOCIATION - 1; i++) {
						if (ids_length[i] == 0) {
							ids_length[QDeep_ASSOCIATION - 1] = i;
							break;
						}
					}
					
					for (int i = 0; i < ids_length[QDeep_ASSOCIATION - 1]; i++) {
						for (int j = 0; j < ids_length[i]; j++) {
							if (ids[i][j] == -1) {
								assert_args("!!! no colnum id,There are related IDs that are not queried in SQL statements");
							}
						}
					}
				}
				ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] = 2;
				
				for (int i = 0; i < ids_length[QDeep_ASSOCIATION - 1]; i++) {
					for (int j = 0; j < ids_length[i]; j++) {
						QVariant prev_id = mythread->res_data[this->Out_count - 1][ids[i][j] + 1];
						QVariant current_id = mythread->res_data[this->Out_count][ids[i][j] + 1];
						if (prev_id != current_id) {
							if (i == 0) {
								qWarning() << QWarn << "The ID is different, but the object is used";
								qDeleteAll(tablenameobjs);
								tablenameobjs.clear();
								tablenames.clear();
								qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
								this->Out_count++;
								return *this;
							}

							//id一样 组合分配内存 适合Qobject
							//id不一样  分割分配内存 适合线性结构
							QByteArray key = mythread->res_data[this->Out_count][ids[i][j]].toByteArray();
							ElemsXml el = tablenames[key];
							ElemsXmlObj* objxx, *objx;
							objxx = objx = tablenameobjs[el.parent];
							int n = ++objx->n;
							int type = objx->type;

							QString partentanme = objx->parents[objx->parents.size() - 1];
							QString precname = objx->parents[objx->parents.size() - 2];
							//mian alloc
							objx = tablenameobjs[precname];
							QObject* t1 = objx->value;
							this->set_invokefunc(t1, el.parent.toUtf8());

							QObject * obj;
							if (type == 2) {
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), n);
							}
							else
							{
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
							}
							adress[(int)obj] = obj;
							Object_utils::clear(obj);
							t1 = obj;
							objxx->value = t1;
						}
					}
				}
			}
		}
		qDeleteAll(tablenameobjs);
		tablenameobjs.clear();
		tablenames.clear();
		qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
		this->Out_count++;
		return *this;
	}
	else
	{
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
				qFatal(str.toUtf8());
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
			qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
			return *this;
		}
		while (this->Out_count < mythread->res_data.size()) {
			QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
			bool s =value.setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
			if (s == false) {
				assert_args("can not set Property!");
			}

			this->Out_count_row += 2;
			if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
				this->Out_count_row = 0;
				this->Out_count++;
				break;
			}
		}
		qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
		this->Out_count++;
		return *this;
	}
}
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator>(T *& value)
{
	if (isUseUnion == true) {
			//第一个输出参数
			if (Out_count == 0) {
				if (In_count != Count_arg) {
					QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
					qFatal(str.toUtf8());
					value = NULL;
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
				value = NULL;
				qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
				return *this;
			}
			int size = mythread->res_data.size();
			if (size == 0) {
				value = NULL;
				return *this;
			}
			//!!!diff 复制要小心 inline QdbcTemplate & QdbcTemplate::operator>(T & value)
			if (value == NULL) {
				value = New_adress<T>();
				Object_utils::clear(value);
			}
			else
			{
				Object_utils::clear(value);
			}
			int ids[QDeep_ASSOCIATION][QDeep_ASSOCIATION]; //最大能关联5条	
			int ids_length[QDeep_ASSOCIATION]; //col count
			memset(&ids, -1, sizeof(ids)); //clear
			memset(&ids_length, 0, sizeof(ids_length)); //clear

			int deep_ass = tablenameobjs.size();
			if (deep_ass > QDeep_ASSOCIATION - 1) {
				assert_args("out of range:" + QString::number(tablenameobjs.size()) + QString::number(QDeep_ASSOCIATION - 1));
			}
			while (this->Out_count < mythread->res_data.size()) {
				QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
				if (!tablenames.contains(key)) {
					assert_args("Duplicate fields " % QString(key));
				}
				ElemsXml el = tablenames[key];
				ElemsXmlObj* objxx, *objx;
				objxx = objx = tablenameobjs[el.parent];
				if (Out_count == 0)
				{
					if (el.isId == true) {
						for (int i = 0; i < deep_ass; i++) {
							if (ids[el.level][i] == -1) {
								ids[el.level][i] = this->Out_count_row;
								ids_length[el.level] ++;
								break;
							}
						}
					}

					QObject* t1 = value;
					if (el.level == 0) {
						ElemsXmlObj* objx = tablenameobjs[el.parent];
						if (objx->value == NULL) {
							objx->value = t1;
						}
					}
					for (int i = 1; i < objxx->parents.size(); i++) {
						QString partentanme = objxx->parents[i];
						objx = tablenameobjs[partentanme];
						if (objx->value == NULL) {
							int type = this->get_invokefunc_type(t1, partentanme.toUtf8());
							objx->type = type;
							this->set_invokefunc(t1, partentanme.toUtf8());
							QObject * obj;
							if (objx->type == 2) {
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), 0);
								objx->n = 0;
							}
							else
							{
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
								objx->n = -1;
							}
							Object_utils::clear(obj);
							adress[(int)obj] = obj;
							t1 = obj;
							objx->value = t1;
						}
						else
						{
							QObject * obj;
							if (objx->type == 2) {
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), 0);
							}
							else
							{
								obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
							}
							t1 = obj;
						}
					}
				}
				bool b = objx->value->setProperty(el.property_name.toUtf8(), mythread->res_data[this->Out_count][this->Out_count_row + 1]);
				if (b == false) {
					assert_args("col error:" + el.property_name);
				}
				this->Out_count_row += 2;
				if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
					this->Out_count_row = 0;
					this->Out_count++;
					ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] = 1;

					if (ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] == 1) {
						int rows = 0;
						for (int i = 0; i < QDeep_ASSOCIATION - 1; i++) {
							if (ids_length[i] == 0) {
								ids_length[QDeep_ASSOCIATION - 1] = i;
								break;
							}
						}

						for (int i = 0; i < ids_length[QDeep_ASSOCIATION - 1]; i++) {
							for (int j = 0; j < ids_length[i]; j++) {
								if (ids[i][j] == -1) {
									assert_args("!!! no colnum id,There are related IDs that are not queried in SQL statements");
								}
							}
						}
					}
					ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] = 2;

					for (int i = 0; i < ids_length[QDeep_ASSOCIATION - 1]; i++) {
						for (int j = 0; j < ids_length[i]; j++) {
							QVariant prev_id = mythread->res_data[this->Out_count - 1][ids[i][j] + 1];
							QVariant current_id = mythread->res_data[this->Out_count][ids[i][j] + 1];
							if (prev_id != current_id) {
								if (i == 0) {
									qWarning() << QWarn << "The ID is different, but the object is used";
									qDeleteAll(tablenameobjs);
									tablenameobjs.clear();
									tablenames.clear();
									qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
									this->Out_count++;
									return *this;
								}

								//id一样 组合分配内存 适合Qobject
								//id不一样  分割分配内存 适合线性结构
								QByteArray key = mythread->res_data[this->Out_count][ids[i][j]].toByteArray();
								ElemsXml el = tablenames[key];
								ElemsXmlObj* objxx, *objx;
								objxx = objx = tablenameobjs[el.parent];
								int n = ++objx->n;
								int type = objx->type;

								QString partentanme = objx->parents[objx->parents.size() - 1];
								QString precname = objx->parents[objx->parents.size() - 2];
								//mian alloc
								objx = tablenameobjs[precname];
								QObject* t1 = objx->value;
								this->set_invokefunc(t1, el.parent.toUtf8());

								QObject * obj;
								if (type == 2) {
									obj = this->get_invokefunc(t1, partentanme.toUtf8(), n);
								}
								else
								{
									obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
								}
								adress[(int)obj] = obj;
								Object_utils::clear(obj);
								t1 = obj;
								objxx->value = t1;
							}
						}
					}
				}
			}
			qDeleteAll(tablenameobjs);
			tablenameobjs.clear();
			tablenames.clear();
			qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
			this->Out_count++;
			return *this;
	}
	else {
		if (value == NULL) {
			//第一个输出参数
			if (Out_count == 0) {
				if (In_count != Count_arg) {
					QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
					qFatal(str.toUtf8());
					value = NULL;
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
				value = NULL;
				qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
				return *this;
			}
			int size = mythread->res_data.size();
			if (size == 0) {
				value = NULL;
				return *this;
			}
			value = New_adress<T>();
			Object_utils::clear(value);
			while (this->Out_count < mythread->res_data.size()) {
				QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
				value->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

				this->Out_count_row += 2;
				if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
					this->Out_count_row = 0;
					this->Out_count++;
					break;
				}
			}
			qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
			this->Out_count++;
			return *this;
		}
		else
		{
			//第一个输出参数
			if (Out_count == 0) {
				if (In_count != Count_arg) {
					QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
					qFatal(str.toUtf8());
					value = NULL;
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
				value = NULL;
				qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
				return *this;
			}
			int size = mythread->res_data.size();
			if (size == 0) {
				value = NULL;
				return *this;
			}
			//value = New_adress<T>();
			Object_utils::clear(value);
			while (this->Out_count < mythread->res_data.size()) {
				QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
				value->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

				this->Out_count_row += 2;
				if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
					this->Out_count_row = 0;
					this->Out_count++;
					break;
				}
			}
			qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
			this->Out_count++;
			return *this;
		}
	}
}
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator >(QList<T*>& value)
{
	if (isUseUnion == true) {
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id  % "[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count - 1) % "sql语句参数：" % QString::number(Count_arg - 2);
				assert_args(str);
				value.clear();
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
		T* t11 = New_adress<T>();
		Object_utils::clear(t11);
		int ids[QDeep_ASSOCIATION][QDeep_ASSOCIATION]; //最大能关联5条	
		int ids_length[QDeep_ASSOCIATION]; //col count
		memset(&ids, -1, sizeof(ids)); //clear
		memset(&ids_length, 0, sizeof(ids_length)); //clear

		int deep_ass = tablenameobjs.size();
		if (deep_ass > QDeep_ASSOCIATION - 1) {
			assert_args("out of range:" + QString::number(tablenameobjs.size()) + QString::number(QDeep_ASSOCIATION - 1));
		}
		bool restart = true;
		while (this->Out_count < mythread->res_data.size()) {
			QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
			if (!tablenames.contains(key)) {
				assert_args("Duplicate fields " % QString(key));
			}
			ElemsXml el = tablenames[key];
			ElemsXmlObj* objxx, *objx;
			objxx = objx = tablenameobjs[el.parent];
			if (restart == true)
			{
				if (el.isId == true) {
					for (int i = 0; i < deep_ass; i++) {
						if (ids[el.level][i] == -1) {
							ids[el.level][i] = this->Out_count_row;
							ids_length[el.level] ++;
							break;
						}
					}
				}

				QObject* t1 = t11;
				if (el.level == 0) {
					ElemsXmlObj* objx = tablenameobjs[el.parent];
					if (objx->value == NULL) {
						objx->value = t1;
					}
				}
				for (int i = 1; i < objxx->parents.size(); i++) {
					QString partentanme = objxx->parents[i];
					objx = tablenameobjs[partentanme];
					if (objx->value == NULL) {
						int type = this->get_invokefunc_type(t1, partentanme.toUtf8());
						objx->type = type;
						this->set_invokefunc(t1, partentanme.toUtf8());
						QObject * obj;
						if (objx->type == 2) {
							obj = this->get_invokefunc(t1, partentanme.toUtf8(), 0);
							objx->n = 0;
						}
						else
						{
							obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
							objx->n = -1;
						}
						Object_utils::clear(obj);
						adress[(int)obj] = obj;
						t1 = obj;
						objx->value = t1;
					}
					else
					{
						QObject * obj;
						if (objx->type == 2) {
							obj = this->get_invokefunc(t1, partentanme.toUtf8(), 0);
						}
						else
						{
							obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
						}
						t1 = obj;
					}
				}
			}
			bool b = objx->value->setProperty(el.property_name.toUtf8(), mythread->res_data[this->Out_count][this->Out_count_row + 1]);
			if (b == false) {
				assert_args("col error:" + el.property_name);
			}
			this->Out_count_row += 2;
			if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
				this->Out_count_row = 0;
				this->Out_count++;
				if (this->Out_count == mythread->res_data.size()) {
					value.append(t11);
					break;
				}


				ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] = 1;

				if (ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] == 1) {
					int rows = 0;
					for (int i = 0; i < QDeep_ASSOCIATION - 1; i++) {
						if (ids_length[i] == 0) {
							ids_length[QDeep_ASSOCIATION - 1] = i;
							break;
						}
					}

					for (int i = 0; i < ids_length[QDeep_ASSOCIATION - 1]; i++) {
						for (int j = 0; j < ids_length[i]; j++) {
							if (ids[i][j] == -1) {
								assert_args("!!! no colnum id,There are related IDs that are not queried in SQL statements");
							}
						}
					}
				}
				ids[QDeep_ASSOCIATION - 1][QDeep_ASSOCIATION - 1] = 2;
				restart = false;
				for (int i = 0; i < ids_length[QDeep_ASSOCIATION - 1]; i++) {
					for (int j = 0; j < ids_length[i]; j++) {
						QVariant prev_id = mythread->res_data[this->Out_count - 1][ids[i][j] + 1];
						QVariant current_id = mythread->res_data[this->Out_count][ids[i][j] + 1];
						if (prev_id != current_id) {
								//id一样 组合分配内存 适合Qobject
								//id不一样  分割分配内存 适合线性结构
								QByteArray key = mythread->res_data[this->Out_count][ids[i][j]].toByteArray();
								ElemsXml el = tablenames[key];
								ElemsXmlObj* objxx, *objx;
								objxx = objx = tablenameobjs[el.parent];
								if (i == 0) {
									value.append(t11);
									t11 = New_adress<T>();
									Object_utils::clear(t11);
									for each (ElemsXmlObj* elem in tablenameobjs)
									{
										elem->value = NULL;
									}
									objxx->value = t11;
									restart = true;
									break;
								}
								else
								{
									int n = ++objx->n;
									int type = objx->type;

									QString partentanme = objx->parents[objx->parents.size() - 1];
									QString precname = objx->parents[objx->parents.size() - 2];
									//mian alloc
									objx = tablenameobjs[precname];
									QObject* t1 = objx->value;
									this->set_invokefunc(t1, el.parent.toUtf8());

									QObject * obj;
									if (type == 2) {
										obj = this->get_invokefunc(t1, partentanme.toUtf8(), n);
									}
									else
									{
										obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
									}
									adress[(int)obj] = obj;
									Object_utils::clear(obj);
									t1 = obj;
									objxx->value = t1;
								}
						}
					}
					if (restart == true) {
						break;
					}
				}

			}
		}
		qDeleteAll(tablenameobjs);
		tablenameobjs.clear();
		tablenames.clear();
		qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
		this->Out_count++;
		return *this;
	}
	else
	{
		//第一个输出参数
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id  % "[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count - 1) % "sql语句参数：" % QString::number(Count_arg - 2);
				assert_args(str);
				value.clear();
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
		T* t1 = New_adress<T>();
		Object_utils::clear(t1);
		while (this->Out_count < mythread->res_data.size()) {
			QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
			t1->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

			this->Out_count_row += 2;
			if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
				this->Out_count_row = 0;
				this->Out_count++;
				value.append(t1);
				t1 = New_adress<T>();
				Object_utils::clear(t1);
			}
			if (this->Out_count == mythread->res_data.size()) {
				delete_address(t1);
				qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
				this->Out_count++;
			}
		}
		return *this;
		// TODO: 在此处插入 return 语句
	}

}

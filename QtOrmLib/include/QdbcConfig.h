#pragma once
#define  QMaxLength_Qresults 1024
#define  QMaxLength_field  100
#define  QDeep_ASSOCIATION  5  

#define QMajor_version "20.10"
#define Qminor_version "016"
#define Qstable   "s"
#define Qtest     "c"

#define  Qversion QMajor_version "." Qminor_version  Qtest

#define  QMysql "QMYSQL"
#define  QOracle "QOCI"
#define  QSqlite3 "QSQLITE"  
#define  QSqlite2 "QSQLITE2"

#define QWarn  "!!!"

#define  __OBJECT 0x01
#define  __LIST   0x02
// __ -》 hide  函数以__开头的视为隐藏，不能调用
#define Q_ATTR(T,member) \
Q_PROPERTY(T member READ get##member WRITE set##member) \
public: \
	Q_INVOKABLE T get##member() { return member;} \
	Q_INVOKABLE void set##member(T t) {member = t;} \
	Q_INVOKABLE char* __getretrunname__##member() { return #T;} \
	T member;\


// __ -》 hide  函数以__开头的视为隐藏，不能调用
#define Q_ATTR_ALIAS(T,member,alias) \
	Q_PROPERTY(T alias READ get##member WRITE set##member) \
public:	\
	Q_INVOKABLE char* __getretrunname__##member() { return #T;} \
	Q_INVOKABLE T get##member() { return member;} \
	Q_INVOKABLE void set##member(T t) {member = t;} \
	T member; 

// __ -》 hide  函数以__开头的视为隐藏，不能调用
#define Q_ASSOCIATION_LIST(T) \
public: \
	Q_INVOKABLE QObject* __get##T(int n) { return (QObject*)__##T[n];} \
	Q_INVOKABLE void __set##T() {T* mem = new T;__##T.append(mem);} \
	Q_INVOKABLE int __getmembertype__##T() {return __LIST;} \
	QList<T*> get##T(){return __##T;} \
private: \
	QList<T*> __##T;

// __ -》 hide 函数以__开头的视为隐藏，不能调用
#define Q_ASSOCIATION_OBJECT(T2) \
public: \
	Q_INVOKABLE QObject* __get##T2(int _n) { return (QObject*)__##T2;} \
	Q_INVOKABLE void __set##T2() {__##T2 = new T2;} \
	Q_INVOKABLE int __getmembertype__##T2() {return __OBJECT;} \
	T2* get##T2(){return __##T2;} \
private: \
	T2* __##T2;


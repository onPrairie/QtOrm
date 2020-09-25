# Qdbc 使用手册

** 此操作数据库框架为类mybatis,如果之前使用过此类框架，则会比较熟悉一些,所有的sql操作都采用宏函数的进行操作，方便简单。
此框架目前为测试版本，非稳定版本。版本查看：在Qdbc.h中的Qversion宏,你可以打印此宏，如果看到版本中有c,为测试版本。为s,则为稳定版本
此框架命名为Qdbc，如果有重名，则立刻修改。**



### Contents
- [1,快速开始](#1快速开始)
- [2,占位符与宏格式](#2占位符与宏格式)
- [3,orm 类定义](#3orm-类定义)
- [4,工具类：Object_utils](#4工具类object_utils)
- [5,配置文件：qjbctemplate.ini](#5配置文件qjbctemplateini)
- [6,结束](#6结束)

## 1,快速开始

1. 将Qdbc文件夹拷贝到新创建的工程目录中

2. 建立一个含qt的工程并且包含于SQL模块

3. 添加"Qdbc.h"头文件导入其中即可快速开苏

4. 导入laneip表（网络检测表），数据库一定要utf-8格式，测试的数据表如下

   ```
   CREATE TABLE NewTable (
   id  int(11) NOT NULL AUTO_INCREMENT ,
   ip  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
   port  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
   url  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
   entryno  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
   etype  int(255) NULL DEFAULT NULL ,
   Status  int(255) NULL DEFAULT 1 ,
   description  varchar(600) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
   updatetime  timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP ,
   PRIMARY KEY (id)
   )
   ENGINE=InnoDB
   DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
   AUTO_INCREMENT=7
   ROW_FORMAT=COMPACT;
   ```

   表结构解释：

   | id          | 默认自增长                                                   |
   | :---------- | :----------------------------------------------------------- |
   | 字段名      | 描述                                                         |
   | ip          | 网站的ip                                                     |
   | port        | 网站的端口                                                   |
   | url         | 网站的url                                                    |
   | entryno     | 网站的地点编号（‘bj’：北京，'sh'：上海，'sz'：深圳，'js'：江苏） |
   | etype       | 网站种类（0：‘ 企业网站 ’，1：‘ 行业网站 ’，2：‘ B2B电子商务网站 ’） |
   | description | 网络状态描述 （‘网络异常,连接超时’，‘网络正常’）             |
   | updatetime  | 更新事件，如果不填，则为当前时间                             |

   5. 插入一些数据，能查询到对应的ip

```c
		#include <QtCore/QCoreApplication>
		#include "Qdbc\Qdbc.h"
		int main(int argc, char *argv[])
		{
			QCoreApplication a(argc, argv);
			QString ip;
			Qselect("SELECT ip from laneip WHERE id = 2") > ip;
			return a.exec();
		}
```

## 2,占位符与宏格式

​	目前占位符分两种#{}和${}。#{}和${}与orm的类的属性要完全匹配，如何不匹配，会出现异常。#{}表示一个预处理占位符号，${} 表示简单的拼接sql   

1. 标签查找规则：优先找#{}在找${}，对于${}注意字符串类型需要加单引号

```c
void updatetest()
{
	QString sql = "UPDATE laneip SET updatetime='${updatetime}' WHERE id=${id}";
	int count;
	Qupdate(sql) < QDateTime::currentDateTime() < 2 > count ;
	qDebug() << "update1:" << count;
}	
```
2. 查找出错
   如果查找出错，则int类型为-1，指针类型为NULL,QString为NULL，需看此返回值是否有查询结果冲突。
- 对于Qselect,可根据需求返回,查询不到或者执行报错，指针类型为NULL,值类型为-1。
  
- 对于Qupdate，Qinsert，Qdelete,只返回int类型,执行成功，返回影响的行数。执行报错，返回-1 。  

3. 宏格式

   所有宏格式都在Qdbc中进行查看,采取语义化的标签，尽量方便使用。此为查询语句格式：
   
   - Qselect 表示查询宏：对于查询 Qselect("select * from xxx") < arg1 < arg2 > result; 可以有多个输入,但只能一个输出。<表示输入，相当于函数参数，>表示输出，相当于函数返回值。<不可以指定为list类型，但返回值可以，一般用于查询多条语句。
   
   - 其他语句格式：
   
     ​	Qdelete 表示删除宏	Qinsert 插入宏	Qdelete删除宏，此上三种宏只能有一个输出或不输出。此输出类型一定为int类型。  

4. **Qtransactional事务宏**	  

   ​	默认放在函数的第一行，这样才能照顾到所有的sql操作，一旦某个sql操作出现了错误，则会回滚操作，否则提交，列子如下：

```c
void transactionaltest()
   	{
   			Qtransactional();
   			Laneip lane = Laneip();
   			lane["ip"] = "192.168.1.103";
   			lane["id"] = 2;
   			QString sql = "UPDATE laneip SET ip=#{ip} WHERE id=#{id}";
   			int count;
   			Qupdate("UPDATE laneip SET1 ip=#{ip} WHERE id=#{id}") < lane > count;
   			lane["ip"] = "192.168.1.104";
   			lane["id"] = 2;
   			Qupdate(sql) < lane > count;
   			qDebug() << "update:" << count;
   		}
```
5. **Qif 宏为逻辑判断,相当于if-else**
- Qif(condition,va) 
        condition 为逻辑判断条件,支持&&,||,!  
        va 为此condition条件成立时候，执行va。但va一定为字符串输出
    - Qelif(condition,va)
        condition 为逻辑判断条件,支持&&,||,!  
    	va 为此condition条件成立时候，执行va。但va一定为字符串输出,另外一定先执行Qif
	-Qelse(va)
		va 为所有的condition条件不成立时候，执行va。但va一定为字符串输出，另外一定先执行Qif或Qelif
6. **Qswitch 宏为选择判断，相当于switch-case,此宏需要跟Qcase连用**
	- Qswitch(const1) 
	  const1 为常量，此常量支持字符串或者整形。
	  
	- Qcase(const1,value1)
	  const1 为常量，此常量支持字符串或者整形。value1为匹配到的const1值
	  
  - Qdefault(value1)
	  const1 为常量，此常量支持字符串或者整形。value1为匹配不到的const1值
	
	
7. **Qforeach(begin,separator,end,collection)** 
	
	Qforeach 宏为循环语句，相当于for语句，此宏需单独使用
	
	- begin 参数为字符串，此为collection的开始点
	
	- separator 参数为字符串，此为collection以什么分割
	
	- end  参数为字符串，此为collection的结束点
	
	- collection 此为QString,QList<int>,QStringList类型，QList<int>可以用宏IntList代替
	
6. **Qclear**  

   ​	 Qclear为释放动态申请内存的指针变量,可以手动进行释放，如果不手动释放，申请的内存也会自动释放

7. **QconfigPath(path,name)**

   ​	此宏需要在执行sql语句之前进行配置，否则配置无效。
   
   - path:可以指定相对路径，也可以指定觉得路径，此路径可以是相对路径也可以是绝对路径。对于路径，请用反斜杠进行路径编码。如C:/11/22 。如何填空字符串，则于可执行文件路径相同。
   - name： 指定的配置文件名，如果name为空字符串，则写指定为默认的配置文件名：qjbctemplate.ini。否则以指定的配置文件名来指定。
   
8.  **QBye()**
   
   此宏用于对于在多线程情况下，线程需要不断的销毁于创建，需要在线程结束的时候放置 QBye()
   
9. **QDBC_Id**
   
   此宏用于在多线程下定位哪个线程打印的数据，每次增加一个线程，QDBC_Id就会增加1， 如：qDebug() << QDBC_Id << Object_utils::toString(lane);
   
   
   
   **此方法为测试Qif功能**

```c
		void iftest()
		{
			Laneip* lane;
			QString ip = "192.168.1.104";
			QString sql = "SELECT * FROM laneip WHERE id = "
				Qif(ip == "192.168.1.104", "2")
				Qelse("0");
			Qselect(sql)  > lane;
			if (lane == NULL) {
				qDebug() << "error";
				return;
			}
			qDebug()  << Object_utils::toString(lane);
		}
```

 	**此方法为测试Qswitch功能，此返回的是**

```c
		void switchtest()
		{
			Laneip* lane;
			QString ip = "192.168.1.104";
			QString sql = "SELECT * FROM laneip WHERE id = "
				Qswitch(ip)
				Qcase("192.168.1.100","1")
				Qcase("192.168.1.104","2");
			Qselect(sql)  > lane;
			if (lane == NULL) {
				qDebug() << "error";
				return;
			}
			qDebug()  << Object_utils::toString(lane);
		}
```
**此方法为测试Qforeach功能,此返回是list类型**

```c
		void QHello::foreachtest()
		{
			qDebug() << "QHello::selectAl start";
			QList<Laneip*> lanelist;
			IntList ls;
			ls << 2 << 3;
			QString sql = "SELECT * FROM laneip where id in" Qforeach("(",",",")",ls);
			Qselect(sql) > lanelist;
			qDebug() << QDBC_Id << Object_utils::toString(lanelist);
		}
```
## 3,orm 类定义

1. 假设定义的类型为Laneip,想将类映射到数据库，那就要将类定义成如下格式：

```c
		class Laneip :public QObject
		{
			Q_OBJECT
		public:
			Q_ATTR(int,id)
			Q_ATTR(QString, ip)
			Q_ATTR(int, port)
			Q_ATTR(QString,url)
			Q_ATTR(QString,entryno)
			Q_ATTR(int,etype)
			Q_ATTR(int, Status)
			Q_ATTR(QString, description)
			Q_ATTR(QDateTime,updatetime)
		};
```
​		对于orm定义的类,使用Q_ATTR来包裹定义的成员变量。此宏默认会在其成员中加入get或set方法，并且大小写敏感，禁止使用static变量

2. 属性说明	

 - Q_ATTR(T,member)  用此宏属性为member，类型为T。定义的属性与数据库表字段一一对应，大小写敏感。

 - 此定义的类一定要继承于QObject，并且写上Q_OBJECT宏，不需要写构造函数。

 - ATTR_ALIAS(T,alias,member) 如果不想与数据库对应名字，则需要别名来定义。alias为别名属性， member为数据库表字段，T表示类型

 > ​	别名属性:

如果不想说明数据库的字段名作为属性，则可以使用别名来定义属性。此Q_ATTR_ALIAS(Laneip,lane_url,url)  

例如：Q_ATTR_ALIAS(Laneip,lane_url,url)    属性为lane_url,指定的表字段为url  

> ​	属性访问：

- 读取属性：
 Laneip ip;
  int id =  ip.id;或者int id =  ip.getid();
  
- 写属性：
      Laneip ip;
      ip.id = 11;或者ip.setid(12);    	

​	**属性类型规定：如果想要使用自定义类型，需要继承Object_qdbc，继承之后就可以与标签进行绑定，属性暂时只能规定: 字符串类型 QString   时间类型 QDateTime	值类型   int,bool		更多类型后续增加**  **。而对于自定义的变量需要定义成指针：**

```c
Laneip *lane;
QString sql = "SELECT * FROM laneip WHERE id = #{lid}";
Qselect(sql) < 3 > lane;
```

   		***注意：orm采用的办自动化内存管理,会在适当的时机释放掉此内存，千万不要收到delete,如果想要delete此指针，请使用此宏函数Qclear()。此指针不会保存很长时间，如果想长期保存，需要自己定义变量来保存。***

​	3 . 输入与输出

```c++
假设orm定义的类为T,对于输入（<） 而言，只能为T，对于输出而言，可以为T，或者 Qlist<T*> 类型，*T。
```

​	我们以Laneip的定义类来举例,详见如下表格。而对于指针类型，直接用NULL进行判断，而对于直接定义的类型，需要用	Object_utils::isNULL 进行判断

| 输入（<）   | 输出（<）                                            |
| ----------- | ---------------------------------------------------- |
| Laneip lane | Laneip lane 或者 Laneip* lane或者QList<Laneip*> lane |

​		 

## 4,工具类：	Object_utils

Object_utils 此类将有三个静态成员函数分别为：

```c
				- static QString toString(T* src)   
				- static QString toString(QList<T*>& value)	*
				- static void copy(T* src,T* dec) 
				- static void clear(T& data)
				- static void clear(T* & data)
				- static bool isNULL(T& data)
				- static bool isClear(T& data)																				
```

1. 对于static QString toString(T* src)   )静态成员函数而言，返回的是定义orm类字符串格式化的内容,以"["开头，“]”结尾，中间为类成员咱开。

​		此内容大致为:"[ id:3 ip:192.168.1.100 port:100 url:https://github.com/linuxguangbo/ entryno:111 etype:1 Status:1 description:正常 updatetime:2020-09-08T17:33:55 ]"

```c
       		Laneip* lane;
            QString sql = "SELECT * FROM laneip where id = 2";
            Qselect(sql)  > lane;
            qDebug() << Object_utils::toString(lane);
```
2. 对于static QString toString(T* src)   )静态成员函数而言，返回的是定义orm的QList类字符串格式化的内容，以"["开头，“]”结尾，中间为类成员咱开，如果有多个类成员，则以&分割。

​			此内容大致为:"[ id:1 ip:127.0.0.1 port:5009 url:www.sohu.com entryno:101 etype:0 Status:2 description:网络异常,连接超时 updatetime:2020-04-30T16:54:29  && id:2 ip:192.168.1.104 port:5010 url:www.baidu.com entryno:102 etype:0 Status:1 description:网络异 常,连接超时! updatetime:2020-09-16T11:23:23  && id:3 ip:192.168.1.100 port:100 url:https://github.com/linuxguangbo/ entryno:111 etype:1 Status:1 description:正常 updatetime:2020-09-08T17:33:55  && id:5 ip:192.168.1.112 port:100 url:www.google.com entryno:111 etype:1 Status:1 description:正常 updatetime:2020-04-30T17:24:19  && id:6 ip:192.168.1.112 port:1144 url:www.google.com entryno:2 etype:2 Status:1 description:网络正常 updatetime:2020-09-08T17:24:47  && ]"

```c
                QList<Laneip*> lane;
                QString sql = "SELECT * FROM laneip";
                Qselect(sql)  > lane;
                qDebug() << Object_utils::toString(lane);
```
3. 对于static void copy(T* src,T* dec) 成员而言，为复制定义为orm类的工具。因为继承于Qobject的类禁止拷贝复制，则可以用 copy函数来实现：

```c
				Laneip *lane;
				QString sql = "SELECT * FROM laneip WHERE id = #{lid}";
				Qselect(sql) < 3 > lane;
				if (lane == NULL) {
					qDebug() << "error";
					return;
				}
				Laneip pp;
				Object_utils<Laneip>::copy(&pp, lane);
```

	4. 对于static void clear(T& data)静态成员函数而言，为清除引用orm类型对象。
 	5. 对于static void clear(T* & data)静态成员函数而言，为清除指针类型orm的对象。
 	6. 对于static bool isNULL(T& data) 静态成员函数而言，判断引用的orm类型是否为空，与静态成员函数isClear的返回值正好相反
 	7. 对于static bool isClear(T& data) 静态成员函数而言，判断引用的orm类型是否为被清除，与静态成员函数isNULL的返回值正好相反

## 5,配置文件：qjbctemplate.ini

​	此为“20.09”的配置文件，如果或许增加内容，则会在配置文件中单独说明，配置文件的说明如：

​	

```
	    [MYSQL]   #暂时只要支持MYSQL
		host=127.0.0.1	#数据库地址
		port=3306	#数据库端口
		dbname=test1	#数据库名
		dbuser=root		#用户名
		dbpwd=			#密码
		isPool=true		#释放开启线程，有true和false两张可选，如果是false，则[pool]不管用
		timeout=2		#连接数据库的延迟
		[TEMPLATE]
		Loglevel=1		#日志水平，0代表全输出，1代表不输出
		automemory=ture	#如果为true,则有Qdbc来管理动态分配的内存，如果false则手动管理内存，Qclear宏将无效。
		version=20.09.014c		#版本既可以在配置文件看，也可以在Qdbc.h中查看

		[pool]
		initialPoolSize=4	#线程池的个数，暂时只支持默认的线程池，如需扩展，等待后续开发
```

## 6,结束

​	

​	希望大家提供大量的测试案列，此版暂定为测试版本，谢谢大家支持。如有bug，请在Issues中提问，或者联系邮箱1104559085@qq.com  。

​	Qdbc源码地址： https://github.com/linuxguangbo/QtOrm  

​	Qdbc测试地址： https://github.com/linuxguangbo/Qt-Orm-test 

​	说明： 测试库(Qt-Orm-test )放置测试代码，源码库（QtOrm  ）不放置测试代码

​	如果想修修改代码，或用于其他用途，请带有Qdbc.h中的注释。另遵守Apache License 2.0协议。版本请看配置文件
​	
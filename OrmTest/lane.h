#pragma once

#include "Qdbc.h"
#include <QDateTime>
class Laneip :public QObject
{
	Q_OBJECT
public:
	Q_ATTR(int, id);
	Q_ATTR(QString, ip)
	Q_ATTR(int, port)
	Q_ATTR_ALIAS(QString,url_url,url)
	Q_ATTR(int,entryno)
	Q_ATTR(int,etype)
	Q_ATTR(int, Status)
	Q_ATTR(QString, description)
	Q_ATTR(QDateTime,updatetime)
};

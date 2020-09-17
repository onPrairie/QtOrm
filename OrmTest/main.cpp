
#include <QtCore/QCoreApplication>
#include "Qdbc.h"
#include "lane.h"
#include <QDebug>
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QList<Laneip*> lane;
	QString sql = "SELECT * FROM laneip";
	Qselect(sql) > lane;
	qDebug() << Object_utils::toString(lane);

	return a.exec();
}

#include "mainwindow.h"
#include <QApplication>
//#include <QCoreApplication>
#include <QCommandLineParser>
#include <echoclient.h>
#include <curlpp.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCommandLineParser parser;
	parser.setApplicationDescription("QtWebSockets example: echoclient");
	parser.addHelpOption();

	QCommandLineOption dbgOption(QStringList() << "d" << "debug",
		QCoreApplication::translate("main", "Debug output [default: off]."));
	parser.addOption(dbgOption);
	parser.process(a);
	bool debug = parser.isSet(dbgOption);

	MainWindow w;
	w.show();

	EchoClient client(debug);
	QObject::connect(&client, SIGNAL(msgReceived(QString)), &w, SLOT(appendMsgOnMonitor(QString)));
	QObject::connect(&w, SIGNAL(connectButtonPressed(QString)), &client, SLOT(open(QString)));
	QObject::connect(&w, SIGNAL(disconnectButtonPressed()), &client, SLOT(close()));
	QObject::connect(&w, SIGNAL(sendMessage(QString,QString)), &client, SLOT(sendMessage(QString,QString)));

	return a.exec();
}

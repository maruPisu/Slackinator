#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <echoclient.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCommandLineParser parser;
	parser.setApplicationDescription("QtWebSockets example: echoclient");
	parser.addHelpOption();

//	QCommandLineOption dbgOption(QStringList() << "d" << "debug",
//		QCoreApplication::translate("main", "Debug output [default: off]."));
//	QCommandLineOption dbgOption2(QStringList() << "t" << "token",
//		QCoreApplication::translate("main", "Default token [default: empty]."));
	parser.addOptions({
				  {"d","specify debug.","on/off"},
				  {"t","specify token.","string"},
			  });
	parser.process(a);
	bool debug = parser.isSet("d");
	QString defaultToken = "";
	if(parser.isSet("t")){
		defaultToken = parser.value("t");
	}

	MainWindow w;
	w.setDefaultToken(defaultToken);
	w.show();

	EchoClient client(debug);
	QObject::connect(&client, SIGNAL(msgReceived(QString)), &w, SLOT(processMessage(QString)));
	QObject::connect(&client, SIGNAL(okConnected()), &w, SLOT(onConnected()));
	QObject::connect(&client, SIGNAL(okDisconnected()), &w, SLOT(onDisonnected()));
	QObject::connect(&w, SIGNAL(connectButtonPressed(QString)), &client, SLOT(open(QString)));
	QObject::connect(&w, SIGNAL(disconnectButtonPressed()), &client, SLOT(close()));
	QObject::connect(&w, SIGNAL(sendMessage(QString,QString)), &client, SLOT(sendMessage(QString,QString)));

	if(!defaultToken.isEmpty()){
		w.connectFromToken(defaultToken);
	}

	return a.exec();
}

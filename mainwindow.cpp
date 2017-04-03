#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <curlpp.h>
#include <QDebug>
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->label_2->setTextInteractionFlags(Qt::TextSelectableByMouse);
	ui->label_4->setTextInteractionFlags(Qt::TextSelectableByMouse);
	ui->pushButton->setProperty("connect", true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	if(ui->pushButton->property("connect").toBool()){

		// Retrieve the Websocket Data
		QString token = ui->tokenLine->text();

		connectFromToken(token);

		// Initiate the connection with the retrieved URL

	}else{
		// Disconnect the WebSocket
		emit disconnectButtonPressed();

	}
}

void MainWindow::appendMsgOnMonitor(QString msg)
{
	// Append the raw data in the monitor tab
	QString prev = ui->label_2->text();
	ui->label_2->setText(prev + "\n" + msg);
}

bool MainWindow::rtmStartErrorHandler(const Json::Value &res, QString &error_message)
{
	std::string ok = res["ok"].asString();
	if(ok.compare("false") == 0){
		//some error occurred
		error_message = QString::fromStdString(res["error"].asString());
		return false;
	}
	return true;
}

void MainWindow::parseMessage(QString msg)
{
	Json::Value jsonMsg;
	Json::Reader reader;

	reader.parse(msg.toStdString().c_str(), jsonMsg);

	if(jsonMsg["type"] != "message"){
		return;
	}

	QString text = QString::fromStdString(jsonMsg["text"].asString());
	QString channel = QString::fromStdString(jsonMsg["channel"].asString());

	if(text.startsWith("roadmap ")){
		roadmapParser(text, channel);
	}
}

void MainWindow::roadmapParser(QString text, QString channel)
{
	if(text.contains(" add ")){
		QString toAdd = text;
		toAdd = removeFirstOccurrance(toAdd, "add");
		toAdd = removeFirstOccurrance(toAdd, "roadmap");
		toAdd = toAdd.trimmed();

		emit sendMessage("adding '" + toAdd + "' to the roadmap", channel);

		QFile file ("roadmap");
		file.open(QIODevice::Append);
	//	QString content = file.readAll();

		QTextStream out(&file);
		out << toAdd << "\n";
		file.close();
	}
	if(text.contains(" read all")){
		QFile file ("roadmap");
		file.open(QIODevice::ReadOnly);
		QString variable = file.readAll();
		file.close();
		emit sendMessage(variable, channel);
	}
}

void MainWindow::connectFromToken(QString token)
{
	CURLpp handler = CURLpp::Builder()
			.set_connect_timeout(2000)
			.set_url("https://slack.com/api/rtm.start?token=" + token.toStdString())
			.build();

	auto res = handler.performJson();

	// Error checking for rtm.start
	QString error = "";
	if(!rtmStartErrorHandler(res, error)){
		QMessageBox msgBox;
		msgBox.setText("rtm.start error: " + error);
		msgBox.exec();
		return;
	}

	// Retrieve channel/group data
	for(Json::ValueIterator i_chann = res["channels"].begin(); i_chann != res["channels"].end(); ++i_chann) {
		auto o_channel = *i_chann;
		instertChannel(o_channel);
	}

	for(Json::ValueIterator i_group = res["groups"].begin(); i_group != res["groups"].end(); ++i_group) {
		auto o_group = *i_group;
		instertChannel(o_group);
	}

	emit connectButtonPressed(QString::fromStdString(res["url"].asString()));
}

QString MainWindow::removeFirstOccurrance(QString source, QString pattern)
{
	QString ret = source.replace(source.indexOf(pattern), pattern.size(), "");
	return ret;
}

QString MainWindow::getDefaultToken() const
{
	return defaultToken;
}

void MainWindow::setDefaultToken(const QString &value)
{
	defaultToken = value;
	ui->tokenLine->setText(defaultToken);
}

void MainWindow::setA(QApplication *value)
{
	a = value;
}

void MainWindow::processMessage(QString msg)
{
	parseMessage(msg);
	appendMsgOnMonitor(msg);
}

void MainWindow::onConnected()
{
	// Update the UI
	ui->tokenLine->setEnabled(false);
	ui->pushButton->setText("disconnect");
	ui->pushButton->setProperty("connect", false);
}

void MainWindow::onDisonnected()
{
	// Update the UI
	ui->tokenLine->setEnabled(true);
	ui->pushButton->setText("connect");
	ui->pushButton->setProperty("connect", true);
}

bool MainWindow::getDebug() const
{
	return debug;
}

void MainWindow::setDebug(bool value)
{
	debug = value;
}

void MainWindow::on_pushButton_2_clicked()
{
	QString msg = ui->lineEdit->text();
	ui->lineEdit->clear();
	appendMsgOnMonitor("Me: " + msg);
	emit sendMessage(msg, ui->comboBox->currentData().toString());
}

void MainWindow::instertChannel(Json::Value channel)
{
	QString name= QString::fromStdString(channel["name"].asString());
	QString id= QString::fromStdString(channel["id"].asString());
	bool arch = channel["is_archived"].asBool();

	channels.push_back({
				   name,
				   arch,
				   id
			   });

	if(!arch){	// insert only if not archived
		ui->comboBox->addItem(name, id);
	}
}

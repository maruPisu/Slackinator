#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <curlpp.h>
#include <QMessageBox>
#include <QDateTime>
#include "utils.h"
#include "roadmap.h"
#include "instantresponse.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->monitorLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
	ui->label_4->setTextInteractionFlags(Qt::TextSelectableByMouse);
	ui->connectButton->setProperty("connect", true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_connectButton_clicked()
{
	if(ui->connectButton->property("connect").toBool()){

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

	static QString toPrintSkeleton = "%1%2@%4: %3";

	Json::Value jsonMsg = parseJson(msg);

	if(jsonMsg.empty()){
		//dosomething
		return;
	}
	QDateTime now = QDateTime::currentDateTime();

	QString type =	QString::fromStdString(jsonMsg["type"].asString());
	QString text =	"<FONT COLOR='#00aa00'>" + QString::fromStdString(jsonMsg["text"].asString()) + "</font>" ;
	std::string user = "";
	if(usernames.contains(jsonMsg["user"].asString())){
		user = usernames.value(jsonMsg["user"].asString());
	}
	QString thisChannel = "";
	if(jsonMsg.isConvertibleTo(Json::ValueType::stringValue) && usernames.contains(jsonMsg["channel"].asString())){
		thisChannel =  channel::getNameFromVector(channels, QString::fromStdString(jsonMsg["channel"].asString()));
	}
	QString date = "" + now.toString("<FONT COLOR='#0000aa'>[dd-MM-yyyy hh:mm:ss]</FONT>");

	QString toPrint = toPrintSkeleton
			.arg(date)
			.arg(user.c_str())
			.arg(text)
			.arg(thisChannel);


	// Append the raw data in the monitor tab
	QString prev = ui->monitorLabel->text();

	if(type == "message"){
		ui->monitorLabel->setText(prev + "<br>" + toPrint);
	}else if(notShownTypes.contains(type)){
		//do nothing
	}else{
		ui->monitorLabel->setText(prev + "<br>" + date + " <FONT COLOR='#aa0000'>" + msg + "</font>");
	}


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
	Json::Value jsonMsg = parseJson(msg);

	if(jsonMsg["type"].asString() != "message"){
		return;
	}

	if(jsonMsg["user"].asString() == botid.toStdString()){
		return;
	}

	QString text = QString::fromStdString(jsonMsg["text"].asString());
	QString channel = QString::fromStdString(jsonMsg["channel"].asString());

	QString instaResp = InstantResponse::autoResponse(text);
	if(!instaResp.isEmpty()){
		emit sendMessage(instaResp, channel);
		return;
	}

	if(checkRegex(text, GlobalAction::showCommands)){
		emit sendMessage("all commands:", channel);
		emit sendMessage(InstantResponse::toString(), channel);
		emit sendMessage(Roadmap::toString(), channel);
	}

	if(checkRegex(text, ResponseActions::Generic)){
		emit sendMessage(InstantResponse::computeResponse(text), channel);
	}

	if(checkRegex(text, RoadmapActions::Generic)){
		emit sendMessage(Roadmap::computeResponse(text), channel);
	}
}

Json::Value MainWindow::parseJson(QString msg)
{
	Json::Value jsonMsg;
	Json::Reader reader;

	reader.parse(msg.toStdString().c_str(), jsonMsg);

	return jsonMsg;

}

void MainWindow::connectFromToken(QString token)
{
	CURLpp handler = CURLpp::Builder()
			.set_connect_timeout(2000)
			.set_url("https://slack.com/api/rtm.start?token=" + token.toStdString())
			.set_verbose(1)
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

	// Retrieve channel/group/user data
	for(Json::ValueIterator i_chann = res["channels"].begin(); i_chann != res["channels"].end(); ++i_chann) {
		auto o_channel = *i_chann;
		instertChannel(o_channel);
	}

	for(Json::ValueIterator i_group = res["groups"].begin(); i_group != res["groups"].end(); ++i_group) {
		auto o_group = *i_group;
		instertChannel(o_group);
	}

	for(Json::ValueIterator i_user = res["users"].begin(); i_user != res["users"].end(); ++i_user) {
		auto o_user = *i_user;
		usernames.insert(o_user["id"].asString(), o_user["name"].asString());
	}

	botid = QString::fromStdString(res["self"]["id"].asString());

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
	ui->connectButton->setText("disconnect");
	ui->connectButton->setProperty("connect", false);
}

void MainWindow::onDisonnected()
{
	// Update the UI
	ui->tokenLine->setEnabled(true);
	ui->connectButton->setText("connect");
	ui->connectButton->setProperty("connect", true);
}

bool MainWindow::getDebug() const
{
	return debug;
}

void MainWindow::setDebug(bool value)
{
	debug = value;
}

void MainWindow::on_sendMessageButton_clicked()
{
	QString msg = ui->sendMessageLineEdit->text();
	ui->sendMessageLineEdit->clear();
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

QString channel::getNameFromVector(const QVector<channel> &vec, QString id)
{
	for(channel sing: vec){
		if(sing.id == id){
			return sing.name;
		}
	}
	return "";
}

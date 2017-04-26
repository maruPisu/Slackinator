#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace GlobalAction{
const QString showCommands	= "^show commands$|^help$";
}

class QApplication;
namespace Json {
	class Value;
}

struct channel{
	QString name = "";
	bool isArchived = false;
	QString id = "";

	static QString getNameFromVector(const QVector<channel> & vec, QString id);
};

const QStringList notShownTypes{
	"user_typing",
	"presence_change",
	"reconnect_url",
	"hello",
	"desktop_notification"
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

signals:
	void connectButtonPressed(QString);
	void disconnectButtonPressed();
	void sendMessage(QString, QString);
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void connectFromToken(QString token);

	bool getDebug() const;
	void setDebug(bool value);

	void setA(QApplication *value);

	QString getDefaultToken() const;
	void setDefaultToken(const QString &value);

public slots:
	void processMessage(QString msg);
	void onConnected();
	void onDisonnected();

private slots:
	void on_connectButton_clicked();
	void on_sendMessageButton_clicked();

private:
	void instertChannel(Json::Value channel);
	void appendMsgOnMonitor(QString msg);
	bool rtmStartErrorHandler(const Json::Value & res, QString &error_message);
	void parseMessage(QString msg);
	Json::Value parseJson(QString msg);

	QString removeFirstOccurrance(QString source, QString pattern);

	QString defaultToken;
	Ui::MainWindow *ui;
	QApplication * a;

	QVector<channel> channels;
	QMap<std::string, std::string> usernames;

	QString botid = "";

	bool debug = false;
};

#endif // MAINWINDOW_H

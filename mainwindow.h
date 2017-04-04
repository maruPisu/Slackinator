#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QApplication;
namespace Json {
	class Value;
}

struct channel{
	QString name = "";
	bool isArchived = false;
	QString id = "";
};

namespace Ui {
class MainWindow;
}

namespace RoadmapActions{
const QString Add = ".*roadmap add (.*)";
const QString ReadAll = ".*roadmap read all.*";
const QString Remove = ".*roadmap remove (.*)";
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
	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

private:
	void instertChannel(Json::Value channel);
	void appendMsgOnMonitor(QString msg);
	bool rtmStartErrorHandler(const Json::Value & res, QString &error_message);
	void parseMessage(QString msg);
	void roadmapParser(QString text, QString channel);
	bool checkRegex(const QString &text, const QString &regex, bool caseInsensitive = true);
	QString replaceRegex(const QString & text, const QString & regex, const char * after, bool caseInsensitive = true);

	QString removeFirstOccurrance(QString source, QString pattern);

	QString defaultToken;
	Ui::MainWindow *ui;
	QApplication * a;

	QVector<channel> channels;


	bool debug = false;
};

#endif // MAINWINDOW_H

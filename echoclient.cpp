#include "echoclient.h"
#include <QtCore/QDebug>

QT_USE_NAMESPACE

EchoClient::EchoClient(bool debug, QObject *parent) :
    QObject(parent),
    m_debug(debug)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &EchoClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &EchoClient::onDisconnected);
}

void EchoClient::fakeMessage(QString message)
{
	emit msgReceived(message);
}

void EchoClient::open(QString url)
{
	if (m_debug)
	    qDebug() << "WebSocket server:" << url;
	m_webSocket.open(QUrl(url));
}

void EchoClient::close()
{
	m_webSocket.close();
}

void EchoClient::sendMessage(QString msg, QString channel)
{
	static QString skeleton =
			"{"
				"\"id\": 1,"
				"\"type\": \"message\","
				"\"channel\": \"%2\","
				"\"text\": \"%1\""
			"}";


	QString totMsg = skeleton
			.arg(msg)
			.arg(channel);

	m_webSocket.sendTextMessage(totMsg);
}

void EchoClient::onConnected()
{
    if (m_debug)
	qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
	    this, &EchoClient::onTextMessageReceived);
    emit okConnected();
}

void EchoClient::onDisconnected()
{
	if (m_debug)
	    qDebug() << "WebSocket disconnected";
	emit okDisconnected();
}

void EchoClient::onTextMessageReceived(QString message)
{
    if (m_debug)
	qDebug() << "Message received:" << message;
    emit msgReceived(message);
}

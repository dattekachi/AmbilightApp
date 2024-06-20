#pragma once

#ifndef PCH_ENABLED
	#include <QString>
	#include <QByteArray>
	#include <QJsonObject>
#endif

#include <utils/Logger.h>

class AmbilightAPI;
class QTcpSocket;
class AmbilightAppManager;

class JsonClientConnection : public QObject
{
	Q_OBJECT

public:
	JsonClientConnection(QTcpSocket* socket, bool localConnection);

signals:
	void SignalClientConnectionClosed(JsonClientConnection* client);

public slots:
	qint64 sendMessage(QJsonObject);

private slots:
	void readRequest();
	void disconnected();

private:
	QTcpSocket* _socket;
	AmbilightAPI* _hmbilightAPI;
	QByteArray _receiveBuffer;
	Logger* _log;
};

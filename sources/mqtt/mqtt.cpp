// project includes
#include <mqtt/mqtt.h>

#include <base/AmbilightAppInstance.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QHostInfo>

#include <api/AmbilightAPI.h>

// default param %1 is 'Ambilight App', do not edit templates here
const static QString TEMPLATE_AMBILIGHTAPPAPI = QStringLiteral("%1/JsonAPI");
const static QString TEMPLATE_AMBILIGHTAPPAPI_RESPONSE = QStringLiteral("%1/JsonAPI/response");

mqtt::mqtt(const QJsonDocument& mqttConfig)
	: QObject()
	, _enabled(false)
	, _port(1883)
	, _is_ssl(false)
	, _ignore_ssl_errors(true)
	, _maxRetry(0)
	, _currentRetry(0)
	, _retryTimer(nullptr)
	, _initialized(false)
	, _log(Logger::getInstance("MQTT"))
	, _clientInstance(nullptr)
{
	handleSettingsUpdate(settings::type::MQTT, mqttConfig);
}

mqtt::~mqtt()
{
	Debug(_log, "Prepare to shutdown");
	stop();
	Debug(_log, "MQTT server is closed");
}

void mqtt::start(QString host, int port, QString username, QString password, bool is_ssl, bool ignore_ssl_errors, QString customTopic)
{

	if (_clientInstance != nullptr)
		return;

	AMBILIGHTAPPAPI = QString(TEMPLATE_AMBILIGHTAPPAPI).arg(customTopic);
	AMBILIGHTAPPAPI_RESPONSE = QString(TEMPLATE_AMBILIGHTAPPAPI_RESPONSE).arg(customTopic);

	Debug(_log, "Starting the MQTT connection. Address: %s:%i. Protocol: %s. Authentication: %s, Ignore errors: %s",
		QSTRING_CSTR(host), port, (is_ssl) ? "SSL" : "NO SSL", (!username.isEmpty() || !password.isEmpty()) ? "YES" : "NO", (ignore_ssl_errors) ? "YES" : "NO");
	Debug(_log, "MQTT topic: %s, MQTT response: %s", QSTRING_CSTR(AMBILIGHTAPPAPI), QSTRING_CSTR(AMBILIGHTAPPAPI_RESPONSE));

	QHostAddress address(host);

	if (!is_ssl && QAbstractSocket::IPv4Protocol != address.protocol() && QAbstractSocket::IPv6Protocol != address.protocol())
	{
		Debug(_log, "The search for the name translated to the IP address has started...");
		QHostInfo info = QHostInfo::fromName(host);
		if (!info.addresses().isEmpty())
			address = info.addresses().first();
		Debug(_log, "The search for IP has finished: %s => %s", QSTRING_CSTR(host), QSTRING_CSTR(address.toString()));
	}
	
	if (is_ssl)
	{
		QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
		_clientInstance = new QMQTT::Client(host, port, sslConfig, ignore_ssl_errors, this);
	}
	else
		_clientInstance = new QMQTT::Client(address, port, this);

	QString clientId = QString("Ambilight App:%1").arg(QHostInfo::localHostName());
	_clientInstance->setClientId(clientId);

	if (!username.isEmpty())
		_clientInstance->setUsername(username);

	if (!password.isEmpty())
		_clientInstance->setPassword(password.toLocal8Bit());

	if (is_ssl && ignore_ssl_errors)
	{
		QObject::connect(_clientInstance, &QMQTT::Client::sslErrors, this, [this](const QList<QSslError>& errors) {
			if (_clientInstance != nullptr)
				_clientInstance->ignoreSslErrors();
		});
	}
	QObject::connect(_clientInstance, &QMQTT::Client::error, this, &mqtt::error);
	QObject::connect(_clientInstance, &QMQTT::Client::connected, this, &mqtt::connected);
	QObject::connect(_clientInstance, &QMQTT::Client::received, this, &mqtt::received);
	QObject::connect(_clientInstance, &QMQTT::Client::disconnected, this, &mqtt::disconnected);
	_clientInstance->connectToHost();
}

void mqtt::stop()
{
	if (_clientInstance != nullptr)
	{
		Debug(_log, "Closing MQTT");
		disconnect(_clientInstance, nullptr, this, nullptr);
		_clientInstance->disconnectFromHost();
		_clientInstance->deleteLater();
		_clientInstance = nullptr;
	}
}

void mqtt::disconnected()
{
	Debug(_log, "Disconnected");
}

void mqtt::connected()
{
	Debug(_log, "Connected");

	if (_retryTimer != nullptr)
	{
		Debug(_log, "Removing retry timer");
		disconnect(_retryTimer, nullptr, nullptr, nullptr);
		_retryTimer->deleteLater();
		_retryTimer = nullptr;
	}

	if (_clientInstance != nullptr)
	{
		_clientInstance->subscribe(AMBILIGHTAPPAPI, 2);
	}
}


void mqtt::error(const QMQTT::ClientError error)
{
	QString message;
	switch (error)
	{
		case(QMQTT::ClientError::UnknownError): message = "UnknownError"; break;
		case(QMQTT::ClientError::SocketConnectionRefusedError): message = "SocketConnectionRefusedError"; break;
		case(QMQTT::ClientError::SocketRemoteHostClosedError): message = "SocketRemoteHostClosedError"; break;
		case(QMQTT::ClientError::SocketHostNotFoundError): message = "SocketHostNotFoundError"; break;
		case(QMQTT::ClientError::SocketAccessError): message = "SocketAccessError"; break;
		case(QMQTT::ClientError::SocketResourceError): message = "SocketResourceError"; break;
		case(QMQTT::ClientError::SocketTimeoutError): message = "SocketTimeoutError"; break;
		case(QMQTT::ClientError::SocketDatagramTooLargeError): message = "SocketDatagramTooLargeError"; break;
		case(QMQTT::ClientError::SocketNetworkError): message = "SocketNetworkError"; break;
		case(QMQTT::ClientError::SocketAddressInUseError): message = "SocketAddressInUseError"; break;
		case(QMQTT::ClientError::SocketAddressNotAvailableError): message = "SocketAddressNotAvailableError"; break;
		case(QMQTT::ClientError::SocketUnsupportedSocketOperationError): message = "SocketUnsupportedSocketOperationError"; break;
		case(QMQTT::ClientError::SocketUnfinishedSocketOperationError): message = "SocketUnfinishedSocketOperationError"; break;
		case(QMQTT::ClientError::SocketProxyAuthenticationRequiredError): message = "SocketProxyAuthenticationRequiredError"; break;
		case(QMQTT::ClientError::SocketSslHandshakeFailedError): message = "SocketSslHandshakeFailedError"; break;
		case(QMQTT::ClientError::SocketProxyConnectionRefusedError): message = "SocketProxyConnectionRefusedError"; break;
		case(QMQTT::ClientError::SocketProxyConnectionClosedError): message = "SocketProxyConnectionClosedError"; break;
		case(QMQTT::ClientError::SocketProxyConnectionTimeoutError): message = "SocketProxyConnectionTimeoutError"; break;
		case(QMQTT::ClientError::SocketProxyNotFoundError): message = "SocketProxyNotFoundError"; break;
		case(QMQTT::ClientError::SocketProxyProtocolError): message = "SocketProxyProtocolError"; break;
		case(QMQTT::ClientError::SocketOperationError): message = "SocketOperationError"; break;
		case(QMQTT::ClientError::SocketSslInternalError): message = "SocketSslInternalError"; break;
		case(QMQTT::ClientError::SocketSslInvalidUserDataError): message = "SocketSslInvalidUserDataError"; break;
		case(QMQTT::ClientError::SocketTemporaryError): message = "SocketTemporaryError"; break;
		case(1 << 16): message = "MqttUnacceptableProtocolVersionError"; break;
		case(QMQTT::ClientError::MqttIdentifierRejectedError): message = "MqttIdentifierRejectedError"; break;
		case(QMQTT::ClientError::MqttServerUnavailableError): message = "MqttServerUnavailableError"; break;
		case(QMQTT::ClientError::MqttBadUserNameOrPasswordError): message = "MqttBadUserNameOrPasswordError"; break;
		case(QMQTT::ClientError::MqttNotAuthorizedError): message = "MqttNotAuthorizedError"; break;
		case(QMQTT::ClientError::MqttNoPingResponse): message = "MqttNoPingResponse"; break;
	}
	Error(_log, "Error: %s", QSTRING_CSTR(message));

	initRetry();
}

void mqtt::initRetry()
{
	if (_maxRetry > 0 && _retryTimer == nullptr)
	{
		int intervalSec = 10;
		Debug(_log, "Prepare to retry in %i seconds (limit: %i)", intervalSec, _maxRetry);
		_currentRetry = 0;
		_retryTimer = new QTimer(this);
		_retryTimer->setInterval(intervalSec * 1000);
		connect(_retryTimer, &QTimer::timeout, this, [this]() {
			if (_clientInstance == nullptr || _clientInstance->isConnectedToHost() || ++_currentRetry > _maxRetry)
			{
				Debug(_log, "Removing retry timer");
				disconnect(_retryTimer, nullptr, nullptr, nullptr);
				_retryTimer->deleteLater();
				_retryTimer = nullptr;
			}
			else
			{
				Debug(_log, "Retrying %i/%i", _currentRetry, _maxRetry);
				_clientInstance->connectToHost();
			}
		});
		_retryTimer->start();
	}
}

void mqtt::handleSettingsUpdate(settings::type type, const QJsonDocument& config)
{
	if (type == settings::type::MQTT)
	{
		QJsonObject obj = config.object();

		_enabled = obj["enable"].toBool(false);
		_host = obj["host"].toString();
		_port = obj["port"].toInt(1883);
		_username = obj["username"].toString();
		_password = obj["password"].toString();
		_is_ssl = obj["is_ssl"].toBool(false);
		_ignore_ssl_errors = obj["ignore_ssl_errors"].toBool(true);
		_maxRetry = obj["maxRetry"].toInt(120);

		_customTopic = obj["custom_topic"].toString().trimmed();
		if (_customTopic.isEmpty())
			_customTopic = "AmbilightAPP";

		if (_initialized)
		{
			stop();
			if (_enabled)
				start(_host, _port, _username, _password, _is_ssl, _ignore_ssl_errors, _customTopic);
		}

		_initialized = true;
	}	
}

void mqtt::begin()
{
	if (_initialized && _enabled)
		start(_host, _port, _username, _password, _is_ssl, _ignore_ssl_errors, _customTopic);
}

void mqtt::executeJson(QString origin, const QJsonDocument& input, QJsonDocument& result)
{	
	AmbilightAPI* _ambilightAPI = new AmbilightAPI(origin, _log, true, this);

	_resultArray = QJsonArray();

	_ambilightAPI->initialize();
	connect(_ambilightAPI, &AmbilightAPI::SignalCallbackJsonMessage, this, [this](QJsonObject ret) {
		_resultArray.append(ret);
	});

	if (input.isObject())
		_ambilightAPI->handleMessage(input.toJson());
	else if (input.isArray())
	{
		const QJsonArray& array = input.array();
		for (const auto& elem : array)
			if (elem.isObject())
			{
				QJsonDocument doc(elem.toObject());
				_ambilightAPI->handleMessage(doc.toJson());
			}
	}

	result.setArray(_resultArray);

	disconnect(_ambilightAPI, &AmbilightAPI::SignalCallbackJsonMessage, nullptr, nullptr);
	_ambilightAPI->deleteLater();
}

///////////////////////////////////////////////////////////
/////////////////////// Testing ///////////////////////////
///////////////////////////////////////////////////////////
// listener:
// mosquitto_sub -h localhost -t AmbilightAPP/JsonAPI/response
// commands:
// mosquitto_pub -h localhost -t AmbilightAPP/JsonAPI -m "[{\"command\" : \"clear\", \"priority\" :1}, {\"command\" : \"clear\", \"priority\" :2}]"
// mosquitto_pub -h localhost -t AmbilightAPP/JsonAPI -m "[{\"command\":\"componentstate\",\"componentstate\": {\"component\":\"HDR\",\"state\": true } }, {\"command\":\"componentstate\",\"componentstate\": {\"component\":\"HDR\",\"state\": false } }]"
// mosquitto_pub -h localhost -t AmbilightAPP/JsonAPI -m "[{\"command\" : \"instance\",\"subcommand\":\"switchTo\",\"instance\":1},{\"command\":\"componentstate\",\"componentstate\":{\"component\":\"LEDDEVICE\",\"state\": false}}]"
///////////////////////////////////////////////////////////

void mqtt::received(const QMQTT::Message& message)
{
	QString topic = message.topic();
	QString payload = QString().fromUtf8(message.payload());

	if (QString::compare(AMBILIGHTAPPAPI, topic) == 0 && payload.length() > 0)
	{
		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8(), &error);
		QMQTT::Message result;
		result.setTopic(AMBILIGHTAPPAPI_RESPONSE);
		result.setQos(2);
		if (doc.isEmpty())
			result.setPayload(QString("{\"success\" : false, \"error\" : \"%1 at offset: %2\"}").arg(error.errorString()).arg(error.offset).toUtf8());
		else
		{
			QJsonDocument resJson;

			executeJson("MQTT", doc, resJson);

			QString returnPayload = resJson.toJson(QJsonDocument::Compact);
			Debug(_log, "JSON result: %s", QSTRING_CSTR(returnPayload));
			result.setPayload(returnPayload.toUtf8());
		}		
		_clientInstance->publish(result);		
	}
}

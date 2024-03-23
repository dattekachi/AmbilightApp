#pragma once

#ifndef PCH_ENABLED		
	#include <memory>
	#include <vector>
#endif

#include <base/AmbilightAppInstance.h>
#include <base/AmbilightAppManager.h>
#include <QSystemTrayIcon>
#include <QWidget>

class AmbilightAppDaemon;
class QMenu;
class QAction;
class QColorDialog;


class SysTray : public QObject
{
	Q_OBJECT

public:
	SysTray(AmbilightAppDaemon* ambilightappDaemon, quint16 webPort);
	~SysTray();

public slots:
	void showColorDialog();
	void setColor(const QColor& color);
	void settings();
	void setEffect();
	void clearEfxColor();
	void runMusicLed();
	//void restartApp();
	void setAutorunState();

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

	///
	/// @brief is called whenever a ambilightapp instance state changes
	///
	void signalInstanceStateChangedHandler(InstanceState state, quint8 instance, const QString& name);
	void signalSettingsChangedHandler(settings::type type, const QJsonDocument& data);

private:
	void createTrayIcon();

#ifdef _WIN32
	///
	/// @brief Checks whether Ambilight App should start at Windows system start.
	/// @return True on success, otherwise false
	///
	bool getCurrentAutorunState();
#endif

	QAction* _quitAction;
	QAction* _startAction;
	QAction* _stopAction;
	QAction* _colorAction;
	QAction* _settingsAction;
	QAction* _clearAction;
	QAction* _runmusicledAction;
	//QAction* _restartappAction;
	QAction* _autorunAction;

	QSystemTrayIcon* _trayIcon;
	QMenu*           _trayIconMenu;
	QMenu*           _trayIconEfxMenu;

	QColorDialog*		_colorDlg;

	std::weak_ptr<AmbilightAppManager> _instanceManager;
	std::weak_ptr<AmbilightAppInstance>	_ambilightappHandle;
	quint16				_webPort;
};

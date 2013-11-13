#include "appstatus.h"

#include <QStatusBar>
#include <QDebug>
#include <QTimer>
#include <QString>

AppStatus* AppStatus::_instance = 0;

//---------------------------------------------------------------------------------
AppStatus::AppStatus()
    : _state(APPSTATE_IDLE)
    , _currentLocale("en")
    , _statusBar(0)
{

}

//---------------------------------------------------------------------------------
void
AppStatus::setStatusBarMessage(const QString& p_message, float p_seconds)
{
    if (_statusBar == 0)
    {
        qWarning() << QObject::tr("Trying to set status bar message without a set status bar.");
        return;
    }
    _statusBar->showMessage(p_message, p_seconds * 1000);
}

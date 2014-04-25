#include "appstatus.h"

#include <QStatusBar>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <QSettings>

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
    setStatusBarMessage(p_message, p_seconds,
                        QSettings().value("preferences/layout/colorCbFree", QColor(0, 0, 0)).value<QColor>());
}

//---------------------------------------------------------------------------------
void
AppStatus::setStatusBarMessage(const QString& p_message, float p_seconds, const QColor& p_color)
{
    if (_statusBar == 0)
    {
        qWarning() << QObject::tr("Trying to set status bar message without a set status bar.");
        return;
    }
    qDebug() << p_message;

    _statusBar->setStyleSheet(QString("QStatusBar { color: rgb(%1, %2, %3) }").arg(p_color.red())
                                                                              .arg(p_color.green())
                                                                              .arg(p_color.blue()));
    _statusBar->showMessage(p_message, p_seconds * 1000);
}

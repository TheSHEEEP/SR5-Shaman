#ifndef APPSTATUS_H
#define APPSTATUS_H

#include <QString>
#include <QObject>

/**
 * @brief The different possible application states.
 */
enum AppState
{
    APPSTATE_INVALID = -1,
    APPSTATE_IDLE,
    APPSTATE_GUIDED_CREATION,
    APPSTATE_CHRACATER_EDITING,
    APPSTATE_NUM_APPSTATES
};

// Friendly helper defines
#define APPSTATUS AppStatus::getSingletonPtr()

class QStatusBar;

/**
 * @brief Singleton class that holds information about the current application status.
 * @author  TheSHEEEP
 */
class AppStatus
{

private:
    static AppStatus* _instance;

    /**
     * @brief Constructor.
     */
    AppStatus();

public:
    /**
     * @brief Returns a pointer to the AppStatus.
     */
    static AppStatus* getSingletonPtr()
    {
        if (_instance == 0)
        {
            _instance = new AppStatus();
        }
        return _instance;
    }

public:

    /**
     * @brief Returns the current application state.
     */
    AppState getState() const
    {
        return _state;
    }

    /**
     * @brief Sets the new application state.
     * @note    This is mere an information some classes use.
     *          It does not change any class behaviour directly.
     */
    void setState(AppState p_state)
    {
        _state = p_state;
    }

    /**
     * @brief Returns the current locale in short form: "en", "de", etc.
     */
    const QString& getCurrentLocale() const;

    /**
     * @brief Sets the status bar to use.
     */
    void setStatusBar(QStatusBar* p_statusBar);

    /**
     * @brief Sets the status bar message.
     * @note    Make sure to call setStatusBar first.
     * @param p_message The message to show.
     * @param p_seconds How many seconds the message should be visible.
     * @param p_color   The color of the message.
     */
    void setStatusBarMessage(const QString& p_message, float p_seconds);

    /**
     * @brief Sets the status bar message.
     * @note    Make sure to call setStatusBar first.
     * @param p_message The message to show.
     * @param p_seconds How many seconds the message should be visible.
     * @param p_color   The color of the message.
     */
    void setStatusBarMessage(const QString& p_message, float p_seconds, const QColor& p_color);

private:
    AppState _state;

    QString     _currentLocale;
    QStatusBar* _statusBar;
};


//---------------------------------------------------------------------------------
inline
const QString&
AppStatus::getCurrentLocale() const
{
    return _currentLocale;
}

//---------------------------------------------------------------------------------
inline
void
AppStatus::setStatusBar(QStatusBar* p_statusBar)
{
    _statusBar = p_statusBar;
}

#endif // APPSTATUS_H

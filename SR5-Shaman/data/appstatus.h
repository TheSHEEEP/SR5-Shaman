#ifndef APPSTATUS_H
#define APPSTATUS_H

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

/**
 * @brief Singleton class that holds information about the current application status.
 */
class AppStatus
{
private:
    static AppStatus* _instance;

    /**
     * @brief Constructor.
     */
    AppStatus()
        : _state(APPSTATE_IDLE)
    {

    }

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

    /**
     * @brief Returns the current application state.
     */
    const AppState getState() const
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

private:
    AppState _state;
};

#endif // APPSTATUS_H

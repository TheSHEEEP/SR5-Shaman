#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class PreferencesWindow;
class CreationInitializingWindow;
class GlobalEventFilter;

/**
 *  Main SR5 Shaman window.
 *  Responsible for the correct initialization of all application UI.
 *  Holds the main status bar and title.
 *  @author TheSHEEEP
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     *  @brief Destructor.
     */
    ~MainWindow();

    /**
     * @brief Initilizes the main window, setting up the correct size, etc.
     */
    void initialize(GlobalEventFilter* p_globalFilter);

    /**
     * @brief Resizes the content.
     */
    void resizeEvent(QResizeEvent* p_event);

private slots:
    /**
     * @brief Show the preferences window.
     */
    void on_actionPreferences_triggered();

    /**
     * @brief Starts the creation of a new character.
     */
    void on_actionNew_Character_triggered();

    /**
     * @brief switches the character preview position.
     */
    void on_actionSwitch_Preview_Position_triggered();

    /**
     * @brief Starts the character creation.
     */
    void startCharacterCreation();

    /**
     * @brief Cancels the character creation.
     */
    void cancelCharacterCreation();

private:
    Ui::MainWindow *ui;

    PreferencesWindow*  _prefWindow;

    CreationInitializingWindow* _creationInitWindow;

    int     _targetWidth;
    int     _targetHeight;
    bool    _initMaximizer;
};

#endif // MAINWINDOW_H

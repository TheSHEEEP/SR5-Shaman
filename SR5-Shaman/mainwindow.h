#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class PreferencesWindow;

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
    void initialize();

private slots:
    /**
     * @brief Show the preferences window.
     */
    void on_actionPreferences_triggered();

private:
    Ui::MainWindow *ui;

    PreferencesWindow*  _prefWindow;
};

#endif // MAINWINDOW_H

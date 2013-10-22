#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QMainWindow>
#include <QItemSelection>

namespace Ui {
class PreferencesWindow;
}
class QStandardItemModel;


/**
 * @brief This class represents a window that holds all application preferences.
 *        Stuff like font settings, user name, etc.
 * @note    This does NOT store any rules, plugin, or house rule settings.
 *          It is only for the application behaviour.
 * @author  TheSHEEEP
 */
class PreferencesWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit PreferencesWindow(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~PreferencesWindow();

    /**
     * @brief Initilizes the main window, setting up the correct size, etc.
     */
    void initialize();

private slots:
    /**
     * @brief Will show the UI for the selected options entry.
     */
    void selectionChangedSlot(const QItemSelection& p_newSelection, const QItemSelection & p_oldSelection);

private:
    Ui::PreferencesWindow *ui;

    QStandardItemModel* _treeModel;

    QWidget* _currentSettings;
    QWidget* _layoutSettings;
};

#endif // PREFERENCESWINDOW_H

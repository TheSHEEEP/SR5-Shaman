#ifndef CREATIONINITIALIZINGWINDOW_H
#define CREATIONINITIALIZINGWINDOW_H

#include <QMainWindow>

namespace Ui {
class CreationInitializingWindow;
}

/**
 * @brief This small windows let's the user choose which type of character
 *          creation she wants.
 * @author  TheSHEEEP
 */
class CreationInitializingWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CreationInitializingWindow(QWidget *parent = 0);
    /**
     * @♦brief Destructor.
     */
    ~CreationInitializingWindow();

    /**
     * @brief Initializes this window.
     */
    void initialize();

    /**
     * @brief Applies the current language to this widget.
     */
    void applyTranslation();

signals:
    void startCreation();
    void cancelCreation();

private slots:
    /**
     * @brief Dispatches the creation start signal.
     */
    void on_btnAccept_clicked();

    /**
     * @brief Dispatches the cancel event.
     */
    void on_btnCancel_clicked();

private:
    Ui::CreationInitializingWindow *ui;
};

#endif // CREATIONINITIALIZINGWINDOW_H

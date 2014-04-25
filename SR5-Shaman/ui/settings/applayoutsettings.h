#ifndef APPLAYOUTSETTINGS_H
#define APPLAYOUTSETTINGS_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class AppLayoutSettings;
}

/**
 * @brief   Holds the UI elements for the application layout settings.
 *          Also responsible for storing these using QSettings.
 * @author  TheSHEEEP
 */
class AppLayoutSettings : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit AppLayoutSettings(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~AppLayoutSettings();

    /**
     * @brief Initilizes the main window, setting up the correct size, etc.
     */
    void initialize();

private slots:
    /**
     * @brief Switches to the selected theme.
     * @param p_newCaption  The new caption of the combo box.
     */
    void handleThemeChange(QString p_newCaption);

    /**
     * @brief Switches to the selected font.
     * @param p_newCaption  The new caption of the combo box.
     */
    void handleFontNameChange(QString p_newCaption);

    /**
     * @brief Applies the new font size.
     * @param p_newSize  The new value of the spin widget.
     */
    void handleFontSizeChange(int p_newSize);

private:
    Ui::AppLayoutSettings *ui;

    QSettings               _settings;
    QMap<QString,int>       _fontIDs;
    QMap<QString,QString>   _themes;

    /**
     * @brief Applies the passed stylesheet file.
     */
    void applyStylesheet(const QString& p_sheet);
};

#endif // APPLAYOUTSETTINGS_H

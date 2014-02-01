#ifndef CUSTOMDESCRIPTORPOPUP_H
#define CUSTOMDESCRIPTORPOPUP_H

#include <QDialog>

namespace Ui {
class CustomDescriptorPopup;
}

/**
 * @brief This is a dialog popup that lets the user select a customization for something.
 *          You can get the set customization by calling getCustomization().
 *          If the user has both selected a value form the ComboBox and a custom value,
 *          the custom value has precedence.
 */
class CustomDescriptorPopup : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param p_parent          The parent widget.
     * @param p_target          The string to display as the target.
     * @param p_showSelection   If the selection frame should be shown.
     * @param p_showCustom      If the custom frame should be shown.
     */
    explicit CustomDescriptorPopup(QWidget* p_parent = 0, const QString &p_target = "", bool p_showSelection = true, bool p_showCustom = true);

    /**
     * @brief Destructor.
     */
    ~CustomDescriptorPopup();

    /**
     * @brief Sets the choices to the passed string list.
     *          An empty item will automatically be added.
     * @param p_choices The list of choices.
     */
    void setChoices(const QStringList& p_choices);

    /**
     * @return The customization chosen by the user.
     */
    QString getCustomization();

private slots:
    /**
     * @brief Checks if the user has entered something valid and accept()s, if so.
     */
    void on_btnOk_clicked();

    /**
     * @brief Rejects.
     */
    void on_btnCancel_clicked();

private:
    Ui::CustomDescriptorPopup *ui;
};

#endif // CUSTOMDESCRIPTORPOPUP_H

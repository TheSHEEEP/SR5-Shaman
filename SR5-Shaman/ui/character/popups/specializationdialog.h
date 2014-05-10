#ifndef SPECIALIZATIONDIALOG_H
#define SPECIALIZATIONDIALOG_H

#include <QDialog>

namespace Ui {
class SpecializationDialog;
}

class QListWidgetItem;

/**
 * @brief This dialog manages the specializations of a single skill.
 */
class SpecializationDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit SpecializationDialog(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~SpecializationDialog();

    /**
     * @brief Sets the skill to display in the window.
     * @param p_skill   The ID of the skill to manage.
     */
    void setSkill(const QString& p_skill);

    /**
     * @brief Apply the current localization here.
     */
    void showEvent(QShowEvent* /*p_event*/);

signals:
    void specializationChanged();

private slots:
    /**
     * @brief Enables the remove button.
     */
    void on_listSpecializations_currentItemChanged(QListWidgetItem* p_current, QListWidgetItem* p_previous);

    /**
     * @brief Removes the selected specialization.
     */
    void on_btnRemoveSpec_clicked();

    /**
     * @brief Adds the entered specialization.
     */
    void on_btnAddSpec_clicked();

    void on_btnDone_clicked();

private:
    Ui::SpecializationDialog *ui;

    QString _currentSkill;
};

#endif // SPECIALIZATIONDIALOG_H

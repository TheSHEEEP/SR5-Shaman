#ifndef KNOWLEDGESKILLDIALOG_H
#define KNOWLEDGESKILLDIALOG_H

#include <QDialog>
#include "rules/skillrules.h"

namespace Ui {
class KnowledgeSkillDialog;
}

/**
 * @brief This dialog lets the user add a knowledge or language skill.
 */
class KnowledgeSkillDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit KnowledgeSkillDialog(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~KnowledgeSkillDialog();

    /**
     * @brief Will update the display.
     */
    void showEvent(QShowEvent* /**/);

    /**
     * @brief Applies the current translation.
     */
    void applyTranslation();

    /**
     * @brief Returns the value typed by the user.
     */
    const QString& getValue() const;

    /**
     * @brief Returns true if the user selected knowledge skill.
     */
    bool getIsKnowledge() const;

    /**
     * @brief Returns the selected knowledge type.
     */
    KnowledgeType getKnowledgeType() const;

private slots:
    /**
     * @brief Accepts if something is selected.
     */
    void on_btnOk_clicked();

    /**
     * @brief Rejects.
     */
    void on_btnCancel_clicked();

    /**
     * @brief Will show/hide the knowledge type frame.
     * @param p_toggleState If the sender is toggled or not.
     */
    void handleRadioToggle(bool p_toggleState);

private:
    Ui::KnowledgeSkillDialog *ui;
};

#endif // KNOWLEDGESKILLDIALOG_H

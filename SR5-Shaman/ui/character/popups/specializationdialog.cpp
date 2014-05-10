#include "specializationdialog.h"
#include "ui_specializationdialog.h"

#include <QStringList>

#include "rules/rules.h"
#include "rules/skillrules.h"
#include "data/appstatus.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"

//---------------------------------------------------------------------------------
SpecializationDialog::SpecializationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SpecializationDialog)
    , _currentSkill("")
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
SpecializationDialog::~SpecializationDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
SpecializationDialog::setSkill(const QString& p_skill)
{
    _currentSkill = p_skill;

    const SkillDefinition& def = SKILL_RULES->getDefinition(p_skill);

    // Set title text
    ui->lblManageSpecSkill->setText("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">"
                                      + def.translations[APPSTATUS->getCurrentLocale()] + "</span></p></body></html>");

    // Get current specializations
    QStringList specs = CHARACTER_VALUES->getSkillSpecializations(p_skill);

    // Apply to list
    ui->listSpecializations->clear();
    ui->listSpecializations->addItems(specs);

    // Clear text edit
    ui->leSpecValue->setText("");
}

//---------------------------------------------------------------------------------
void
SpecializationDialog::showEvent(QShowEvent* /*p_event*/)
{
    // Apply localization
}

//---------------------------------------------------------------------------------
void
SpecializationDialog::on_listSpecializations_currentItemChanged(QListWidgetItem* p_current, QListWidgetItem* p_previous)
{
    // Activate the remove button
    ui->btnRemoveSpec->setEnabled(ui->listSpecializations->currentIndex().isValid());
}

//---------------------------------------------------------------------------------
void
SpecializationDialog::on_btnRemoveSpec_clicked()
{
    if (!ui->listSpecializations->currentIndex().isValid())
    {
        return;
    }

    // Remove the specialization
    CHARACTER_CHOICES->removeSkillSpecialization(_currentSkill, ui->listSpecializations->currentItem()->text());
    delete ui->listSpecializations->takeItem(ui->listSpecializations->currentRow());

    emit specializationChanged();
}

//---------------------------------------------------------------------------------
void
SpecializationDialog::on_btnAddSpec_clicked()
{
    // Check if there is something to add
    if (ui->leSpecValue->text().size() <= 1)
    {
        return;
    }

    // Add the specialization
    CHARACTER_CHOICES->addSkillSpecialization(_currentSkill, ui->leSpecValue->text());

    // Get current specializations
    // We can't just add to the list as it is possible that the add did not work out
    QStringList specs = CHARACTER_VALUES->getSkillSpecializations(_currentSkill);

    // Apply to list
    ui->listSpecializations->clear();
    ui->listSpecializations->addItems(specs);

    // Clear text edit
    ui->leSpecValue->setText("");

    emit specializationChanged();
}

//---------------------------------------------------------------------------------
void
SpecializationDialog::on_btnDone_clicked()
{
    hide();
}

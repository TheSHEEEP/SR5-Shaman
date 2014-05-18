#include "knowledgeskilldialog.h"
#include "ui_knowledgeskilldialog.h"

#include "data/dictionary.h"

//---------------------------------------------------------------------------------
KnowledgeSkillDialog::KnowledgeSkillDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KnowledgeSkillDialog)
{
    ui->setupUi(this);

    applyTranslation();

    connect(ui->radioKnowledge, SIGNAL(toggled(bool)), SLOT(handleRadioToggle(bool)));
    connect(ui->radioLanguage, SIGNAL(toggled(bool)), SLOT(handleRadioToggle(bool)));
}

//---------------------------------------------------------------------------------
KnowledgeSkillDialog::~KnowledgeSkillDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
KnowledgeSkillDialog::applyTranslation()
{
    ui->cbKnowledgeType->blockSignals(true);
    ui->cbKnowledgeType->clear();
    ui->cbKnowledgeType->addItem(Dictionary::getTranslation("KNOWLEDGE_TYPE_ACADEMIC"), (int)KNOWLEDGE_TYPE_ACADEMIC);
    ui->cbKnowledgeType->addItem(Dictionary::getTranslation("KNOWLEDGE_TYPE_PROFESSIONAL"), (int)KNOWLEDGE_TYPE_PROFESSIONAL);
    ui->cbKnowledgeType->addItem(Dictionary::getTranslation("KNOWLEDGE_TYPE_INTEREST"), (int)KNOWLEDGE_TYPE_INTEREST);
    ui->cbKnowledgeType->addItem(Dictionary::getTranslation("KNOWLEDGE_TYPE_STREET"), (int)KNOWLEDGE_TYPE_STREET);
    ui->cbKnowledgeType->blockSignals(false);
}

//---------------------------------------------------------------------------------
void
KnowledgeSkillDialog::showEvent(QShowEvent* /**/)
{
    applyTranslation();

    ui->frameKnowledge->hide();
    ui->frameNative->hide();
}

//---------------------------------------------------------------------------------
QString
KnowledgeSkillDialog::getValue() const
{
    return ui->leValue->text();
}

//---------------------------------------------------------------------------------
bool
KnowledgeSkillDialog::getIsKnowledge() const
{
    return ui->radioKnowledge->isChecked();
}

//---------------------------------------------------------------------------------
bool
KnowledgeSkillDialog::getIsNativeTongue() const
{
    return ui->checkNative->isChecked();
}

//---------------------------------------------------------------------------------
KnowledgeType
KnowledgeSkillDialog::getKnowledgeType() const
{
    return (KnowledgeType)ui->cbKnowledgeType->itemData(ui->cbKnowledgeType->currentIndex()).toInt();
}

//---------------------------------------------------------------------------------
void
KnowledgeSkillDialog::on_btnOk_clicked()
{
    // Make sure something is selected
    if ((ui->radioKnowledge->isChecked() || ui->radioLanguage->isChecked()) &&
        ui->leValue->text().size() > 0)
    {
        accept();
    }
}

//---------------------------------------------------------------------------------
void
KnowledgeSkillDialog::on_btnCancel_clicked()
{
    reject();
}

//---------------------------------------------------------------------------------
void
KnowledgeSkillDialog::handleRadioToggle(bool p_toggleState)
{
    QRadioButton* button = static_cast<QRadioButton*>(sender());

    ui->frameKnowledge->setHidden(!(p_toggleState && button == ui->radioKnowledge));
    ui->frameNative->setHidden(!ui->frameKnowledge->isHidden());
}

#include "chareditmisctab.h"
#include "ui_chareditmisctab.h"

#include <QMap>
#include <QString>

#include "data/appstatus.h"
#include "rules/rules.h"
#include "data/character/characterdata.h"

//---------------------------------------------------------------------------------
CharEditMiscTab::CharEditMiscTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharEditMiscTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditMiscTab::~CharEditMiscTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::initialize()
{
    // Fill the metatypes
    ui->cbMetatype->blockSignals(true);
    const QMap<QString, MetatypeDefinition*>& definitions = METATYPE_RULES_PTR->getAllDefinitions();
    QMap<QString, MetatypeDefinition*>::const_iterator it;
    for (it = definitions.begin(); it != definitions.end(); ++it)
    {
        ui->cbMetatype->addItem((*it)->translations[APPSTATUS->getCurrentLocale()], it.key());
    }
    ui->cbMetatype->addItem("", "");
    ui->cbMetatype->setCurrentIndex(definitions.size());
    ui->cbMetatype->blockSignals(false);
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::showEvent(QShowEvent* p_event)
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        ui->btnGuidedContinue->show();
        checkContinue();
    }
    else
    {
        ui->btnGuidedContinue->hide();
    }
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_cbMetatype_currentIndexChanged(int index)
{
    // Clear priority list
    for (int i = 0; i < 6; ++i)
    {
        ui->cbPriority->removeItem(0);
    }

    // Invalidate selection when "nothing" was selected
    QString chosenType = ui->cbMetatype->itemData(index).toString();
    if (chosenType == "")
    {
        CHARACTER->setMetatypeID("");
        checkContinue();
        return;
    }

    // Store selection in character data
    CHARACTER->setMetatypeID(chosenType);

    // Update priority selection
    const MetatypeDefinition& definition = METATYPE_RULES_PTR->getDefinition(chosenType);
    ui->cbPriority->blockSignals(true);
    ui->cbPriority->addItem(QString("A (%1)").arg(definition.specialAttribPointsPerPrio[0]), 0);
    ui->cbPriority->addItem(QString("B (%1)").arg(definition.specialAttribPointsPerPrio[1]), 1);
    ui->cbPriority->addItem(QString("C (%1)").arg(definition.specialAttribPointsPerPrio[2]), 2);
    ui->cbPriority->addItem(QString("D (%1)").arg(definition.specialAttribPointsPerPrio[3]), 3);
    ui->cbPriority->addItem(QString("E (%1)").arg(definition.specialAttribPointsPerPrio[4]), 4);
    ui->cbPriority->addItem("", -1);

    // Apply last selection of priority (if any)
    if (CHARACTER->getPriorityIndex(PRIORITY_METATYPE) != -1)
    {
        ui->cbPriority->setCurrentIndex(CHARACTER->getPriorityIndex(PRIORITY_METATYPE));
    }
    else
    {
        ui->cbPriority->setCurrentIndex(5);
    }
    ui->cbPriority->blockSignals(false);

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_cbPriority_currentIndexChanged(int index)
{
    // Invalidate selection when "nothing" was selected
    int chosenPriority = ui->cbPriority->itemData(index).toInt();
    if (chosenPriority == -1)
    {
        CHARACTER->unsetPriority(PRIORITY_METATYPE);
        checkContinue();
        return;
    }

    // Store selection in character data
    CHARACTER->setPriority(chosenPriority, PRIORITY_METATYPE);

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::checkContinue()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        // Must have a metatype and a priority
        if (CHARACTER->getPriorityIndex(PRIORITY_METATYPE) != -1 &&
            CHARACTER->getMetatypeID() != "")
        {
            ui->btnGuidedContinue->setEnabled(true);
            ui->btnGuidedContinue->setText(tr("Continue"));
        }
        else
        {
            ui->btnGuidedContinue->setEnabled(false);
            ui->btnGuidedContinue->setText(tr("Choose metatype and priority to continue"));
        }
    }
}

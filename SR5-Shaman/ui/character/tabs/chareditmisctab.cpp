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
    const QMap<QString, MetatypeDefinition*>& definitions = METATYPE_RULES_PTR->getAllDefinitions();
    QMap<QString, MetatypeDefinition*>::const_iterator it;
    for (it = definitions.begin(); it != definitions.end(); ++it)
    {
        ui->cbMetatype->addItem((*it)->translations[APPSTATUS->getCurrentLocale()], it.key());
    }
    ui->cbMetatype->addItem("", "");
    ui->cbMetatype->setCurrentIndex(definitions.size());
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
        return;
    }

    // Store selection in character data
    CHARACTER->setMetatypeID(ui->cbMetatype->itemData(index).toString());

    // Update priority selection
    const MetatypeDefinition& definition =
            METATYPE_RULES_PTR->getDefinition(ui->cbMetatype->itemData(index).toString());
    ui->cbPriority->addItem(QString("A (%1)").arg(definition.specialAttribPointsPerPrio[0]), 0);
    ui->cbPriority->addItem(QString("B (%1)").arg(definition.specialAttribPointsPerPrio[1]), 1);
    ui->cbPriority->addItem(QString("C (%1)").arg(definition.specialAttribPointsPerPrio[2]), 2);
    ui->cbPriority->addItem(QString("D (%1)").arg(definition.specialAttribPointsPerPrio[3]), 3);
    ui->cbPriority->addItem(QString("E (%1)").arg(definition.specialAttribPointsPerPrio[4]), 4);
    ui->cbPriority->addItem("", -1);
    ui->cbPriority->setCurrentIndex(5);
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_cbPriority_currentIndexChanged(int index)
{
    // Store selection in character data
    // TODO: HERE!!
}

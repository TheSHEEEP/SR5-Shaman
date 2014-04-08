#include "chareditmisctab.h"
#include "ui_chareditmisctab.h"

#include <QMap>
#include <QString>

#include "ui/utils/priorityeventfilter.h"
#include "data/appstatus.h"
#include "rules/rules.h"
#include "data/character/characterchoices.h"

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
    delete _filter;
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::initialize()
{
    // Set priority delegates
    _filter = new PriorityEventFilter();
    ui->cbPriority->installEventFilter(_filter);
    ui->cbMagicPriority->installEventFilter(_filter);
    connect(ui->cbPriority, SIGNAL(activated(int)), _filter ,SLOT(handlePrioritySelection(int)));
    connect(ui->cbMagicPriority, SIGNAL(activated(int)), _filter ,SLOT(handlePrioritySelection(int)));

    // Fill the metatypes
    ui->cbMetatype->blockSignals(true);
    const QMap<QString, MetatypeDefinition*>& definitions = METATYPE_RULES->getAllDefinitions();
    QMap<QString, MetatypeDefinition*>::const_iterator it;
    for (it = definitions.begin(); it != definitions.end(); ++it)
    {
        ui->cbMetatype->addItem((*it)->translations[APPSTATUS->getCurrentLocale()], it.key());
    }
    ui->cbMetatype->addItem("", "");
    ui->cbMetatype->setCurrentIndex(definitions.size());
    ui->cbMetatype->blockSignals(false);

    // Initialize available magic user types
    ui->cbMagicType->blockSignals(true);
    const QMap<QString, MagicTypeDefinition*>& magicTypeDefs = MAGIC_RULES->getAllMagicTypeDefinitions();
    QMap<QString, MagicTypeDefinition*>::const_iterator mtIt;
    for(mtIt = magicTypeDefs.begin(); mtIt != magicTypeDefs.end(); ++mtIt)
    {
        ui->cbMagicType->addItem((*mtIt)->translations[APPSTATUS->getCurrentLocale()], mtIt.key());
    }
    ui->cbMagicType->addItem("", "");
    ui->cbMagicType->setCurrentIndex(definitions.size());
    ui->cbMagicType->blockSignals(false);

    // Apply translation
    applyTranslation();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::applyTranslation()
{

}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::showEvent(QShowEvent* /*unused*/)
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        ui->btnGuidedContinue->show();
        checkContinue();

        // Update values
        updateValues();

        // Magic user
        ui->magicUserFrame->setEnabled(CHARACTER_CHOICES->getIsMagicUser());
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
    ui->cbPriority->blockSignals(true);
    for (int i = 0; i < 6; ++i)
    {
        ui->cbPriority->removeItem(0);
    }

    // Invalidate selection when "nothing" was selected
    QString chosenType = ui->cbMetatype->itemData(index).toString();
    if (chosenType == "")
    {
        CHARACTER_CHOICES->setMetatypeID("");
        checkContinue();
        return;
    }

    // Store selection in character data
    CHARACTER_CHOICES->setMetatypeID(chosenType);

    // Update priority selection
    const MetatypeDefinition& definition = METATYPE_RULES->getDefinition(chosenType);
    ui->cbPriority->addItem(QString("A (%1)").arg(definition.specialAttribPointsPerPrio[0]), 0);
    ui->cbPriority->addItem(QString("B (%1)").arg(definition.specialAttribPointsPerPrio[1]), 1);
    ui->cbPriority->addItem(QString("C (%1)").arg(definition.specialAttribPointsPerPrio[2]), 2);
    ui->cbPriority->addItem(QString("D (%1)").arg(definition.specialAttribPointsPerPrio[3]), 3);
    ui->cbPriority->addItem(QString("E (%1)").arg(definition.specialAttribPointsPerPrio[4]), 4);
    ui->cbPriority->addItem("", -1);

    // Apply last selection of priority (if any)
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) != -1)
    {
        ui->cbPriority->setCurrentIndex(CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE));
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
        CHARACTER_CHOICES->unsetPriority(PRIORITY_METATYPE);
        checkContinue();
        return;
    }

    // Store selection in character data
    CHARACTER_CHOICES->setPriority(chosenPriority, PRIORITY_METATYPE);

    // It is possible that this "enabled"/"disabled" magic user
    if (CHARACTER_CHOICES->getIsMagicUser())
    {
        ui->checkIsMagicUser->setCheckState(Qt::Checked);
        ui->magicUserFrame->setEnabled(true);
    }
    else
    {
        ui->checkIsMagicUser->setCheckState(Qt::Unchecked);
        ui->magicUserFrame->setEnabled(false);
        ui->cbMagicPriority->setCurrentIndex(ui->cbMagicPriority->count() - 1);
    }

    // Can we move on?
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::updateValues()
{
    // Priority value
    ui->cbPriority->blockSignals(true);
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) != -1)
    {
        ui->cbPriority->setCurrentIndex(CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE));
    }
    else
    {
        ui->cbPriority->setCurrentIndex(5);
    }
    ui->cbPriority->blockSignals(false);
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::checkContinue()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        // Must have a metatype and a priority
        // And, if some kind of magic user is selected, must have a priority and type
        if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) != -1 &&
            CHARACTER_CHOICES->getMetatypeID() != "")
        {
            if((!CHARACTER_CHOICES->getIsMagicUser() && !ui->checkIsMagicUser->isChecked()) ||
                 (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC) <= 3 &&
                  CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC) >= 0 &&
                  CHARACTER_CHOICES->getMagicUserTypeID() != ""))
            {
                ui->btnGuidedContinue->setEnabled(true);
                ui->btnGuidedContinue->setText(tr("Continue"));
            }
            else
            {
                ui->btnGuidedContinue->setEnabled(false);
                ui->btnGuidedContinue->setText(tr("You need to select a magic user priority and type"));
            }
        }
        else
        {
            ui->btnGuidedContinue->setEnabled(false);
            ui->btnGuidedContinue->setText(tr("Choose metatype and priority to continue"));

            emit disableNext();
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_leRealName_textEdited(const QString& p_name)
{
    CHARACTER_CHOICES->setName(p_name);
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_leAlias_textEdited(const QString& p_nick)
{
    CHARACTER_CHOICES->setNick(p_nick);
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_btnGuidedContinue_clicked()
{
    emit guidedNextStep();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_checkIsMagicUser_stateChanged(int p_state)
{
    CHARACTER_CHOICES->setIsMagicUser(p_state == Qt::Checked);
    ui->magicUserFrame->setEnabled(p_state == Qt::Checked);

    // This possibly invalidates metatype priority
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) == -1)
    {
        ui->cbPriority->setCurrentIndex(5);
    }

    // Unselect any magic settings
    if (ui->cbMagicType->count() > 0)
    {
        ui->cbMagicType->setCurrentIndex(ui->cbMagicType->count() - 1);
    }
    if (ui->cbMagicPriority->count() > 0)
    {
        ui->cbMagicPriority->setCurrentIndex(ui->cbMagicPriority->count() - 1);
    }

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_cbMagicType_currentIndexChanged(int p_index)
{
    QString type = ui->cbMagicType->itemData(p_index).toString();
    // Unselect magic type
    if (type == "")
    {
        CHARACTER_CHOICES->setMagicUserType("");
        CHARACTER_CHOICES->setPriority(4, PRIORITY_MAGIC);
        if (ui->cbMagicPriority->count() > 0)
        {
            ui->cbMagicPriority->blockSignals(true);
            ui->cbMagicPriority->setCurrentIndex(ui->cbMagicPriority->count() - 1);
            ui->cbMagicPriority->blockSignals(false);
        }
        return;
    }

    // Select magic type
    CHARACTER_CHOICES->setMagicUserType(type);

    // Unset priority
    CHARACTER_CHOICES->unsetPriority(PRIORITY_MAGIC);
    CHARACTER_CHOICES->resetFreeSkills();
    CHARACTER_CHOICES->resetFreeSpells();

    // Clear priority selection// Clear priority list
    ui->cbMagicPriority->blockSignals(true);
    while (ui->cbMagicPriority->count() > 0)
    {
        ui->cbMagicPriority->removeItem(0);
    }

    // Fill priority selection
    const MagicTypeDefinition& definition = MAGIC_RULES->getMagicTypeDefinition(type);
    if (definition.priorities.contains(0))
    {
        ui->cbMagicPriority->addItem("A", 0);
    }
    if (definition.priorities.contains(1))
    {
        ui->cbMagicPriority->addItem("B", 1);
    }
    if (definition.priorities.contains(2))
    {
        ui->cbMagicPriority->addItem("C", 2);
    }
    if (definition.priorities.contains(3))
    {
        ui->cbMagicPriority->addItem("D", 3);
    }
    if (definition.priorities.contains(4))
    {
        ui->cbMagicPriority->addItem("E", 4);
    }
    ui->cbMagicPriority->addItem("", -1);
    ui->cbMagicPriority->setCurrentIndex(ui->cbMagicPriority->count() - 1);
    ui->cbMagicPriority->blockSignals(false);

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::on_cbMagicPriority_currentIndexChanged(int p_index)
{
    int prio = ui->cbMagicPriority->itemData(p_index).toInt();

    // Unset priority
    if (prio == -1)
    {
        CHARACTER_CHOICES->unsetPriority(PRIORITY_MAGIC);
        checkContinue();
        return;
    }

    // Set priority
    CHARACTER_CHOICES->setPriority(prio, PRIORITY_MAGIC);
    CHARACTER_CHOICES->resetFreeSkills();
    CHARACTER_CHOICES->resetFreeSpells();

    // This possibly unselected the metatype priority
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) == -1)
    {
        ui->cbPriority->setCurrentIndex(5);
    }

    checkContinue();
}

#include "chareditmagictab.h"
#include "ui_chareditmagictab.h"

#include "data/appstatus.h"
#include "rules/rules.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "ui/models/skilltreemodel.h"

//---------------------------------------------------------------------------------
CharEditMagicTab::CharEditMagicTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditMagicTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditMagicTab::~CharEditMagicTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::initialize()
{
    // Initialize the skill views
    SkillTreeModel* skillTreeModel = new SkillTreeModel();
    skillTreeModel->initialize();
    ui->treeSkillsAvailable->setModel(skillTreeModel);
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::showEvent(QShowEvent* /*unused*/)
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        ui->btnGuidedContinue->show();

        // Show / hide / update different ui widgets
        QString magicTypeID = CHARACTER_CHOICES->getMagicUserTypeID();
        int prio = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC);
        if (magicTypeID != "")
        {
            const MagicTypeDefinition& def = MAGIC_RULES->getDefinition(magicTypeID);
            ui->lblMagicTypeValue->setText(def.translations[APPSTATUS->getCurrentLocale()]);

            // Possibly show aspected frame
            ui->frameAspected->setVisible(std::find(def.types.begin(), def.types.end(), "aspected") != def.types.end());

            // Possibly show free skills
            if (def.priorities.contains(prio))
            {
                bool show = false;
                if (def.priorities[prio]->freeSkillGroup.first > 0)
                {
                    show = true;
                }
                if (def.priorities[prio]->freeSkills.first > 0)
                {
                    show = true;
                }
                ui->frameFreeSkills->setVisible(show);

                // Disable it if no aspect is selected
                if (ui->frameAspected->isVisible() &&
                    (ui->cbAspect->count() == 0 ||
                    ui->cbAspect->currentIndex() == (ui->cbAspect->count() - 1)))
                {
                    ui->frameFreeSkills->setEnabled(false);
                }
                else
                {
                    ui->frameFreeSkills->setEnabled(true);
                }

                // TODO: Update skill view attributes
            }
        }
        else
        {
            ui->lblMagicTypeValue->setText(tr("None"));
        }

        // Update priority
        if (prio != -1 && prio < 4)
        {
            switch (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC))
            {
            case 0:
                ui->lblPriorityValue->setText("A");
                break;
            case 1:
                ui->lblPriorityValue->setText("B");
                break;
            case 2:
                ui->lblPriorityValue->setText("C");
                break;
            case 3:
                ui->lblPriorityValue->setText("D");
                break;
            }
        }
        else
        {
            ui->lblPriorityValue->setText("E");
        }

        // Update values
        updateValues();

        checkContinue();
    } // END if guided creation
    else
    {
        ui->btnGuidedContinue->hide();
    }
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::updateValues()
{
    if (!isEnabled())
    {
        return;
    }

    // Free spells / complex forms / powers
    // TODO: Implement possibility to spend free spells / power points / resonance
//    ui->lblFreeSpellsValue->setText(QString("%1 / %2")
//                                .arg(CHARACTER_CHOICES->getAvailableAttributePoints())
//                                .arg(ATTRIBUTE_RULES->getNumFreebies(
//                                         CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES))));

    // Karma points
    ui->lblKarmaValue->setText(QString("%1 / %2")
                                .arg(CHARACTER_CHOICES->getAvailableKarma())
                                .arg(CHARACTER_VALUES->getKarmaPool()));
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::checkContinue()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        // Either continue because this is not a magic user, or check the other conditions
        if (!CHARACTER_CHOICES->getIsMagicUser())
        {
            ui->btnGuidedContinue->setEnabled(true);
            ui->btnGuidedContinue->setText(tr("Continue"));
        }
        else
        {
            ui->btnGuidedContinue->setEnabled(false);
            ui->btnGuidedContinue->setText(tr("Select skills and spells / complex forms to continue"));

            emit disableNext();
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnGuidedContinue_clicked()
{
    emit guidedNextStep();
}

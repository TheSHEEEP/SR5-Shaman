#include "chareditmagictab.h"
#include "ui_chareditmagictab.h"

#include "data/appstatus.h"
#include "rules/rules.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "ui/models/skilltreemodel.h"
#include "ui/models/skillsortfilterproxymodel.h"

//---------------------------------------------------------------------------------
CharEditMagicTab::CharEditMagicTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditMagicTab)
    , _skillFilter(NULL)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditMagicTab::~CharEditMagicTab()
{
    // Cannot delete the model I created, or it crashes..
//    delete ((SkillSortFilterProxyModel*)ui->treeSkillsAvailable->model())->sourceModel();
//    delete ui->treeSkillsAvailable->model();
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::initialize()
{
    // Initialize the skill view
    SkillTreeModel* skillTreeModel = new SkillTreeModel();
    skillTreeModel->initialize();

    _skillFilter = new SkillSortFilterProxyModel(ui->treeSkillsAvailable);
    _skillFilter->setSourceModel(skillTreeModel);
    std::vector<SkillType> filterTypes;
    filterTypes.push_back(SKILL_TYPE_INVALID);
    _skillFilter->setFilterTypes(filterTypes);
    _skillFilter->setFilterMask(SKILL_FILTERMASK_TYPE);
    _skillFilter->setShowEmptyCategories(false);
    _skillFilter->applyFilter();

    ui->treeSkillsAvailable->setModel(_skillFilter);
    ui->treeSkillsAvailable->setSortingEnabled(true);

    // Fill available aspects
    ui->cbAspect->blockSignals(true);
    std::vector<std::pair<QString, SkillDefinition*> > magicGroups =
            SKILL_RULES->getDefinitionsByType(SKILL_TYPE_MAGIC, true);
    for (int i = 0; i < magicGroups.size(); ++i)
    {
        ui->cbAspect->addItem(  magicGroups[i].second->translations[APPSTATUS->getCurrentLocale()],
                                magicGroups[i].first);
    }
    ui->cbAspect->addItem(QString(" "), "INVALID");
    ui->cbAspect->setCurrentIndex(magicGroups.size());
    ui->cbAspect->blockSignals(false);
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

            showHideSkillsViews();
        } // END we have a magic type
        else
        {
            ui->lblMagicTypeValue->setText(tr("None"));
            ui->frameFreeSkills->setVisible(false);
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
CharEditMagicTab::showHideSkillsViews()
{
    QString magicTypeID = CHARACTER_CHOICES->getMagicUserTypeID();
    int prio = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC);
    const MagicTypeDefinition& def = MAGIC_RULES->getDefinition(magicTypeID);

    // Possibly show free skills
    if (def.priorities.contains(prio))
    {
        // Do we have free groups or skills
        bool showGroups = false;
        bool showSkills = false;
        if (def.priorities[prio]->freeSkillGroup.first > 0)
        {
            showGroups = true;
        }
        if (def.priorities[prio]->freeSkills.first > 0)
        {
            showSkills = true;
        }

        // Filter the kinds of skills we are to show
        if (showGroups || showSkills)
        {
            std::vector<SkillType>& filterTypes = _skillFilter->getFilterTypes();
            filterTypes.clear();
            if (std::find(def.types.begin(), def.types.end(), "magic") != def.types.end())
            {
                filterTypes.push_back(SKILL_TYPE_MAGIC);
            }
            if (std::find(def.types.begin(), def.types.end(), "resonance") != def.types.end())
            {
                filterTypes.push_back(SKILL_TYPE_RESONANCE);
            }
            if (std::find(def.types.begin(), def.types.end(), "adept") != def.types.end())
            {
                filterTypes.push_back(SKILL_TYPE_COMBAT);
                filterTypes.push_back(SKILL_TYPE_PHYSICAL);
                filterTypes.push_back(SKILL_TYPE_SOCIAL);
                filterTypes.push_back(SKILL_TYPE_TECHNICAL);
                filterTypes.push_back(SKILL_TYPE_VEHICLE);
            }

            // Show only groups, only normal skills, or both?
            _skillFilter->setShowSkillGroups(showGroups);
            _skillFilter->setShowNormalSkills(showSkills);

            // Apply
            _skillFilter->setFilterMask(SKILL_FILTERMASK_TYPE | SKILL_FILTERMASK_ID_CONTAINS);
            _skillFilter->applyFilter();
        }

        // Set visibility
        ui->frameFreeSkills->setVisible(showGroups || showSkills);

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
    }
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnGuidedContinue_clicked()
{
    emit guidedNextStep();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_cbAspect_currentIndexChanged(int p_index)
{
    // Update skills filter
    QStringList ids;
    ids.push_back(ui->cbAspect->itemData(p_index).toString());
    _skillFilter->setFilterIDContains(ids);

    // Show or hide the skills views
    showHideSkillsViews();
}

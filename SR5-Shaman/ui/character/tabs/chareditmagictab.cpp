#include "chareditmagictab.h"
#include "ui_chareditmagictab.h"

#include <QDebug>

#include "data/appstatus.h"
#include "rules/rules.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "ui/models/skilltreemodel.h"
#include "ui/models/skillsortfilterproxymodel.h"
#include "ui/models/skilldelegate.h"

//---------------------------------------------------------------------------------
CharEditMagicTab::CharEditMagicTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditMagicTab)
    , _skillsAvailableFilter(NULL)
    , _skillsAvailableDelegate(NULL)
    , _skillsFilter(NULL)
    , _skillsDelegate(NULL)
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
    // Initialize the skill views
    // Available skills
    SkillTreeModel* skillTreeModel = new SkillTreeModel();
    skillTreeModel->initialize();
    // Filter
    _skillsAvailableFilter = new SkillSortFilterProxyModel(ui->treeSkillsAvailable);
    _skillsAvailableFilter->setSourceModel(skillTreeModel);
    std::vector<SkillType> filterTypes;
    filterTypes.push_back(SKILL_TYPE_INVALID);
    _skillsAvailableFilter->setFilterTypes(filterTypes);
    _skillsAvailableFilter->setFilterMask(SKILL_FILTERMASK_TYPE);
    _skillsAvailableFilter->setShowEmptyCategories(false);
    _skillsAvailableFilter->applyFilter();
    // Model & sorting
    ui->treeSkillsAvailable->setModel(_skillsAvailableFilter);
    ui->treeSkillsAvailable->setSortingEnabled(true);
    // Delegate
    _skillsAvailableDelegate = new SkillDelegate();
    ui->treeSkillsAvailable->setItemDelegate(_skillsAvailableDelegate);
    // Handle selection & drag
    connect(ui->treeSkillsAvailable->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                                        SLOT(handleSkillChanged(QModelIndex,QModelIndex)));

    // Selected skills
    skillTreeModel = new SkillTreeModel();
    skillTreeModel->initialize();
    // Filter
    _skillsFilter = new SkillSortFilterProxyModel(ui->treeSkills);
    _skillsFilter->setSourceModel(skillTreeModel);
    _skillsFilter->getFilterIDContains().clear();
    _skillsFilter->setFilterMask(SKILL_FILTERMASK_ID_EQUALS);
    _skillsFilter->setShowEmptyCategories(false);
    _skillsFilter->applyFilter();
    // Model & sorting
    ui->treeSkills->setModel(_skillsFilter);
    ui->treeSkills->setSortingEnabled(true);
    // Delegate
    _skillsDelegate = new SkillDelegate();
    ui->treeSkills->setItemDelegate(_skillsDelegate);
    // Handle selection & drag
    connect(ui->treeSkills->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                               SLOT(handleSkillChanged(QModelIndex,QModelIndex)));


    // Fill available aspects
    ui->cbAspect->blockSignals(true);
    std::vector<std::pair<QString, SkillDefinition*> > magicGroups =
            SKILL_RULES->getDefinitionsByType(SKILL_TYPE_MAGIC, true);
    for (uint i = 0; i < magicGroups.size(); ++i)
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

    if (CHARACTER_CHOICES->getIsMagicUser())
    {
        // Free skills
        std::pair<int, int> freeSkills = MAGIC_RULES->getDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                                            .priorities[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)]
                                                ->freeSkills;
        if (freeSkills.first < 0)
        {
            freeSkills.first = 0;
        }
        ui->lblFreeSkillsValue->setText(QString("%1 / %2")
                                        .arg(CHARACTER_CHOICES->getAvailableFreeSkills(false))
                                        .arg(freeSkills.first));

        // Free skill groups
        freeSkills = MAGIC_RULES->getDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                        .priorities[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)]
                            ->freeSkillGroup;
        if (freeSkills.first < 0)
        {
            freeSkills.first = 0;
        }
        ui->lblFreeSkillGroupsValue->setText(QString("%1 / %2")
                                        .arg(CHARACTER_CHOICES->getAvailableFreeSkills(true))
                                        .arg(freeSkills.first));

        // Free spells / complex forms / powers
        // TODO: Implement possibility to spend free spells / power points / resonance
    //    ui->lblFreeSpellsValue->setText(QString("%1 / %2")
    //                                .arg(CHARACTER_CHOICES->getAvailableAttributePoints())
    //                                .arg(ATTRIBUTE_RULES->getNumFreebies(
    //                                         CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES))));
    }

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
            // TODO: Mystical Adept is a special case not handled correctly here, need to have a look at the ruleset
            std::vector<SkillType>& filterTypes = _skillsAvailableFilter->getFilterTypes();
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
            _skillsAvailableFilter->setShowSkillGroups(showGroups);
            _skillsAvailableFilter->setShowNormalSkills(showSkills);

            // Set Mask
            int mask = SKILL_FILTERMASK_TYPE;
            if (ui->frameAspected->isVisible())
            {
                mask |= SKILL_FILTERMASK_ID_EQUALS;
            }
            _skillsAvailableFilter->setFilterMask(mask);

            // Apply
            _skillsAvailableFilter->applyFilter();
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

        // Clear the skills view if the free skills were reset
        const MagicTypePriorityDefinition* magicPrioDef =
                MAGIC_RULES->getDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                    .priorities[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)];
        if (CHARACTER_CHOICES->getAvailableFreeSkills(false) >= magicPrioDef->freeSkills.first &&
            CHARACTER_CHOICES->getAvailableFreeSkills(true) >= magicPrioDef->freeSkillGroup.first)
        {
            _skillsFilter->getFilterIDEquals().clear();
            _skillsFilter->applyFilter();
        }
    } // END if we have free skills
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
    _skillsAvailableFilter->setFilterIDEquals(ids);

    // Show or hide the skills views
    showHideSkillsViews();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::handleSkillChanged(const QModelIndex& p_current, const QModelIndex& p_previous)
{
    QItemSelectionModel* model = static_cast<QItemSelectionModel*>(sender());
    bool isSkillView = model == ui->treeSkills->selectionModel();
    QPushButton* button = isSkillView ? ui->btnRemoveSkill : ui->btnAddSkill;

    // If this is adding and we have no more free skills, disable
    if (!isSkillView &&
        CHARACTER_CHOICES->getAvailableFreeSkills(false) == 0 &&
        CHARACTER_CHOICES->getAvailableFreeSkills(true) == 0)
    {
        button->setEnabled(false);
        return;
    }

    // We somehow selected an invalid item
    if (!p_current.isValid())
    {
        button->setEnabled(false);
        return;
    }

    // If the item is a category, we can't remove/add it
    SkillModelItem* item = static_cast<SkillModelItem*>(p_current.data().value<void*>());
    if (item->isCategory)
    {
        button->setEnabled(false);
        return;
    }

    // If the item is the child of a group, we can't remove/add it
    if (item->parent && !item->parent->isCategory &&
        SKILL_RULES->getDefinition(item->parent->id).isGroup)
    {
        button->setEnabled(false);
        return;
    }

    // It is a removable/addable item, so enable the button
    button->setEnabled(true);
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnAddSkill_clicked()
{
    // Get the currently selected skill
    SkillModelItem* item = static_cast<SkillModelItem*>(ui->treeSkillsAvailable->currentIndex().data().value<void*>());

    // Add the skill to the list (prevent double adding)
    QStringList& filterIDs = _skillsFilter->getFilterIDEquals();
    if (!filterIDs.contains(item->id))
    {
        filterIDs.push_back(item->id);
    }

    // Take note of the choice
    CHARACTER_CHOICES->addFreeSkill(item->id, PRIORITY_MAGIC);

    // Update the display
    _skillsFilter->applyFilter();
    ui->treeSkills->expandAll();

    // Disable the add button if we have no more free skills
    // If this is adding and we have no more free skills, disable
    if (CHARACTER_CHOICES->getAvailableFreeSkills(false) == 0 &&
        CHARACTER_CHOICES->getAvailableFreeSkills(true) == 0)
    {
        ui->btnAddSkill->setEnabled(false);
    }

    // Update shown values
    updateValues();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnRemoveSkill_clicked()
{
    // Get the currently selected skill
    SkillModelItem* item = static_cast<SkillModelItem*>(ui->treeSkills->currentIndex().data().value<void*>());

    // Remove the skill from the list
    QStringList& filterIDs = _skillsFilter->getFilterIDEquals();
    for (int i = 0; i < filterIDs.size(); ++i)
    {
        if (filterIDs[i] == item->id)
        {
            filterIDs.removeAt(i);
            break;
        }
    }

    // Take note of the choice
    CHARACTER_CHOICES->removeFreeSkill(item->id);

    // Update the display
    _skillsFilter->applyFilter();
    ui->treeSkills->expandAll();

    // Re-select the current skill from the available list to cause updating the add button
    QModelIndex currentIndex(ui->treeSkillsAvailable->currentIndex());
    ui->treeSkillsAvailable->setCurrentIndex(QModelIndex());
    ui->treeSkillsAvailable->setCurrentIndex(currentIndex);

    // As we have removed a skill, disable the remove button
    ui->btnRemoveSkill->setEnabled(false);

    // Update shown values
    updateValues();
}

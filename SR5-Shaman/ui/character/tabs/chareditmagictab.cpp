#include "chareditmagictab.h"
#include "ui_chareditmagictab.h"

#include <QDebug>
#include <QKeyEvent>

#include "data/appstatus.h"
#include "rules/rules.h"
#include "rules/misc/customchoice.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "ui/models/skilltreemodel.h"
#include "ui/models/skillsortfilterproxymodel.h"
#include "ui/models/skilldelegate.h"
#include "ui/models/magictreemodel.h"
#include "ui/models/magicsortfilterproxymodel.h"
#include "ui/models/magicdelegate.h"
#include "ui/character/popups/customdescriptorpopup.h"

//---------------------------------------------------------------------------------
CharEditMagicTab::CharEditMagicTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditMagicTab)
    , _skillsAvailableFilter(NULL)
    , _skillsAvailableDelegate(NULL)
    , _skillsFilter(NULL)
    , _skillsDelegate(NULL)
    , _spellsAvailableFilter(NULL)
    , _spellsAvailableDelegate(NULL)
    , _spellsFilter(NULL)
    , _spellsDelegate(NULL)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditMagicTab::~CharEditMagicTab()
{
    // TODO: Created Models, filters, etc. should be deleted here. But doing so crashes the app. Huh.
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
    ui->treeSkillsAvailable->setHeaderHidden(true);
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
    _skillsFilter->getFilterIDEquals().clear();
    _skillsFilter->setFilterMask(SKILL_FILTERMASK_ID_EQUALS);
    _skillsFilter->setShowEmptyCategories(false);
    _skillsFilter->setShowUserSkills(true);
    _skillsFilter->applyFilter();
    // Model & sorting
    ui->treeSkills->setModel(_skillsFilter);
    ui->treeSkills->setSortingEnabled(true);
    // Delegate
    _skillsDelegate = new SkillDelegate();
    ui->treeSkills->setItemDelegate(_skillsDelegate);
    ui->treeSkills->setHeaderHidden(true);
    // Handle selection & drag
    connect(ui->treeSkills->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                               SLOT(handleSkillChanged(QModelIndex,QModelIndex)));

    // Available spells/powers/complex forms
    MagicTreeModel* magicTreeModel = new MagicTreeModel();
    magicTreeModel->initialize();
    // Filter
    _spellsAvailableFilter = new MagicSortFilterProxyModel(ui->treeSkills);
    _spellsAvailableFilter->setSourceModel(magicTreeModel);
    std::vector<MagicAbilityType> filterTypesSpells;
    filterTypesSpells.push_back(MAGICABILITYTYPE_INVALID);
    _spellsAvailableFilter->setFilterTypes(filterTypesSpells);
    _spellsAvailableFilter->setFilterMask(MAGIC_FILTERMASK_TYPE);
    _spellsAvailableFilter->setShowEmptyCategories(false);
    _spellsAvailableFilter->applyFilter();
    // Model & sorting
    ui->treeSpellsAvailable->setModel(_spellsAvailableFilter);
    ui->treeSpellsAvailable->setSortingEnabled(true);
    // Delegate
    _spellsAvailableDelegate = new MagicDelegate();
    ui->treeSpellsAvailable->setItemDelegate(_spellsAvailableDelegate);
    ui->treeSpellsAvailable->setHeaderHidden(true);
    // Handle selection & drag
    connect(ui->treeSpellsAvailable->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                                        SLOT(handleSpellChanged(QModelIndex,QModelIndex)));

    // Selected spells/powers/complex forms
    magicTreeModel = new MagicTreeModel();
    magicTreeModel->initialize();
    // Filter
    _spellsFilter = new MagicSortFilterProxyModel(ui->treeSpells);
    _spellsFilter->setSourceModel(magicTreeModel);
    _spellsFilter->getFilterIDContains().clear();
    _spellsFilter->getFilterIDEquals().clear();
    _spellsFilter->setFilterMask(MAGIC_FILTERMASK_ID_EQUALS);
    _spellsFilter->setShowEmptyCategories(false);
    _spellsFilter->applyFilter();
    // Model & sorting
    ui->treeSpells->setModel(_spellsFilter);
    ui->treeSpells->setSortingEnabled(true);
    // Delegate
    _spellsDelegate = new MagicDelegate();
    _spellsDelegate->setShowAdeptPowerLevel(true);
    ui->treeSpells->setItemDelegate(_spellsDelegate);
    ui->treeSpells->setHeaderHidden(true);
    // Handle selection & drag
    connect(ui->treeSpells->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                               SLOT(handleSpellChanged(QModelIndex,QModelIndex)));

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

    // Connect the skill view groupbox to the show/hide of its frame
    connect(ui->groupBoxSkills, SIGNAL(clicked(bool)), SLOT(handleSkillsBoxChecked(bool)));

    // Apply translation
    applyTranslation();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::applyTranslation()
{

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

        // We have a magic user
        if (magicTypeID != "")
        {
            const MagicTypeDefinition& def = MAGIC_RULES->getMagicTypeDefinition(magicTypeID);
            ui->lblMagicTypeValue->setText(def.translations[APPSTATUS->getCurrentLocale()]);

            // Possibly show aspected frame
            ui->frameAspected->setVisible(std::find(def.types.begin(), def.types.end(), "aspected") != def.types.end());

            // Show or hide the free skills views
            showHideSkillsViews();

            // Show or hide the spells views
            showHideSpellsViews();

            // Enable the power point purchase if this is a mystic adept (is this the only case?)
            if (std::find(def.types.begin(), def.types.end(), "magic") != def.types.end() &&
                std::find(def.types.begin(), def.types.end(), "adept") != def.types.end())
            {
                ui->spinPurchasePP->setEnabled(true);
                ui->spinPurchasePP->setMaximum(CHARACTER_VALUES->getAttribute("magic"));
                ui->spinPurchasePP->setValue(CHARACTER_CHOICES->getPurchasedPowerPoints());
            }
            else
            {
                ui->spinPurchasePP->setEnabled(false);
            }
        }
        else
        {
            ui->lblMagicTypeValue->setText(tr("None"));
            ui->frameFreeSkills->setVisible(false);
            ui->groupBoxSkills->setVisible(false);
            ui->spinPurchasePP->setEnabled(false);
            ui->frameAspected->setVisible(false);
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
CharEditMagicTab::keyPressedEvent(QKeyEvent* p_keyEvent)
{
    switch (p_keyEvent->key())
    {
    case Qt::Key_NumberSign:
    case Qt::Key_Apostrophe:
        // Fast-forward mode for testing
        if (p_keyEvent->modifiers() & Qt::ShiftModifier)
        {
            ui->btnGuidedContinue->setEnabled(true);
            emit guidedNextStep();
        }
        break;

    default:
        break;
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
        MagicTypePriorityDefinition* typePriority =
                MAGIC_RULES->getMagicTypeDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                    .priorities[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)];
        // Free skills
        std::pair<int, int> freeSkills = typePriority->freeSkills;
        if (freeSkills.first < 0)
        {
            freeSkills.first = 0;
        }
        ui->lblFreeSkillsValue->setText(QString("%1 / %2")
                                        .arg(CHARACTER_CHOICES->getAvailableFreeSkills(false))
                                        .arg(freeSkills.first));

        // Free skill groups
        freeSkills = typePriority->freeSkillGroup;
        if (freeSkills.first < 0)
        {
            freeSkills.first = 0;
        }
        ui->lblFreeSkillGroupsValue->setText(QString("%1 / %2")
                                        .arg(CHARACTER_CHOICES->getAvailableFreeSkills(true))
                                        .arg(freeSkills.first));

        // Free spells / complex forms
        float freeSpells = typePriority->freeSpells >= 0? typePriority->freeSpells : 0;
        ui->lblFreeSpellsValue->setText(QString("%1 / %2")
                                    .arg((int)CHARACTER_CHOICES->getAvailableFreeSpells())
                                    .arg(freeSpells));

        // Power points
        ui->lblPowerPointsValue->setText(QString("%1 / %2")
                                    .arg(CHARACTER_CHOICES->getAvailablePowerPoints())
                                    .arg((int)CHARACTER_CHOICES->getPowerPoints()));
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
            // Continue if all freebies were spent
            if (CHARACTER_CHOICES->getAvailableFreeSpells() > 0.0f ||
                CHARACTER_CHOICES->getAvailableFreeSkills(false) > 0 ||
                CHARACTER_CHOICES->getAvailableFreeSkills(true) > 0)
            {
                ui->btnGuidedContinue->setEnabled(false);
                ui->btnGuidedContinue->setText(tr("Select skills and spells / complex forms to continue"));

                emit disableNext();
            }
            else
            {
                ui->btnGuidedContinue->setEnabled(true);
                ui->btnGuidedContinue->setText(tr("Continue"));
            }
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::showHideSkillsViews()
{
    QString magicTypeID = CHARACTER_CHOICES->getMagicUserTypeID();
    int prio = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC);
    const MagicTypeDefinition& def = MAGIC_RULES->getMagicTypeDefinition(magicTypeID);

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
            // Mystical adepts are a special case as they do not get mundane skills for free
            // TODO: Ugly hack. There should be a better way.
            if (std::find(def.types.begin(), def.types.end(), "adept") != def.types.end() &&
                magicTypeID != "MYSTIC_ADEPT")
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
        ui->groupBoxSkills->setVisible(showGroups || showSkills);
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

        // Reset the skills view
        _skillsFilter->getFilterIDEquals().clear();
        QStringList skills = CHARACTER_CHOICES->getChosenFreeSkills();
        for (int i = 0; i < skills.size(); ++i)
        {
            _skillsFilter->getFilterIDEquals().push_back(skills[i]);
        }
        _skillsFilter->applyFilter();
    } // END if we have free skills
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::showHideSpellsViews()
{
    QString magicTypeID = CHARACTER_CHOICES->getMagicUserTypeID();
    int prio = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC);
    const MagicTypeDefinition& def = MAGIC_RULES->getMagicTypeDefinition(magicTypeID);

    // Possibly show spells
    if (def.priorities.contains(prio))
    {
        // Filter the kinds of spells we are to show
        std::vector<MagicAbilityType>& filterTypes = _spellsAvailableFilter->getFilterTypes();
        filterTypes.clear();
        if (std::find(def.types.begin(), def.types.end(), "magic") != def.types.end())
        {
            filterTypes.push_back(MAGICABILITYTYPE_SPELL);
        }
        if (std::find(def.types.begin(), def.types.end(), "resonance") != def.types.end())
        {
            filterTypes.push_back(MAGICABILITYTYPE_COMPLEX_FORM);
        }
        if (std::find(def.types.begin(), def.types.end(), "adept") != def.types.end())
        {
            filterTypes.push_back(MAGICABILITYTYPE_ADEPT_POWER);
        }

        // Set Mask
        int mask = MAGIC_FILTERMASK_TYPE;
        _spellsAvailableFilter->setFilterMask(mask);

        // Apply
        _spellsAvailableFilter->applyFilter();
    }

    // Reset the spells view
    _spellsFilter->getFilterIDEquals().clear();
    QStringList spells = CHARACTER_CHOICES->getChosenFreeSpells();
    for (int i = 0; i < spells.size(); ++i)
    {
        _spellsFilter->getFilterIDEquals().push_back(spells[i]);
    }
    _spellsFilter->applyFilter();
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
    SkillDefinition* item = static_cast<SkillDefinition*>(p_current.data().value<void*>());
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
CharEditMagicTab::handleSpellChanged(const QModelIndex& p_current, const QModelIndex& p_previous)
{
    QItemSelectionModel* model = static_cast<QItemSelectionModel*>(sender());
    bool isSpellsView = model == ui->treeSpells->selectionModel();
    QPushButton* button = isSpellsView ? ui->btnRemoveSpell : ui->btnAddSpell;

    // If this is adding and we have no more free skills or power points, disable
    if (!isSpellsView &&
        CHARACTER_CHOICES->getAvailableFreeSpells() == 0 &&
        CHARACTER_CHOICES->getAvailablePowerPoints() == 0)
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
    MagicAbilityDefinition* item = static_cast<MagicAbilityDefinition*>(p_current.data().value<void*>());
    if (item->isCategory ||
        (item->abilityType == MAGICABILITYTYPE_SPELL && item->spell->isSpellCategory))
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
    SkillDefinition* item = static_cast<SkillDefinition*>(ui->treeSkillsAvailable->currentIndex().data().value<void*>());
    QString id = item->id;

    // If this requires a custom choice, show the modal window
    QString customValue = "";
    if (item->requiresCustom)
    {
        // Show the popup and get its result
        CustomDescriptorPopup popup(NULL, item->translations[APPSTATUS->getCurrentLocale()],
                                    false, true);
        int result = popup.exec();
        if (result == QDialog::Rejected)
        {
            return;
        }
        customValue = popup.getCustomization();

        // Construct a new custom skill (this will do nothing if it already exists)
        id = SKILL_RULES->constructCustomizedSkill(id, customValue);
    }

    // Take note of the choice
    CHARACTER_CHOICES->addFreeSkill(id, PRIORITY_MAGIC);

    // Add the skill to the list (prevent double adding)
    QStringList& filterIDs = _skillsFilter->getFilterIDEquals();
    if (!filterIDs.contains(id))
    {
        filterIDs.push_back(id);
    }

    // Update the display
    _skillsFilter->applyFilter();
    ui->treeSkills->expandAll();

    // Disable the add button if we have no more free skills
    // If this is adding and we have no more free skills, disable
    if (CHARACTER_CHOICES->getAvailableFreeSkills(false) == 0 &&
        CHARACTER_CHOICES->getAvailableFreeSkills(true) == 0 &&
        CHARACTER_CHOICES->getAvailablePowerPoints() == 0)
    {
        ui->btnAddSkill->setEnabled(false);
    }

    // Update shown values
    updateValues();

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnRemoveSkill_clicked()
{
    // Get the currently selected skill
    SkillDefinition* item = static_cast<SkillDefinition*>(ui->treeSkills->currentIndex().data().value<void*>());

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

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnAddSpell_clicked()
{
    // Get the currently selected skill
    MagicAbilityDefinition* item = static_cast<MagicAbilityDefinition*>(
                ui->treeSpellsAvailable->currentIndex().data().value<void*>());
    QString id = item->id;

    // If this requires a custom choice, show the modal window
    QString customValue = "";
    bool needsLevel = (item->abilityType == MAGICABILITYTYPE_ADEPT_POWER
                       &&
                       (item->adeptPower->costType == COSTTYPE_ARRAY ||
                       item->adeptPower->costType == COSTTYPE_PER_LEVEL));
    int numAdds = 0;
    if (item->requiresCustom || needsLevel)
    {
        // Create the popup and fill it
        CustomDescriptorPopup popup(NULL, item->translations[APPSTATUS->getCurrentLocale()],
                                    false, item->requiresCustom, needsLevel);
        if (item->customChoices)
        {
            item->customChoices->fillDescriptorPopup(&popup);
        }
        if (needsLevel)
        {
            // The cost array can be used as-is
            if (item->adeptPower->costType == COSTTYPE_ARRAY)
            {
                popup.setLevels(item->adeptPower->costArray);
            }
            // The maximum level must be calculated here
            else if (item->adeptPower->costType == COSTTYPE_PER_LEVEL)
            {
                std::vector<float> levels;
                int maxLevel = CHARACTER_VALUES->getAttribute("magic");
                for (int i = 1; i <= maxLevel; ++i)
                {
                    levels.push_back(i * item->adeptPower->costArray[0]);
                }
                popup.setLevels(levels);
            }
        }

        // Execute the popup
        int result = popup.exec();
        if (result == QDialog::Rejected)
        {
            return;
        }

        // Gather data
        if (item->requiresCustom)
        {
            // Construct a new custom skill (this will do nothing if it already exists)
            customValue = popup.getCustomization();
            id = MAGIC_RULES->constructCustomizedSpell(id, customValue, popup.getCustomizationString());
        }
        if (needsLevel)
        {
            numAdds = popup.getLevel() + 1;
        }
    }

    // Take note of the choice
    if (!needsLevel)
    {
        if (!CHARACTER_CHOICES->addFreeSpell(id))
        {
            return;
        }
    }
    else
    {
        // If this was a levelled selection, add the spell the selected number of times
        CHARACTER_CHOICES->removeFreeSpell(id);
        for (int i = 0; i < numAdds; ++i)
        {
            bool result = CHARACTER_CHOICES->addFreeSpell(id);
            // If the first add does not work out, stop here
            if (i == 0 && !result)
            {
                return;
            }
        }
    }

    // Add the spell to the list (prevent double adding)
    QStringList& filterIDs = _spellsFilter->getFilterIDEquals();
    if (!filterIDs.contains(id))
    {
        filterIDs.push_back(id);
    }

    // Update the display
    _spellsFilter->applyFilter();
    ui->treeSpells->expandAll();

    // Disable the add button if we have no more free spells
    // If this is adding and we have no more free spells, disable
    if (CHARACTER_CHOICES->getAvailableFreeSpells() <= 0.0f &&
        CHARACTER_CHOICES->getAvailablePowerPoints() <= 0.0f)
    {
        ui->btnAddSpell->setEnabled(false);
    }

    // Update shown values
    updateValues();

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_btnRemoveSpell_clicked()
{
    // Get the currently selected spell
    MagicAbilityDefinition* item = static_cast<MagicAbilityDefinition*>(
                ui->treeSpells->currentIndex().data().value<void*>());

    // Remove the spell from the list
    QStringList& filterIDs = _spellsFilter->getFilterIDEquals();
    for (int i = 0; i < filterIDs.size(); ++i)
    {
        if (filterIDs[i] == item->id)
        {
            filterIDs.removeAt(i);
            break;
        }
    }

    // Take note of the choice
    CHARACTER_CHOICES->removeFreeSpell(item->id);

    // Update the display
    _spellsFilter->applyFilter();
    ui->treeSpells->expandAll();

    // Re-select the current skill from the available list to cause updating the add button
    QModelIndex currentIndex(ui->treeSpellsAvailable->currentIndex());
    ui->treeSpellsAvailable->setCurrentIndex(QModelIndex());
    ui->treeSpellsAvailable->setCurrentIndex(currentIndex);

    // As we have removed a spell, disable the remove button
    ui->btnRemoveSpell->setEnabled(false);

    // Update shown values
    updateValues();

    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::on_spinPurchasePP_valueChanged(int p_value)
{
    // Try to set the purchased power points to the new value
    CHARACTER_CHOICES->setPurchasePowerPoints(p_value);

    // Re-set the value of the spin box, as it might have changed within setPurchasePowerPoints
    ui->spinPurchasePP->blockSignals(true);
    ui->spinPurchasePP->setValue(CHARACTER_CHOICES->getPurchasedPowerPoints());
    ui->spinPurchasePP->blockSignals(false);

    // It is possible that this was a decrease that lead to the adept powers being reset
    // In that case, remove adept powers from the filter
    if (CHARACTER_CHOICES->getAvailablePowerPoints() == CHARACTER_CHOICES->getPowerPoints())
    {
        QStringList& filter = _spellsFilter->getFilterIDEquals();
        for (int i = 0; i < filter.size(); ++i)
        {
            const MagicAbilityDefinition& def = MAGIC_RULES->getDefinition(filter[i]);
            if (def.abilityType == MAGICABILITYTYPE_ADEPT_POWER)
            {
                filter.erase(filter.begin() + i);
                i--;
            }
        }
        _spellsFilter->applyFilter();
    }

    // Update display
    updateValues();
}

//---------------------------------------------------------------------------------
void
CharEditMagicTab::handleSkillsBoxChecked(bool p_checked)
{
    if (p_checked)
    {
        ui->groupBoxSkills->layout()->addWidget(ui->frameFreeSkills);
        ui->frameFreeSkills->setVisible(true);
    }
    else
    {
        ui->groupBoxSkills->layout()->removeWidget(ui->frameFreeSkills);
        ui->frameFreeSkills->setVisible(false);
    }
}

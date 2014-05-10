#include "chareditskilltab.h"
#include "ui_chareditskilltab.h"

#include <QKeyEvent>
#include <QTimer>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "data/dictionary.h"
#include "rules/rules.h"
#include "ui/models/skilltreemodel.h"
#include "ui/models/skilldelegate.h"
#include "ui/models/skillsortfilterproxymodel.h"
#include "ui/utils/priorityeventfilter.h"
#include "ui/character/popups/customdescriptorpopup.h"
#include "ui/character/popups/specializationdialog.h"

//---------------------------------------------------------------------------------
CharEditSkillTab::CharEditSkillTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditSkillTab)
    , _skillsModel(NULL)
    , _skillsFilter(NULL)
    , _skillsDelegate(NULL)
    , _filter(NULL)
    , _specDialog(NULL)
{
    ui->setupUi(this);

    _specDialog = new SpecializationDialog(this);
    connect(_specDialog, SIGNAL(specializationChanged()), SLOT(handleSkillValueChanged()));
}

//---------------------------------------------------------------------------------
CharEditSkillTab::~CharEditSkillTab()
{
    delete ui;

    delete _specDialog;
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::initialize()
{
    // Set priority event filter
    _filter = new PriorityEventFilter();
    ui->cbPriority->installEventFilter(_filter);
    connect(ui->cbPriority, SIGNAL(activated(int)), _filter ,SLOT(handlePrioritySelection(int)));

    // Initialize the skill view
    // Available skills
    _skillsModel = new SkillTreeModel(true);
    _skillsModel->setItemView(ui->treeSkills);
    _skillsModel->initialize();
    // Filter
    _skillsFilter = new SkillSortFilterProxyModel(ui->treeSkills);
    _skillsFilter->setSourceModel(_skillsModel);
    std::vector<SkillType> filterTypes;
    filterTypes.push_back(SKILL_TYPE_COMBAT);
    filterTypes.push_back(SKILL_TYPE_KNOWLEDGE);
    filterTypes.push_back(SKILL_TYPE_MAGIC);
    filterTypes.push_back(SKILL_TYPE_PHYSICAL);
    filterTypes.push_back(SKILL_TYPE_RESONANCE);
    filterTypes.push_back(SKILL_TYPE_SOCIAL);
    filterTypes.push_back(SKILL_TYPE_TECHNICAL);
    filterTypes.push_back(SKILL_TYPE_VEHICLE);
    _skillsFilter->setFilterTypes(filterTypes);
    _skillsFilter->setFilterMask(SKILL_FILTERMASK_TYPE);
    _skillsFilter->setShowEmptyCategories(true);
    _skillsFilter->setShowUserSkills(true);
    _skillsFilter->applyFilter();
    // Model & sorting
    ui->treeSkills->setModel(_skillsFilter);
    ui->treeSkills->setSortingEnabled(true);
    ui->treeSkills->setEditTriggers(QAbstractItemView::AllEditTriggers);
    // Delegate
    _skillsDelegate = new SkillDelegate();
    ui->treeSkills->setItemDelegate(_skillsDelegate);
    connect(_skillsDelegate, SIGNAL(skillChanged()), SLOT(handleSkillValueChanged()));
    connect(_skillsDelegate, SIGNAL(addButtonClicked(SkillDefinition*)),
                             SLOT(handleCustomSkillAdd(SkillDefinition*)));
    connect(_skillsDelegate, SIGNAL(specializationsClicked(SkillDefinition*)),
                             SLOT(manageSpecializations(SkillDefinition*)));
    // Handle selection & drag
    connect(ui->treeSkills->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                                SLOT(handleSkillChanged(QModelIndex,QModelIndex)));

    // Insert skill priorities
    ui->cbPriority->blockSignals(true);
    ui->cbPriority->addItem(QString("A (%1/%2)").arg(SKILL_RULES->getNumSkillPoints(0, false))
                                                .arg(SKILL_RULES->getNumSkillPoints(0, true)), 0);
    ui->cbPriority->addItem(QString("B (%1/%2)").arg(SKILL_RULES->getNumSkillPoints(1, false))
                                                .arg(SKILL_RULES->getNumSkillPoints(1, true)), 1);
    ui->cbPriority->addItem(QString("C (%1/%2)").arg(SKILL_RULES->getNumSkillPoints(2, false))
                                                .arg(SKILL_RULES->getNumSkillPoints(2, true)), 2);
    ui->cbPriority->addItem(QString("D (%1/%2)").arg(SKILL_RULES->getNumSkillPoints(3, false))
                                                .arg(SKILL_RULES->getNumSkillPoints(3, true)), 3);
    ui->cbPriority->addItem(QString("E (%1/%2)").arg(SKILL_RULES->getNumSkillPoints(4, false))
                                                .arg(SKILL_RULES->getNumSkillPoints(4, true)), 4);
    ui->cbPriority->addItem(QString(" "), -1);
    ui->cbPriority->setCurrentIndex(5);
    ui->cbPriority->blockSignals(false);

    // Apply translation
    applyTranslation();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::applyTranslation()
{

}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::showEvent(QShowEvent* /*unused*/)
{
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) != -1)
    {
        setEnabled(true);
    }
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_SKILLS) != -1)
    {
        ui->treeSkills->setEnabled(true);
    }

    updateValues();
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::resizeEvent(QResizeEvent* p_event)
{
    int totalWidth = p_event->size().width();
    ui->treeSkills->setColumnWidth(0, totalWidth * 0.35);
    ui->treeSkills->setColumnWidth(1, totalWidth * 0.15);
    ui->treeSkills->setColumnWidth(2, totalWidth * 0.1);
    ui->treeSkills->setColumnWidth(3, totalWidth * 0.125);
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::keyPressedEvent(QKeyEvent* p_keyEvent)
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
CharEditSkillTab::checkContinue()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_SKILLS) != -1 &&
            CHARACTER_CHOICES->getAvailableSkillPoints(false) == 0 &&
            CHARACTER_CHOICES->getAvailableSkillPoints(true) == 0)
        {
            ui->btnGuidedContinue->setEnabled(true);
            ui->btnGuidedContinue->setText(Dictionary::getTranslation("CONTINUE"));
        }
        else
        {
            ui->btnGuidedContinue->setEnabled(false);
            ui->btnGuidedContinue->setText(Dictionary::getTranslation("SKILL_CANT_CONTINUE"));
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::updateValues()
{
    // Karma points
    ui->lblKarmaValue->setText(QString("%1 / %2")
                                .arg(CHARACTER_CHOICES->getAvailableKarma())
                                .arg(CHARACTER_VALUES->getKarmaPool()));

    // Make sure we have a valid priority
    int priority = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_SKILLS);
    if (priority == -1)
    {
        return;
    }

    // Skill points
    ui->lblSkillPointsValue->setText(QString("%1 / %2")
                                .arg(CHARACTER_CHOICES->getAvailableSkillPoints(false))
                                .arg(SKILL_RULES->getNumSkillPoints(priority, false)));

    // Skill group points
    ui->lblSkillGroupPointsValue->setText(QString("%1 / %2")
                                .arg(CHARACTER_CHOICES->getAvailableSkillPoints(true))
                                .arg(SKILL_RULES->getNumSkillPoints(priority, true)));
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::storeViewState()
{
    _expandedSkills.clear();
    for(int i = 0; i < _skillsFilter->rowCount(); ++i)
    {
        QModelIndex index = _skillsFilter->index(i, 0);
        if (ui->treeSkills->isExpanded(index))
        {
            _expandedSkills << i;
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::restoreViewState()
{
    foreach (int index, _expandedSkills)
    {
        // Search `item` text in model
        QModelIndex item = _skillsFilter->index(index, 0);
        if (item.isValid())
        {
            // Information: with this code, expands ONLY first level in QTreeView
            ui->treeSkills->setExpanded(item, true);
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::handleSkillValueChanged()
{
    updateValues();
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::handleCustomSkillAdd(SkillDefinition* p_skill)
{
    // If this requires a custom choice, show the modal window
    QString customValue = "";

    // Show the popup and get its result
    CustomDescriptorPopup popup(NULL, p_skill->translations[APPSTATUS->getCurrentLocale()],
                                false, true);
    int result = popup.exec();
    if (result == QDialog::Rejected)
    {
        return;
    }
    customValue = popup.getCustomization();

    // Construct a new custom skill (this will do nothing if it already exists)
    SKILL_RULES->constructCustomizedSkill(p_skill->id, customValue);

    updateValues();
    checkContinue();

    // We cannot refresh the skill view now as this will cause a crash
    // because we are actually still within a delegate function. Weird shit.
    QTimer::singleShot(250, this, SLOT(forceViewUpdate()));
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::manageSpecializations(SkillDefinition* p_skill)
{
    // Show specializations management window
    _specDialog->setSkill(p_skill->id);
    _specDialog->show();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::forceViewUpdate()
{
    storeViewState();
    _skillsFilter->applyFilter();
    restoreViewState();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::on_cbPriority_currentIndexChanged(int p_index)
{
    int prio = ui->cbPriority->itemData(p_index).toInt();
    int oldPrio = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_SKILLS);

    // Set/Unset the chosen priority for the skills
    if (prio != -1)
    {
        CHARACTER_CHOICES->setPriority(prio, PRIORITY_SKILLS);
    }
    else
    {
        CHARACTER_CHOICES->unsetPriority(PRIORITY_SKILLS);
    }

    // If the old priority was higher than the new one, reset purchased skill points
    if (oldPrio != -1 && oldPrio < prio)
    {
        CHARACTER_CHOICES->resetSkillPoints();
        CHARACTER_CHOICES->resetSkillPurchases();
    }

    // It is possible that this choice deselected the metatype
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) == -1)
    {
        setEnabled(false);
        APPSTATUS->setStatusBarMessage(
                    tr("Deselected metatype. Go back and select your metatype!"), 5.0f, QColor(255, 0, 0));
        return;
    }

    // Enable or disable the attribute increase buttons
    ui->treeSkills->setEnabled(CHARACTER_CHOICES->getPriorityIndex(PRIORITY_SKILLS) != -1);

    // Update the displayed values
    updateValues();

    // Check if we can continue
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::handleSkillChanged(const QModelIndex& p_current, const QModelIndex& p_previous)
{
    QItemSelectionModel* model = static_cast<QItemSelectionModel*>(sender());

    // We somehow selected an invalid item
    if (!p_current.isValid())
    {
        ui->btnRemoveSkill->setEnabled(false);
        return;
    }

    // If the item is a category, we can't remove it
    SkillDefinition* item = static_cast<SkillDefinition*>(p_current.data().value<void*>());
    if (item->isCategory)
    {
        ui->btnRemoveSkill->setEnabled(false);
        return;
    }

    // If the item is the child of a group, we can't remove it
    if (item->parent && !item->parent->isCategory &&
        SKILL_RULES->getDefinition(item->parent->id).isGroup)
    {
        ui->btnRemoveSkill->setEnabled(false);
        return;
    }

    // It is a removable item, so enable the button
    ui->btnRemoveSkill->setEnabled(true);
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::on_btnRemoveSkill_clicked()
{

}

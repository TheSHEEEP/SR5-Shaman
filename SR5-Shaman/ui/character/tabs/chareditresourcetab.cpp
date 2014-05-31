#include "chareditresourcetab.h"
#include "ui_chareditresourcetab.h"

#include <QKeyEvent>
#include <QTimer>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "data/dictionary.h"
#include "rules/rules.h"
#include "ui/models/resourcetreemodel.h"
#include "ui/models/resourcedelegate.h"
#include "ui/models/resourcesortfilterproxymodel.h"
#include "ui/utils/priorityeventfilter.h"
#include "ui/character/popups/customdescriptorpopup.h"

//---------------------------------------------------------------------------------
CharEditResourceTab::CharEditResourceTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditResourceTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditResourceTab::~CharEditResourceTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::initialize()
{
    // Set priority event filter
    _filter = new PriorityEventFilter();
    ui->cbPriority->installEventFilter(_filter);
    connect(ui->cbPriority, SIGNAL(activated(int)), _filter ,SLOT(handlePrioritySelection(int)));

    // Initialize the skill view
    // Available skills
    _resourceModel = new ResourceTreeModel(true);
    _resourceModel->setItemView(ui->treeResources);
    _resourceModel->initialize();
    // Filter
    _resourceFilter = new ResourceSortFilterProxyModel(ui->treeResources);
    _resourceFilter->setSourceModel(_resourceModel);
    /*std::vector<SkillType> filterTypes;
    filterTypes.push_back(SKILL_TYPE_COMBAT);
    filterTypes.push_back(SKILL_TYPE_KNOWLEDGE);
    filterTypes.push_back(SKILL_TYPE_LANGUAGE);
    filterTypes.push_back(SKILL_TYPE_MAGIC);
    filterTypes.push_back(SKILL_TYPE_PHYSICAL);
    filterTypes.push_back(SKILL_TYPE_RESONANCE);
    filterTypes.push_back(SKILL_TYPE_SOCIAL);
    filterTypes.push_back(SKILL_TYPE_TECHNICAL);
    filterTypes.push_back(SKILL_TYPE_VEHICLE);
    _skillsFilter->setFilterTypes(filterTypes);
    _skillsFilter->setFilterMask(SKILL_FILTERMASK_TYPE);
    _skillsFilter->setShowEmptyCategories(true);
    _skillsFilter->setShowUserSkills(true);*/
    _resourceFilter->applyFilter();
    // Model & sorting
    ui->treeResources->setModel(_resourceFilter);
    ui->treeResources->setSortingEnabled(true);
    ui->treeResources->setEditTriggers(QAbstractItemView::AllEditTriggers);
    // Delegate
    _resourceDelegate = new ResourceDelegate();
    ui->treeResources->setItemDelegate(_resourceDelegate);
    /*connect(_resourceDelegate, SIGNAL(skillChanged()), SLOT(handleSkillValueChanged()));
    connect(_resourceDelegate, SIGNAL(addButtonClicked(SkillDefinition*)),
                               SLOT(handleCustomSkillAdd(SkillDefinition*)));
    connect(_resourceDelegate, SIGNAL(specializationsClicked(SkillDefinition*)),
                               SLOT(manageSpecializations(SkillDefinition*)));*/
    // Handle selection & drag
    connect(ui->treeResources->selectionModel(),  SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                                  SLOT(handleResourceChanged(QModelIndex,QModelIndex)));

    // Insert skill priorities
    ui->cbPriority->blockSignals(true);
    /*ui->cbPriority->addItem(QString("A (%1)").arg(RESOURCE_RULES->getNumSkillPoints(0, false));
    ui->cbPriority->addItem(QString("B (%1)").arg(RESOURCE_RULES->getNumSkillPoints(1, false));
    ui->cbPriority->addItem(QString("C (%1)").arg(RESOURCE_RULES->getNumSkillPoints(2, false));
    ui->cbPriority->addItem(QString("D (%1)").arg(RESOURCE_RULES->getNumSkillPoints(3, false));
    ui->cbPriority->addItem(QString("E (%1)").arg(RESOURCE_RULES->getNumSkillPoints(4, false));*/
    ui->cbPriority->addItem(QString(" "), -1);
    ui->cbPriority->setCurrentIndex(5);
    ui->cbPriority->blockSignals(false);

    // Apply translation
    applyTranslation();
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::applyTranslation()
{
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) != -1)
    {
        setEnabled(true);
    }
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_RESOURCES) != -1)
    {
        ui->treeResources->setEnabled(true);
    }

    updateValues();
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::showEvent(QShowEvent* /*unused*/)
{
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) != -1)
    {
        setEnabled(true);
    }
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_RESOURCES) != -1)
    {
        ui->treeResources->setEnabled(true);
    }

    updateValues();
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::resizeEvent(QResizeEvent* p_event)
{
    int totalWidth = p_event->size().width();
    ui->treeResources->setColumnWidth(0, totalWidth * 0.35);
    ui->treeResources->setColumnWidth(1, totalWidth * 0.15);
    ui->treeResources->setColumnWidth(2, totalWidth * 0.1);
    ui->treeResources->setColumnWidth(3, totalWidth * 0.125);
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::keyPressedEvent(QKeyEvent* p_keyEvent)
{
    switch (p_keyEvent->key())
    {
    case Qt::Key_NumberSign:
    case Qt::Key_Apostrophe:
        // Fast-forward mode for testing
        if (p_keyEvent->modifiers() & Qt::ShiftModifier)
        {
            emit guidedNextStep();
        }
        break;

    default:
        break;
    }
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::checkContinue()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_RESOURCES) != -1)
        {
            ui->btnGuidedContinue->setEnabled(true);
            ui->btnGuidedContinue->setText(Dictionary::getTranslation("CONTINUE"));
        }
        else
        {
            ui->btnGuidedContinue->setEnabled(false);
            ui->btnGuidedContinue->setText(Dictionary::getTranslation("RESOURCE_CANT_CONTINUE"));
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::updateValues()
{
    // Karma points
    ui->lblKarmaValue->setText(QString("%1 / %2")
                                .arg(CHARACTER_CHOICES->getAvailableKarma())
                                .arg(CHARACTER_VALUES->getKarmaPool()));

    // Make sure we have a valid priority
    int priority = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_RESOURCES);
    if (priority == -1)
    {
        return;
    }
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::storeViewState()
{
    _expandedResources.clear();
    for(int i = 0; i < _resourceFilter->rowCount(); ++i)
    {
        QModelIndex index = _resourceFilter->index(i, 0);
        if (ui->treeResources->isExpanded(index))
        {
            _expandedResources << i;
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::restoreViewState()
{
    foreach (int index, _expandedResources)
    {
        // Search `item` text in model
        QModelIndex item = _resourceFilter->index(index, 0);
        if (item.isValid())
        {
            // Information: with this code, expands ONLY first level in QTreeView
            ui->treeResources->setExpanded(item, true);
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::forceViewUpdate()
{
    storeViewState();
    _resourceFilter->applyFilter();
    restoreViewState();
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::on_cbPriority_currentIndexChanged(int p_index)
{

}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::handleResourceChanged(const QModelIndex& p_current, const QModelIndex& p_previous)
{
    // We somehow selected an invalid item
    if (!p_current.isValid())
    {
        ui->btnRemoveItem->setEnabled(false);
        return;
    }

    // If the item is a category, we can't remove it
    ResourceDefinition* item = static_cast<ResourceDefinition*>(p_current.data().value<void*>());
    if (item->isCategory)
    {
        ui->btnRemoveItem->setEnabled(false);
        return;
    }

    // It is a removable item, so enable the button
    ui->btnRemoveItem->setEnabled(true);
}

//---------------------------------------------------------------------------------
void
CharEditResourceTab::on_spinBuyNuyen_valueChanged(int arg1)
{

}

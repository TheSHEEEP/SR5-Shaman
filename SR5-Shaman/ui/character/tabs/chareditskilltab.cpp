#include "chareditskilltab.h"
#include "ui_chareditskilltab.h"

#include <QKeyEvent>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "rules/rules.h"
#include "ui/models/skilltreemodel.h"
#include "ui/models/skilldelegate.h"
#include "ui/models/skillsortfilterproxymodel.h"

//---------------------------------------------------------------------------------
CharEditSkillTab::CharEditSkillTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditSkillTab)
    , _skillsFilter(NULL)
    , _skillsDelegate(NULL)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditSkillTab::~CharEditSkillTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::initialize()
{
    // Initialize the skill view
    // Available skills
    SkillTreeModel* skillTreeModel = new SkillTreeModel();
    skillTreeModel->initialize();
    // Filter
    _skillsFilter = new SkillSortFilterProxyModel(ui->treeSkills);
    _skillsFilter->setSourceModel(skillTreeModel);
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
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditSkillTab::keyPressEvent(QKeyEvent* p_keyEvent)
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

    }
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

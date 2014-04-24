#include "chareditqualitytab.h"
#include "ui_chareditqualitytab.h"

#include <QItemSelectionModel>
#include <QPushButton>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "rules/misc/customchoice.h"
#include "ui/models/qualitydelegate.h"
#include "ui/models/qualitysortfilterproxymodel.h"
#include "ui/models/qualitytreemodel.h"
#include "ui/character/popups/customdescriptorpopup.h"
#include "rules/effects/condition.h"
#include "rules/effects/effect.h"

//---------------------------------------------------------------------------------
CharEditQualityTab::CharEditQualityTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharEditQualityTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditQualityTab::~CharEditQualityTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::initialize()
{
    // Initialize the quality views
    // Available qualities
    QualityTreeModel* qualityTreeModel = new QualityTreeModel();
    qualityTreeModel->initialize();
    // Filter
    _qualitiesAvailableFilter = new QualitySortFilterProxyModel(ui->treeQualitiesAvailable);
    _qualitiesAvailableFilter->setSourceModel(qualityTreeModel);
    _qualitiesAvailableFilter->setShowEmptyCategories(false);
    _qualitiesAvailableFilter->applyFilter();
    // Model & sorting
    ui->treeQualitiesAvailable->setModel(_qualitiesAvailableFilter);
    ui->treeQualitiesAvailable->setSortingEnabled(true);
    // Delegate
    _qualitiesAvailableDelegate = new QualityDelegate();
    ui->treeQualitiesAvailable->setItemDelegate(_qualitiesAvailableDelegate);
    ui->treeQualitiesAvailable->setHeaderHidden(true);

    // Selected qualities
    qualityTreeModel = new QualityTreeModel();
    qualityTreeModel->initialize();
    // Filter
    _qualitiesFilter = new QualitySortFilterProxyModel(ui->treeQualities);
    _qualitiesFilter->setSourceModel(qualityTreeModel);
    _qualitiesFilter->getFilterIDContains().clear();
    _qualitiesFilter->getFilterIDEquals().clear();
    _qualitiesFilter->setFilterMask(QUALITY_FILTERMASK_ID_EQUALS);
    _qualitiesFilter->setShowEmptyCategories(false);
    _qualitiesFilter->applyFilter();
    // Model & sorting
    ui->treeQualities->setModel(_qualitiesFilter);
    ui->treeQualities->setSortingEnabled(true);
    // Delegate
    _qualitiesDelegate = new QualityDelegate();
    _qualitiesDelegate->setShowQualityLevel(true);
    ui->treeQualities->setItemDelegate(_qualitiesDelegate);
    ui->treeQualities->setHeaderHidden(true);

    // Handle selection & drag
    connect(ui->treeQualitiesAvailable->selectionModel(),   SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                                            SLOT(handleQualityChanged(QModelIndex,QModelIndex)));
    connect(ui->treeQualities->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                                SLOT(handleQualityChanged(QModelIndex,QModelIndex)));

    // Apply translation
    applyTranslation();
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::applyTranslation()
{

}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::showEvent(QShowEvent* /*unused*/)
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        ui->btnGuidedContinue->show();
    } // END if guided creation
    else
    {
        ui->btnGuidedContinue->hide();
    }

    // Validate all chosen qualities, maybe some conditions are not
    // fulfilled any more
    // TODO: There should be a better place to do this
    bool allSame = CHARACTER_CHOICES->validateQualities();

    // Refresh selected qualities to reflect current status
    // As it is possible that some qualities were invalidated
    if (!allSame)
    {
        _qualitiesFilter->getFilterIDEquals().clear();
        const QMap<QString, int>& qualities = CHARACTER_CHOICES->getQualities();
        for (int i = 0; i < qualities.keys().size(); ++i)
        {
            _qualitiesFilter->getFilterIDEquals().push_back(qualities.keys().at(i));
        }
        _qualitiesFilter->applyFilter();
    }

    // Update values
    updateValues();
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::handleQualityChanged(const QModelIndex& p_current, const QModelIndex& p_previous)
{
    QItemSelectionModel* model = static_cast<QItemSelectionModel*>(sender());
    bool isSelectedQuality = model == ui->treeQualities->selectionModel();
    QPushButton* button = isSelectedQuality ? ui->btnRemoveQuality : ui->btnAddQuality;

    // We somehow selected an invalid item
    if (!p_current.isValid())
    {
        button->setEnabled(false);
        return;
    }

    // If the quality is already chosen, and not levelled, can't add it again
    QualityDefinition* def = static_cast<QualityDefinition*>(p_current.data().value<void*>());
    if (!isSelectedQuality)
    {
        if (def->costType == COSTTYPE_NORMAL &&
            _qualitiesFilter->getFilterIDEquals().contains(def->id))
        {
            button->setEnabled(false);
        }
        else
        {
            button->setEnabled(true);
        }
    }
    // Selected qualities can always be removed
    else
    {
        button->setEnabled(true);
    }
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::on_btnAddQuality_clicked()
{
    // Get the currently selected skill
    QualityDefinition* item = static_cast<QualityDefinition*>(
                ui->treeQualitiesAvailable->currentIndex().data().value<void*>());
    QString id = item->id;

    // If this requires a custom choice, show the modal window
    QString customValue = "";
    bool needsLevel = (item->costType == COSTTYPE_ARRAY ||
                       item->costType == COSTTYPE_PER_LEVEL);
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
            if (item->costType == COSTTYPE_ARRAY)
            {
                popup.setLevels(item->costArray);
            }
            // The maximum level must be calculated here
            else if (item->costType == COSTTYPE_PER_LEVEL)
            {
                std::vector<float> levels;
                int maxLevel = 0;
                // If the quality has a max_level condition, use that
                for (unsigned int i = 0; i < item->effects.size(); ++i)
                {
                    bool found = false;
                    for (unsigned int j = 0; j < item->effects[i]->getConditions().size(); ++j)
                    {
                        if (item->effects[i]->getConditions()[j]->getType() == CONDITIONTYPE_MAX_LEVEL ||
                            item->effects[i]->getConditions()[j]->getType() == CONDITIONTYPE_MAX_LEVEL_WITH_CUSTOMS)
                        {
                            maxLevel = item->effects[i]->getConditions()[j]->getValues()[0].toInt();
                            found = true;
                            break;
                        }
                    }

                    if (found) break;
                }
                for (int i = 1; i <= maxLevel; ++i)
                {
                    levels.push_back(i * item->costArray[0]);
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
            id = QUALITY_RULES->constructCustomizedQuality(id, customValue, popup.getCustomizationString());
        }
        if (needsLevel)
        {
            numAdds = popup.getLevel() + 1;
        }
    }

    // Take note of the choice
    if (!needsLevel)
    {
        if (!CHARACTER_CHOICES->addQuality(id))
        {
            return;
        }
    }
    else
    {
        // If this was a levelled selection, add the spell the selected number of times
        CHARACTER_CHOICES->removeQuality(id);
        for (int i = 0; i < numAdds; ++i)
        {
            bool result = CHARACTER_CHOICES->addQuality(id);
            // If the first add does not work out, stop here
            if (i == 0 && !result)
            {
                return;
            }
        }
    }

    // Add the spell to the list (prevent double adding)
    QStringList& filterIDs = _qualitiesFilter->getFilterIDEquals();
    if (!filterIDs.contains(id))
    {
        filterIDs.push_back(id);
    }

    // Update the display
    _qualitiesFilter->applyFilter();
    ui->treeQualities->expandAll();

    // Update shown values
    updateValues();
}


//---------------------------------------------------------------------------------
void
CharEditQualityTab::on_btnRemoveQuality_clicked()
{
    // Get the currently selected spell
    QualityDefinition* item = static_cast<QualityDefinition*>(
                ui->treeQualities->currentIndex().data().value<void*>());

    // Remove the spell from the list
    QStringList& filterIDs = _qualitiesFilter->getFilterIDEquals();
    for (int i = 0; i < filterIDs.size(); ++i)
    {
        if (filterIDs[i] == item->id)
        {
            filterIDs.removeAt(i);
            break;
        }
    }

    // Take note of the choice
    CHARACTER_CHOICES->removeQuality(item->id);

    // Update the display
    _qualitiesFilter->applyFilter();
    ui->treeQualities->expandAll();

    // Re-select the current skill from the available list to cause updating the add button
    QModelIndex currentIndex(ui->treeQualitiesAvailable->currentIndex());
    ui->treeQualitiesAvailable->setCurrentIndex(QModelIndex());
    ui->treeQualitiesAvailable->setCurrentIndex(currentIndex);

    // As we have removed a spell, disable the remove button
    ui->btnRemoveQuality->setEnabled(false);

    // Update shown values
    updateValues();
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::on_btnGuidedContinue_clicked()
{
    emit guidedNextStep();
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::updateValues()
{
    if (!isEnabled())
    {
        return;
    }

    // Spent on positive
    int positiveKarma = CHARACTER_CHOICES->getQualitiesKarma(true);
    ui->lblPosQualValue->setText(QString("%1")
                                 .arg(positiveKarma));

    // Gained from negative
    int negativeKarma = CHARACTER_CHOICES->getQualitiesKarma(false);
    ui->lblNegQualValue->setText(QString("%1")
                                 .arg(negativeKarma));

    // Quality karma balance
    ui->lblKarmaBalanceValue->setText(QString("%1")
                                      .arg(negativeKarma - positiveKarma));

    // Karma points
    ui->lblKarmaValue->setText(QString("%1 / %2")
                                .arg(CHARACTER_CHOICES->getAvailableKarma())
                                .arg(CHARACTER_VALUES->getKarmaPool()));
}

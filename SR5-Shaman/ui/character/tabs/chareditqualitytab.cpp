#include "chareditqualitytab.h"
#include "ui_chareditqualitytab.h"

#include <QItemSelectionModel>
#include <QPushButton>

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
    // TODO: here
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
CharEditQualityTab::handleQualityChanged(const QModelIndex& p_current, const QModelIndex& p_previous)
{
    QItemSelectionModel* model = static_cast<QItemSelectionModel*>(sender());
    bool isQualityView = model == ui->treeQualities->selectionModel();
    QPushButton* button = isQualityView ? ui->btnRemoveQuality : ui->btnAddQuality;
}

//---------------------------------------------------------------------------------
void
CharEditQualityTab::on_btnAddQuality_clicked()
{

}


//---------------------------------------------------------------------------------
void
CharEditQualityTab::on_btnRemoveQuality_clicked()
{

}

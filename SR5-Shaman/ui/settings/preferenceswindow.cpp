#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

#include <QStandardItem>

#include "applayoutsettings.h"

//---------------------------------------------------------------------------------
PreferencesWindow::PreferencesWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PreferencesWindow)
    , _treeModel(0)
    , _currentSettings(0)
    , _layoutSettings(0)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
PreferencesWindow::~PreferencesWindow()
{
    delete ui;

    if (_treeModel)
    {
        delete _treeModel;
    }
    if (_layoutSettings)
    {
        delete _layoutSettings;
    }
}

//---------------------------------------------------------------------------------
void
PreferencesWindow::initialize()
{
    // Create settings widgets
    _layoutSettings = new AppLayoutSettings(0);
    ((AppLayoutSettings*)_layoutSettings)->initialize();
    _currentSettings = _layoutSettings;
    ui->currentSettingsContainer->addWidget(_currentSettings);

    // Define tree model
    _treeModel = new QStandardItemModel();
    QStandardItem* rootNode = _treeModel->invisibleRootItem();

    // Define tree items
    QStandardItem* layoutItem = new QStandardItem("Layout");

    // Add items
    rootNode->appendRow(layoutItem);

    // Register the model
    ui->settingsView->setModel(_treeModel);
    ui->settingsView->expandAll();

    // Selection changes shall trigger a slot
    QItemSelectionModel* selectionModel= ui->settingsView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
}

//---------------------------------------------------------------------------------
void
PreferencesWindow::selectionChangedSlot(const QItemSelection& p_newSelection, const QItemSelection & p_oldSelection)
{
    // Get the text of the selected item
    const QModelIndex index = ui->settingsView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    // Get the according widget
    QWidget* newSettings = 0;
    if (selectedText == "Layout")
    {
        newSettings = _layoutSettings;
    }

    // Switch to new layout
    if (_currentSettings != newSettings)
    {
        ui->currentSettingsContainer->removeWidget(_currentSettings);
        ui->currentSettingsContainer->addWidget(newSettings);
        _currentSettings = newSettings;
    }
}

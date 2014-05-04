#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdesktopwidget.h>

#include <QLabel>
#include <QTimer>

#include "globaleventfilter.h"
#include "ui/settings/preferenceswindow.h"
#include "ui/character/creationinitializingwindow.h"
#include "data/appstatus.h"

//---------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _prefWindow(0)
    , _creationInitWindow(0)
    , _targetWidth(0), _targetHeight(0)
    , _initMaximizer(false)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;

    if (_prefWindow)
    {
        delete _prefWindow;
    }
    if (_creationInitWindow)
    {
        delete _creationInitWindow;
    }
}

//---------------------------------------------------------------------------------
void
MainWindow::initialize(GlobalEventFilter* p_globalFilter)
{
    // Create setting windows
    _prefWindow = new PreferencesWindow(this);
    _prefWindow->initialize();

    // Create character related windows
    _creationInitWindow = new CreationInitializingWindow(this);
    _creationInitWindow->initialize();
    connect(_creationInitWindow, SIGNAL(startCreation()), SLOT(startCharacterCreation()));
    connect(_creationInitWindow, SIGNAL(cancelCreation()), SLOT(cancelCharacterCreation()));

    // Init children
    ui->mainSplitView->initialize();
    p_globalFilter->setMainSplitView(ui->mainSplitView);

    // Show init message while initializing
    ui->statusBar->showMessage(tr("Initializing..."), 2000);

    // Set global status bar
    APPSTATUS->setStatusBar(ui->statusBar);

    // Make sure the window fits the screen
    _targetWidth = QApplication::desktop()->width();
    _targetHeight = QApplication::desktop()->height();
    showMaximized();
    _initMaximizer = true;
}

//---------------------------------------------------------------------------------
void
MainWindow::resizeEvent(QResizeEvent* p_event)
{
    if (_initMaximizer)
    {
        // This has to be done to prevent the mainSplitView from kicking out the status bar
        ui->mainSplitView->hide();
        ui->mainSplitView->setFixedHeight(height() * 0.9f);
        ui->mainSplitView->show();
    }
}

//---------------------------------------------------------------------------------
void
MainWindow::startCharacterCreation()
{
    // Hide the window
    _creationInitWindow->hide();

    // Tell the main view to initialize the chosen creation
    ui->mainSplitView->initializeCharacterCreation();
}

//---------------------------------------------------------------------------------
void
MainWindow::cancelCharacterCreation()
{
    // Hide the window
    _creationInitWindow->hide();
}

//---------------------------------------------------------------------------------
void
MainWindow::on_actionPreferences_triggered()
{
    // Open the settings dialog
    _prefWindow->show();
}

//---------------------------------------------------------------------------------
void
MainWindow::on_actionNew_Character_triggered()
{
    // Open creation init dialog
    _creationInitWindow->show();
}

//---------------------------------------------------------------------------------
void
MainWindow::on_actionSwitch_Preview_Position_triggered()
{
    ui->mainSplitView->switchPreviewOrder();
}

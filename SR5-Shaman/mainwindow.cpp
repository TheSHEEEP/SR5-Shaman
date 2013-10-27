#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdesktopwidget.h>

#include "ui/settings/preferenceswindow.h"
#include "ui/character/creationinitializingwindow.h"

//---------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _prefWindow(0)
    , _creationInitWindow(0)
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
MainWindow::initialize()
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

    // Show init message while initializing
    statusBar()->showMessage(tr("Initializing..."), 2000);

    // Make sure the window fits the screen
    int desktopWidth = QApplication::desktop()->width();
    int desktopHeight = QApplication::desktop()->height();
    int targetWidth = 1280;
    int targetHeight = 800;
    float step = 0.9f;
    while (desktopWidth < targetWidth && desktopHeight < targetHeight)
    {
        targetWidth = (targetWidth * step) + 0.5f;
        targetHeight = (targetHeight * step) + 0.5f;
    }
    setGeometry((desktopWidth >> 1) - (targetWidth >> 1), (desktopHeight >> 1) - (targetHeight >> 1),
                targetWidth, targetHeight);
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

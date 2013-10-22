#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdesktopwidget.h>

#include "ui/settings/preferenceswindow.h"

//---------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _prefWindow(0)
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
}

//---------------------------------------------------------------------------------
void
MainWindow::initialize()
{
    // Create setting windows
    _prefWindow = new PreferencesWindow(this);
    _prefWindow->initialize();

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
MainWindow::on_actionPreferences_triggered()
{
    // Open the settings dialog
    _prefWindow->show();
}

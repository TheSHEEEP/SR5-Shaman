#include "creationinitializingwindow.h"
#include "ui_creationinitializingwindow.h"

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"

//---------------------------------------------------------------------------------
CreationInitializingWindow::CreationInitializingWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreationInitializingWindow)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CreationInitializingWindow::~CreationInitializingWindow()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CreationInitializingWindow::initialize()
{

}

//---------------------------------------------------------------------------------
void
CreationInitializingWindow::on_btnAccept_clicked()
{
    APPSTATUS->setState(APPSTATE_GUIDED_CREATION);

    emit startCreation();
}

//---------------------------------------------------------------------------------
void
CreationInitializingWindow::on_btnCancel_clicked()
{
    emit cancelCreation();
}

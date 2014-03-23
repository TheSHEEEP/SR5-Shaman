#include "creationinitializingwindow.h"
#include "ui_creationinitializingwindow.h"

#include "data/appstatus.h"
#include "data/dictionary.h"
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
    applyTranslation();
}

//---------------------------------------------------------------------------------
void
CreationInitializingWindow::applyTranslation()
{
    ui->cbQualityMax->setText(Dictionary::getTranslation("CB_QUALITYMAX_TEXT"));
    ui->cbQualityMax->setToolTip(Dictionary::getTranslation("CB_QUALITYMAX_TOOLTIP"));
}

//---------------------------------------------------------------------------------
void
CreationInitializingWindow::on_btnAccept_clicked()
{
    APPSTATUS->setState(APPSTATE_GUIDED_CREATION);

    // Note selected house rules
    CHARACTER_CHOICES->getHouseRules().qualityLimitSumCounts = ui->cbQualityMax->isChecked();

    emit startCreation();
}

//---------------------------------------------------------------------------------
void
CreationInitializingWindow::on_btnCancel_clicked()
{
    emit cancelCreation();
}

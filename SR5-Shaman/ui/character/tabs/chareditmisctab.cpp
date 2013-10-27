#include "chareditmisctab.h"
#include "ui_chareditmisctab.h"

//---------------------------------------------------------------------------------
CharEditMiscTab::CharEditMiscTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharEditMiscTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditMiscTab::~CharEditMiscTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditMiscTab::initialize()
{

}

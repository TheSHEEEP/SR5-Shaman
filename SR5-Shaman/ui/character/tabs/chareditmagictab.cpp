#include "chareditmagictab.h"
#include "ui_chareditmagictab.h"

//---------------------------------------------------------------------------------
CharEditMagicTab::CharEditMagicTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditMagicTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditMagicTab::~CharEditMagicTab()
{
    delete ui;
}

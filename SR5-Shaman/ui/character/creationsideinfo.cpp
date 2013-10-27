#include "creationsideinfo.h"
#include "ui_creationsideinfo.h"

//---------------------------------------------------------------------------------
CreationSideInfo::CreationSideInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreationSideInfo)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CreationSideInfo::~CreationSideInfo()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CreationSideInfo::initialize()
{

}

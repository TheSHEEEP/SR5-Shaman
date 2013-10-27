#include "characterpreviewsideinfo.h"
#include "ui_characterpreview.h"

//---------------------------------------------------------------------------------
CharacterPreviewSideInfo::CharacterPreviewSideInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharacterPreviewSideInfo)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharacterPreviewSideInfo::~CharacterPreviewSideInfo()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void CharacterPreviewSideInfo::initialize()
{

}


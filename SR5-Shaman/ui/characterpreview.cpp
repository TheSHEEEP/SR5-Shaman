#include "characterpreview.h"
#include "ui_characterpreview.h"

CharacterPreview::CharacterPreview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharacterPreview)
{
    ui->setupUi(this);
}

CharacterPreview::~CharacterPreview()
{
    delete ui;
}

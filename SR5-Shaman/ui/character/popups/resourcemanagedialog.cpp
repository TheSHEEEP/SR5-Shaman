#include "resourcemanagedialog.h"
#include "ui_resourcemanagedialog.h"

#include "rules/rules.h"
#include "data/appstatus.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"

//---------------------------------------------------------------------------------
ResourceManageDialog::ResourceManageDialog(QWidget *p_parent, const QString& p_id)
    : QDialog(NULL)
    , ui(new Ui::ResourceManageDialog)
{
    ui->setupUi(this);

    applyTranslation();
}

//---------------------------------------------------------------------------------
ResourceManageDialog::~ResourceManageDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
ResourceManageDialog::applyTranslation()
{

}

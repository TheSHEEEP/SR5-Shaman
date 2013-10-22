#include "currentsettingscontainer.h"
#include "ui_currentsettingscontainer.h"

//---------------------------------------------------------------------------------
CurrentSettingsContainer::CurrentSettingsContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentSettingsContainer)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CurrentSettingsContainer::~CurrentSettingsContainer()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CurrentSettingsContainer::addWidget(QWidget* p_widget)
{
    ui->gridLayout->addWidget(p_widget);
}

//---------------------------------------------------------------------------------
void
CurrentSettingsContainer::removeWidget(QWidget* p_widget)
{
    ui->gridLayout->removeWidget(p_widget);
}

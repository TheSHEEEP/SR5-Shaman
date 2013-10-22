#include "mainsplitview.h"
#include "ui_mainsplitview.h"

MainSplitView::MainSplitView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSplitView)
{
    ui->setupUi(this);
}

MainSplitView::~MainSplitView()
{
    delete ui;
}

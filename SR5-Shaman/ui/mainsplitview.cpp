#include "mainsplitview.h"
#include "ui_mainsplitview.h"

#include <QSettings>

#include "data/appstatus.h"
#include "ui/character/characterpreviewsideinfo.h"
#include "ui/character/creationsideinfo.h"
#include "ui/character/tabs/chareditmisctab.h"

//---------------------------------------------------------------------------------
MainSplitView::MainSplitView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainSplitView)
    , _currentSideInfo(0)
    , _charPreviewSideInfo(0)
    , _creationSideInfo(0)
    , _previewLeft(true)
    , _tabCharEditMisc(0)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
MainSplitView::~MainSplitView()
{
    // This has to be done here to prevent crashing on exit
    // TODO: Find out why exactly
    ui->horizontalLayout->removeWidget(_currentSideInfo);
    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    ui->horizontalLayout->removeWidget(ui->charTabs);

    delete ui;
}

//---------------------------------------------------------------------------------
void
MainSplitView::initialize()
{
    // Init character preview
    _charPreviewSideInfo = new CharacterPreviewSideInfo();
    _charPreviewSideInfo->initialize();
    _currentSideInfo = _charPreviewSideInfo;

    // Init creation side preview
    _creationSideInfo = new CreationSideInfo();
    _creationSideInfo->initialize();

    // Read correct split setup from settings
    QSettings settings;
    _previewLeft = settings.value("previewPositionLeft", true).toBool();
    applyOrder();

    // Initialize main tabs
    _tabCharEditMisc = new CharEditMiscTab();
    _tabCharEditMisc->initialize();
    connect(_tabCharEditMisc, SIGNAL(guidedNextStep()), SLOT(handleGuidedNext()));

    // TODO: here
}

//---------------------------------------------------------------------------------
void
MainSplitView::switchPreviewOrder()
{
    ui->horizontalLayout->removeWidget(_currentSideInfo);
    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    ui->horizontalLayout->removeWidget(ui->charTabs);

    _previewLeft = !_previewLeft;
    QSettings settings;
    settings.setValue("previewPositionLeft", _previewLeft);
    applyOrder();
}

//---------------------------------------------------------------------------------
void
MainSplitView::initializeCharacterCreation()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        // Remove old widgets
        ui->horizontalLayout->removeWidget(_currentSideInfo);
        _currentSideInfo->hide();
        ui->horizontalLayout->removeItem(ui->horizontalSpacer);
        ui->horizontalLayout->removeWidget(ui->charTabs);

        // Insert correct side info
        _currentSideInfo = (QWidget*)_creationSideInfo;
        applyOrder();

        // Remove current tabs
        while (ui->charTabs->count() > 0)
        {
            ui->charTabs->removeTab(0);
        }

        // Insert widgets into tab container
        // Step 1: Metatype & misc
        ui->charTabs->addTab(_tabCharEditMisc, tr("Step 1: Priorities"));
        // Step 2:
        ui->charTabs->setCurrentIndex(0);
    }
}

//---------------------------------------------------------------------------------
void
MainSplitView::applyOrder()
{
    if (_previewLeft)
    {
        ui->horizontalLayout->addWidget(_currentSideInfo);
        ui->horizontalLayout->addItem(ui->horizontalSpacer);
        ui->horizontalLayout->addWidget(ui->charTabs);
    }
    else
    {
        ui->horizontalLayout->addWidget(ui->charTabs);
        ui->horizontalLayout->addItem(ui->horizontalSpacer);
        ui->horizontalLayout->addWidget(_currentSideInfo);
    }
}

//---------------------------------------------------------------------------------
void
MainSplitView::handleGuidedNext()
{
    if (ui->charTabs->currentIndex() < (ui->charTabs->count() - 1))
    {
        ui->charTabs->setCurrentIndex(ui->charTabs->currentIndex() + 1);
    }
    else
    {
        ui->charTabs->setCurrentIndex(0);
    }
}

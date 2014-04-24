#include "mainsplitview.h"
#include "ui_mainsplitview.h"

#include <QSettings>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "data/dictionary.h"
#include "ui/character/characterpreviewsideinfo.h"
#include "ui/character/creationsideinfo.h"
#include "ui/character/tabs/chareditmisctab.h"
#include "ui/character/tabs/chareditattributetab.h"
#include "ui/character/tabs/chareditmagictab.h"
#include "ui/character/tabs/chareditqualitytab.h"

//---------------------------------------------------------------------------------
MainSplitView::MainSplitView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainSplitView)
    , _previewLeft(true)
    , _currentSideInfo(0)
    , _charPreviewSideInfo(0)
    , _creationSideInfo(0)
    , _tabCharEditMisc(0)
    , _tabCharEditAttribute(0)
    , _tabCharEditMagic(0)
    , _tabCharEditQuality(0)
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

    if (_charPreviewSideInfo)
    {
        delete _charPreviewSideInfo;
        delete _creationSideInfo;
        delete _tabCharEditMisc;
        delete _tabCharEditAttribute;
        delete _tabCharEditMagic;
        delete _tabCharEditQuality;
    }
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
    connect(_tabCharEditMisc, SIGNAL(disableNext()), SLOT(handleDisableNext()));

    _tabCharEditAttribute = new CharEditAttributeTab();
    _tabCharEditAttribute->initialize();
    connect(_tabCharEditAttribute, SIGNAL(guidedNextStep()), SLOT(handleGuidedNext()));
    connect(_tabCharEditAttribute, SIGNAL(disableNext()), SLOT(handleDisableNext()));

    _tabCharEditMagic = new CharEditMagicTab();
    _tabCharEditMagic->initialize();
    connect(_tabCharEditMagic, SIGNAL(guidedNextStep()), SLOT(handleGuidedNext()));
    connect(_tabCharEditMagic, SIGNAL(disableNext()), SLOT(handleDisableNext()));

    _tabCharEditQuality = new CharEditQualityTab();
    _tabCharEditQuality->initialize();
    connect(_tabCharEditQuality, SIGNAL(guidedNextStep()), SLOT(handleGuidedNext()));
    connect(_tabCharEditQuality, SIGNAL(disableNext()), SLOT(handleDisableNext()));
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

        // Set character default values
        CHARACTER_CHOICES->setIsMagicUser(false);
        CHARACTER_VALUES->setKarmaPool(25);

        // Insert widgets into tab container
        // Step 1: Metatype & magic type
        ui->charTabs->addTab(_tabCharEditMisc, Dictionary::getTranslation("TAB_1_NAME_TYPES"));
        _tabCharEditMisc->setEnabled(true);
        // Step 2: Attributes
        ui->charTabs->addTab(_tabCharEditAttribute, Dictionary::getTranslation("TAB_2_ATTRIBUTES"));
        _tabCharEditAttribute->setEnabled(false);
        // Step 3: Magic / Resonance
        ui->charTabs->addTab(_tabCharEditMagic, Dictionary::getTranslation("TAB_3_MAGIC_RESONANCE"));
        _tabCharEditMagic->setEnabled(false);
        // Step 4: Qualities
        ui->charTabs->addTab(_tabCharEditQuality, Dictionary::getTranslation("TAB_4_QUALITIES"));
        _tabCharEditQuality->setEnabled(false);
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
        ui->charTabs->setTabEnabled(ui->charTabs->currentIndex() + 1, true);
        ui->charTabs->widget(ui->charTabs->currentIndex() + 1)->setEnabled(true);
        ui->charTabs->setCurrentIndex(ui->charTabs->currentIndex() + 1);
    }
    else
    {
        ui->charTabs->setCurrentIndex(0);
    }
}

//---------------------------------------------------------------------------------
void
MainSplitView::handleDisableNext()
{
    for (int i = ui->charTabs->currentIndex() + 1; i < ui->charTabs->count(); ++i)
    {
        ui->charTabs->setTabEnabled(i, false);
    }
}

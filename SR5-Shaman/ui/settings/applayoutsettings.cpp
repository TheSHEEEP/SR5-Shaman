#include "applayoutsettings.h"
#include "ui_applayoutsettings.h"

#include <QSettings>
#include <QFont>
#include <QFontDatabase>

//---------------------------------------------------------------------------------
AppLayoutSettings::AppLayoutSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppLayoutSettings)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
AppLayoutSettings::~AppLayoutSettings()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
AppLayoutSettings::initialize()
{
    // Add fonts
    QFontDatabase database;
    _fontIDs["Bog Stand"] = database.addApplicationFont(":/Fonts/Bog Stand");
    _fontIDs["Expressway"] = database.addApplicationFont(":/Fonts/Expressway");
    _fontIDs["Fragile Bomber"] = database.addApplicationFont(":/Fonts/Fragile Bomber");
    _fontIDs["Hemi Head"] = database.addApplicationFont(":/Fonts/Hemi Head");
    _fontIDs["Jagged Dreams"] =  database.addApplicationFont(":/Fonts/Jagged Dreams");
    _fontIDs["New Republic"] = database.addApplicationFont(":/Fonts/New Republic");
    _fontIDs["Pakenham"] = database.addApplicationFont(":/Fonts/Pakenham");
    _fontIDs["Teen"] = database.addApplicationFont(":/Fonts/Teen");
    _fontIDs["Zeroes Three"] = database.addApplicationFont(":/Fonts/Zeroes Three");

    // Set the stored font
    QString storedFont =
            _settings.value("preferences/layout/fontName", "Zeroes Three").toString();
    QString fontFamily;
    // Get default font or...
    if (storedFont == "Default")
    {
        QFont defaultFont;
        fontFamily = defaultFont.defaultFamily();
    }
    // ... get custom font
    else
    {
        fontFamily = database.applicationFontFamilies(_fontIDs[storedFont]).at(0);
    }

    QFont newFont(fontFamily,
                  _settings.value("preferences/layout/fontSize", 10).toInt(),
                  QFont::Normal, false);
    QApplication::setFont(newFont);

    // Enter font names
    ui->cbFont->addItem("Default", "Default");
    ui->cbFont->addItem("Bog Stand", "Bog Stand");
    ui->cbFont->addItem("Expressway", "Expressway");
    ui->cbFont->addItem("Fragile Bomber", "Fragile Bomber");
    ui->cbFont->addItem("Hemi Head", "Hemi Head");
    ui->cbFont->addItem("Jagged Dreams", "Jagged Dreams");
    ui->cbFont->addItem("New Republic", "New Republic");
    ui->cbFont->addItem("Pakenham", "Pakenham");
    ui->cbFont->addItem("Teen", "Teen");
    ui->cbFont->addItem("Zeroes Three", "Zeroes Three");
    ui->cbFont->setCurrentIndex(ui->cbFont->findData(storedFont));

    // Setup font sizes
    ui->spinFontSize->setValue(_settings.value("preferences/layout/fontSize", 10).toInt());

    // Register slots
    connect(ui->cbFont, SIGNAL(currentIndexChanged(QString)), SLOT(handleFontNameChange(QString)));
    connect(ui->spinFontSize, SIGNAL(valueChanged(int)), SLOT(handleFontSizeChange(int)));
}

//---------------------------------------------------------------------------------
void
AppLayoutSettings::handleFontNameChange(QString p_newCaption)
{
    // Store
    _settings.setValue("preferences/layout/fontName", p_newCaption);

    QFontDatabase database;
    QString fontFamily;
    // Get default font or...
    if (p_newCaption == "Default")
    {
        QFont defaultFont;
        fontFamily = defaultFont.defaultFamily();
    }
    // ... get custom font
    else
    {
        fontFamily = database.applicationFontFamilies(_fontIDs[p_newCaption]).at(0);
    }

    // Apply
    QFont newFont(fontFamily,
                  ui->spinFontSize->value(),
                  QFont::Normal, false);
    QApplication::setFont(newFont);
}

//---------------------------------------------------------------------------------
void
AppLayoutSettings::handleFontSizeChange(int p_newSize)
{
    // Store
    _settings.setValue("preferences/layout/fontSize", p_newSize);

    QFontDatabase database;
    QString fontFamily;
    // Get default font or...
    if (ui->cbFont->currentText() == "Default")
    {
        QFont defaultFont;
        fontFamily = defaultFont.defaultFamily();
    }
    // ... get custom font
    else
    {
        fontFamily = database.applicationFontFamilies(_fontIDs[ui->cbFont->currentText()]).at(0);
    }

    // Apply
    QFont newFont(fontFamily,
                  p_newSize,
                  QFont::Normal, false);
    QApplication::setFont(newFont);
}

#include "applayoutsettings.h"
#include "ui_applayoutsettings.h"

#include <QSettings>
#include <QFont>
#include <QFontDatabase>
#include <QFile>
#include "data/appstatus.h"

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
    // THEME
    // Add themes
    _themes["Default"] = "";
    _themes["Dark Orange"] = ":stylesheets/darkOrange/darkOrange.qss";
    _themes["Dark"] = ":stylesheets/dark/dark.qss";
    _themes["Darkstyle"] = ":stylesheets/qdarkstyle/style.qss";

    // Set CB choices
    ui->cbTheme->addItem("Default", "Default");
    ui->cbTheme->addItem("Dark", "Dark");
    ui->cbTheme->addItem("Darkstyle", "Darkstyle");
    ui->cbTheme->addItem("Dark Orange", "Dark Orange");

    // Get the stored theme
    QString storedTheme = _settings.value("preferences/layout/theme", "Default").toString();

    // Apply the theme
    applyStylesheet(_themes[storedTheme]);
    ui->cbTheme->setCurrentIndex(ui->cbTheme->findData(storedTheme));

    // FONTS -----------------------------------------------------
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
    _fontIDs["Default"] = database.addApplicationFont(":/Fonts/Zeroes Three");

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

    // Register slots
    connect(ui->cbTheme, SIGNAL(currentIndexChanged(QString)), SLOT(handleThemeChange(QString)));
    connect(ui->cbFont, SIGNAL(currentIndexChanged(QString)), SLOT(handleFontNameChange(QString)));
    connect(ui->spinFontSize, SIGNAL(valueChanged(int)), SLOT(handleFontSizeChange(int)));
}

//---------------------------------------------------------------------------------
void
AppLayoutSettings::handleThemeChange(QString p_newCaption)
{
    // Store
    _settings.setValue("preferences/layout/theme", p_newCaption);

    // Apply the theme
    applyStylesheet(_themes[p_newCaption]);
    ui->cbTheme->setCurrentIndex(ui->cbTheme->findData(p_newCaption));
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

//---------------------------------------------------------------------------------
void
AppLayoutSettings::applyStylesheet(const QString& p_sheet)
{
    if (p_sheet != "")
    {
        QFile file(p_sheet);
        if(file.open(QFile::ReadOnly))
        {
           QString styleSheet = QLatin1String(file.readAll());
           qApp->setStyleSheet(styleSheet);

           // Read some specific entries to get the color values
           QRegularExpression regExp("CustomText(?s).*\\}");
           QRegularExpressionMatch match = regExp.match(styleSheet);
           if (match.hasMatch())
           {
               QString searchText = match.captured();
               QRegularExpression regExp2("([0-9]{3})");
               QRegularExpressionMatchIterator matches = regExp2.globalMatch(searchText);
               QColor color(0, 0, 0);
               int r = matches.next().captured().toInt();
               int g = matches.next().captured().toInt();
               int b = matches.next().captured().toInt();
               color.setRgb(r, g, b);
               APPSTATUS->getHelperColors().cbFree = color;
               r = matches.next().captured().toInt();
               g = matches.next().captured().toInt();
               b = matches.next().captured().toInt();
               color.setRgb(r, g, b);
               APPSTATUS->getHelperColors().cbTaken = color;
               r = matches.next().captured().toInt();
               g = matches.next().captured().toInt();
               b = matches.next().captured().toInt();
               color.setRgb(r, g, b);
               APPSTATUS->getHelperColors().statusBarMessage = color;
               r = matches.next().captured().toInt();
               g = matches.next().captured().toInt();
               b = matches.next().captured().toInt();
               color.setRgb(r, g, b);
               APPSTATUS->getHelperColors().tableOddBg = color;
           }
        }
    }
    else
    {
        qApp->setStyleSheet("");
        APPSTATUS->getHelperColors().cbFree = QColor(0, 0, 0);
        APPSTATUS->getHelperColors().cbTaken = QColor(255, 0, 0);
        APPSTATUS->getHelperColors().statusBarMessage = QColor(0, 0, 255);
        APPSTATUS->getHelperColors().tableOddBg = QColor(235, 235, 255);
    }
}

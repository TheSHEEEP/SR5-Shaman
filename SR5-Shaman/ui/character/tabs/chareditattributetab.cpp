#include "chareditattributetab.h"
#include "ui_chareditattributetab.h"

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "rules/rules.h"

//---------------------------------------------------------------------------------
CharEditAttributeTab::CharEditAttributeTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CharEditAttributeTab)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CharEditAttributeTab::~CharEditAttributeTab()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::initialize()
{
    // Connect all spin buttons
    _attributeSpinBoxAssignment[ui->spinAgility] = "agility";
    _attributeSpinBoxAssignment[ui->spinBody] = "body";
    _attributeSpinBoxAssignment[ui->spinCharisma] = "charisma";
    _attributeSpinBoxAssignment[ui->spinEdge] = "edge";
    _attributeSpinBoxAssignment[ui->spinIntuition] = "intuition";
    _attributeSpinBoxAssignment[ui->spinLogic] = "logic";
    _attributeSpinBoxAssignment[ui->spinMagic] = "magic";
    _attributeSpinBoxAssignment[ui->spinReaction] = "reaction";
    _attributeSpinBoxAssignment[ui->spinStrength] = "strength";
    _attributeSpinBoxAssignment[ui->spinWillpower] = "willpower";
    connect(ui->spinAgility, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinBody, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinCharisma, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinEdge, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinIntuition, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinLogic, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinMagic, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinReaction, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinStrength, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));
    connect(ui->spinWillpower, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));

    // Insert attribute priorities
    ui->cbPriority->blockSignals(true);
    ui->cbPriority->addItem(QString("A (%1)").arg(ATTRIBUTE_RULES->getNumFreebies(0)), 0);
    ui->cbPriority->addItem(QString("B (%1)").arg(ATTRIBUTE_RULES->getNumFreebies(1)), 1);
    ui->cbPriority->addItem(QString("C (%1)").arg(ATTRIBUTE_RULES->getNumFreebies(2)), 2);
    ui->cbPriority->addItem(QString("D (%1)").arg(ATTRIBUTE_RULES->getNumFreebies(3)), 3);
    ui->cbPriority->addItem(QString("E (%1)").arg(ATTRIBUTE_RULES->getNumFreebies(4)), 4);
    ui->cbPriority->addItem(QString(" "), -1);
    ui->cbPriority->setCurrentIndex(5);
    ui->cbPriority->blockSignals(false);
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::spinBoxChanged(int p_newValue)
{
    // Get the box and the attribute
    QSpinBox* spinBox = (QSpinBox*)sender();
    QString& attribute = _attributeSpinBoxAssignment[spinBox];

    // We're going to change the text so lock updates
    spinBox->blockSignals(true);

    // Get the current attribute value
    int currentValue = CHARACTER_VALUES->getAttribute(attribute, false, false);

    // Try the increase/decrease
    int fromFreebies = 0;
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        fromFreebies = p_newValue - currentValue;
    }
    CHARACTER_CHOICES->increaseAttribute(attribute, p_newValue - currentValue, fromFreebies);

    // Update the text
    updateSpinBoxText(spinBox, attribute);

    // Done, unlock signals
    spinBox->blockSignals(false);
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::updateSpinBoxText(QSpinBox* p_spinBox, const QString& p_attribute)
{
    int currentValue = CHARACTER_VALUES->getAttribute(p_attribute, false, false);
    int maxValue = METATYPE_RULES->getDefinition(CHARACTER_CHOICES->getMetatypeID()).attributesMax[p_attribute];

    QString suffix = QString(" / %1").arg(maxValue);
    // TODO: add augmentation/other influences here

    p_spinBox->blockSignals(true);
    p_spinBox->setValue(currentValue);
    p_spinBox->setSuffix(suffix);
    p_spinBox->blockSignals(false);

    // TODO: update correct derived values
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::updateValues()
{
    if (!isEnabled())
    {
        return;
    }

    // Priority value
    ui->cbPriority->blockSignals(true);
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES) != -1)
    {
        ui->cbPriority->setCurrentIndex(CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES));
    }
    else
    {
        ui->cbPriority->setCurrentIndex(5);
    }
    ui->cbPriority->blockSignals(false);

    // TODO: Give each available priority in a comboBox another color if it is occupied

    // SpinBoxes
    updateSpinBoxText(ui->spinAgility, "agility");
    updateSpinBoxText(ui->spinBody, "body");
    updateSpinBoxText(ui->spinCharisma, "charisma");
    updateSpinBoxText(ui->spinEdge, "edge");
    updateSpinBoxText(ui->spinIntuition, "intuition");
    updateSpinBoxText(ui->spinLogic, "logic");
    updateSpinBoxText(ui->spinMagic, "magic");
    updateSpinBoxText(ui->spinReaction, "reaction");
    updateSpinBoxText(ui->spinStrength, "strength");
    updateSpinBoxText(ui->spinWillpower, "willpower");

    // Magic could be disabled
    bool magicUser = CHARACTER_CHOICES->getIsMagicUser();
    ui->spinMagic->setEnabled(magicUser);
    ui->lblMagic->setEnabled(magicUser);
    ui->lblAstralIniValue->setEnabled(magicUser);
    ui->lblAtralIni->setEnabled(magicUser);

    // Derived values
    updateDerivedValues();
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::updateDerivedValues()
{
    // Limits
    ui->lblPhysicalLimitValue->setText(QString("%1")
                                       .arg(ATTRIBUTE_RULES->calculatePhysicalLimit(
                                                CHARACTER_VALUES->getAttribute("strength"),
                                                CHARACTER_VALUES->getAttribute("body"),
                                                CHARACTER_VALUES->getAttribute("reaction"))));
    ui->lblMentalLimitValue->setText(QString("%1")
                                       .arg(ATTRIBUTE_RULES->calculateMentalLimit(
                                                CHARACTER_VALUES->getAttribute("logic"),
                                                CHARACTER_VALUES->getAttribute("intuition"),
                                                CHARACTER_VALUES->getAttribute("willpower"))));
    ui->lblSocialLimitValue->setText(QString("%1")
                                       .arg(ATTRIBUTE_RULES->calculateSocialLimit(
                                                CHARACTER_VALUES->getAttribute("charisma"),
                                                CHARACTER_VALUES->getAttribute("willpower"),
                                                CHARACTER_VALUES->getEssence())));

    // TODO: other derived values
    // TODO: here
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::showEvent(QShowEvent* /*unused*/)
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        ui->btnGuidedContinue->show();
        updateValues();

        // Enable or disable the attribute increase buttons
        bool enable = CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES) != -1;
        ui->attributeFrame->setEnabled(enable);
        ui->limitFrame->setEnabled(enable);

        checkContinue();
    }
    else
    {
        ui->btnGuidedContinue->hide();
    }
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::checkContinue()
{
    if (APPSTATUS->getState() == APPSTATE_GUIDED_CREATION)
    {
        // Must have a priority and spent all attribute points
        if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES) != -1 &&
            CHARACTER_CHOICES->getAvailableAttributePoints() == 0)
        {
            ui->btnGuidedContinue->setEnabled(true);
            ui->btnGuidedContinue->setText(tr("Continue"));
        }
        else
        {
            ui->btnGuidedContinue->setEnabled(false);
            ui->btnGuidedContinue->setText(tr("Select priority and spend attribute points to continue"));

            emit disableNext();
        }
    }
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::on_cbPriority_currentIndexChanged(int p_index)
{
    // Set/Unset the chosen priority for the attributes
    if (ui->cbPriority->itemData(p_index).toInt() != -1)
    {
        CHARACTER_CHOICES->setPriority(p_index, PRIORITY_ATTRIBUTES);
    }
    else
    {
        CHARACTER_CHOICES->unsetPriority(PRIORITY_ATTRIBUTES);
    }

    // It is possible that this choice deselected the metatype
    if (CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE) == -1)
    {
        setEnabled(false);
        APPSTATUS->setStatusBarMessage(
                    tr("Deselected metatype. Go back and select your metatype!"), 5.0f, QColor(255, 0, 0));
        return;
    }

    // Enable or disable the attribute increase buttons
    ui->attributeFrame->setEnabled(CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES) != -1);
    ui->limitFrame->setEnabled(CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES) != -1);

    // Update the displayed values
    updateValues();

    // Check if we can continue
    checkContinue();
}

//---------------------------------------------------------------------------------
void
CharEditAttributeTab::on_btnGuidedContinue_clicked()
{
    emit guidedNextStep();
}

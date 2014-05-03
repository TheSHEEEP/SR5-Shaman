#include "creationsideinfo.h"
#include "ui_creationsideinfo.h"

#include <QTimer>
#include <QMap>
#include <QLabel>

#include "data/character/characterchoices.h"
#include "data/appstatus.h"
#include "data/dictionary.h"

//---------------------------------------------------------------------------------
CreationSideInfo::CreationSideInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreationSideInfo)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------------
CreationSideInfo::~CreationSideInfo()
{
    delete ui;

    if (_timer)
    {
        _timer->stop();
        delete _timer;
    }
}

//---------------------------------------------------------------------------------
void
CreationSideInfo::initialize()
{
    // Update the side info automatically once per second
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
    _timer->start(1000);
}

//---------------------------------------------------------------------------------
void
CreationSideInfo::update()
{
    // Do nothing when not visible
    if (!isVisible())
    {
        return;
    }

    // Get the index of each priority
    QMap<int, Priority> indexPriorityMap;
    for (int i = 0; i < 5; ++i)
    {
        indexPriorityMap[i] = PRIORITY_INVALID;
    }
    indexPriorityMap[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_ATTRIBUTES)] = PRIORITY_ATTRIBUTES;
    indexPriorityMap[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_METATYPE)] = PRIORITY_METATYPE;
    indexPriorityMap[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)] = PRIORITY_MAGIC;
    indexPriorityMap[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_SKILLS)] = PRIORITY_SKILLS;
    indexPriorityMap[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_RESOURCES)] = PRIORITY_RESOURCES;

    // Check each priority and show the current selection
    updatePriorityLabelText(ui->lblASel, 0, indexPriorityMap[0]);
    updatePriorityLabelText(ui->lblBSel, 1, indexPriorityMap[1]);
    updatePriorityLabelText(ui->lblCSel, 2, indexPriorityMap[2]);
    updatePriorityLabelText(ui->lblDSel, 3, indexPriorityMap[3]);
    updatePriorityLabelText(ui->lblESel, 4, indexPriorityMap[4]);
}

//---------------------------------------------------------------------------------
void
CreationSideInfo::updatePriorityLabelText(QLabel* p_label, int p_prioIndex, Priority p_prio)
{
    // TODO: Show all information
    switch (p_prio)
    {
    case PRIORITY_NUM_PRIORITIES:
    case PRIORITY_INVALID:
        p_label->setText("-");
        break;

    case PRIORITY_ATTRIBUTES:
        p_label->setText(Dictionary::getTranslation("SIDE_ATTRIBUTES")
                         .arg(ATTRIBUTE_RULES->getNumFreebies(p_prioIndex)));
        break;

    case PRIORITY_SKILLS:
        p_label->setText(Dictionary::getTranslation("SIDE_SKILLS")
                         .arg(SKILL_RULES->getNumSkillPoints(p_prioIndex, false))
                         .arg(SKILL_RULES->getNumSkillPoints(p_prioIndex, true)));
        break;

    case PRIORITY_RESOURCES:
        p_label->setText(tr("Resources"));
        break;

    case PRIORITY_MAGIC:
        {
            if (CHARACTER_CHOICES->getIsMagicUser())
            {
                if (CHARACTER_CHOICES->getMagicUserTypeID() == "")
                {
                    return;
                }
                const MagicTypeDefinition& magicType =
                        MAGIC_RULES->getMagicTypeDefinition(CHARACTER_CHOICES->getMagicUserTypeID());
                QString magicTypeName = magicType.translations[APPSTATUS->getCurrentLocale()];
                p_label->setText(Dictionary::getTranslation("SIDE_MAGIC_TYPE")
                                 .arg(magicTypeName));
            }
            else
            {p_label->setText(Dictionary::getTranslation("SIDE_MAGIC"));
            }
        }
        break;

    case PRIORITY_METATYPE:
        if (CHARACTER_CHOICES->getMetatypeID() == "")
        {
            return;
        }
        const MetatypeDefinition& type =
                METATYPE_RULES->getDefinition(CHARACTER_CHOICES->getMetatypeID());
        QString metatypeName = type.translations[APPSTATUS->getCurrentLocale()];
        int specialAttribPoints = type.specialAttribPointsPerPrio[p_prioIndex];
        p_label->setText(Dictionary::getTranslation("SIDE_METATYPE")
                            .arg(metatypeName)
                            .arg(specialAttribPoints));
        break;
    }
}


#ifndef RESEFFECT_H
#define RESEFFECT_H

#include <vector>
#include <QString>
#include <QVariant>
#include "rules/commonrules.h"

class QJsonValueRef;

/**
 * @brief The different types of effects.
 */
enum ResourceEffectType
{
    RESOURCE_EFFECTTYPE_INVALID = -1,
    RESOURCE_EFFECTTYPE_MOD_AC,
    RESOURCE_EFFECTTYPE_MOD_RC,
    RESOURCE_EFFECTTYPE_NONE,
    NUM_RESOURCE_EFFECTTYPES
};

/**
 * @brief This class holds a single resource effect, as applied by mounts.
 */
class ResourceEffect
{
public:
    /**
     * @brief Constructor.
     * @param p_jsonObject  The JSON node to construct this ResourceEffect from.
     */
    ResourceEffect(QJsonValueRef* p_jsonObject);

    /**
     * @brief Copy constructor.
     */
    ResourceEffect(const ResourceEffect& p_other);

    /**
     * @brief Destructor.
     */
    ~ResourceEffect();

    /**
     * @brief Returns the type of this ResourceEffect.
     */
    const ResourceEffectType getType() const;

    /**
     * @brief Returns the ID of the current target. May be a skill, an attribute, etc.
     */
    const QString& getCurrentTarget() const;

    /**
     * @brief returns the value of this ResourceEffect.
     */
    const QVariant& getValue() const;

protected:
    ResourceEffectType      _resourceEffectType;
    QString                 _target;
    QString                 _currentTarget;
    QVariant                _value;
};

//---------------------------------------------------------------------------------
inline
const ResourceEffectType
ResourceEffect::getType() const
{
    return _resourceEffectType;
}

//---------------------------------------------------------------------------------
inline
const QString&
ResourceEffect::getCurrentTarget() const
{
    return _currentTarget;
}

//---------------------------------------------------------------------------------
inline
const QVariant&
ResourceEffect::getValue() const
{
    return _value;
}

#endif // RESEFFECT_H

#include "rules.h"

#include "metatyperules.h"

Rules* Rules::_instance = 0;

//---------------------------------------------------------------------------------
Rules::Rules()
    : _initialized(false)
    , _metatypeRules(0)
{

}

//---------------------------------------------------------------------------------
Rules::~Rules()
{
    if (_metatypeRules)
    {
        delete _metatypeRules;
    }
}

//---------------------------------------------------------------------------------
void
Rules::initialize()
{
    // Read metatype rules
    _metatypeRules = new MetatypeRules();
    _metatypeRules->initialize("../../ruleset/metatypes.json");

    // Read attribute rules
    _attributeRules = new AttributeRules();
    _attributeRules->initialize("../../ruleset/attributes.json");

    // Read magic rules
    _magicRules = new MagicRules();
    _magicRules->initialize("../../ruleset/magic.json");

    // Done!
    _initialized = true;
}

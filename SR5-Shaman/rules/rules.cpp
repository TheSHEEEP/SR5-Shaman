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

    // Done!
    _initialized = true;
}

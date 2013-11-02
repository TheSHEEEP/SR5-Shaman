#include "characterdata.h"

CharacterData* CharacterData::_instance = 0;

//---------------------------------------------------------------------------------
CharacterData::CharacterData()
    : _metatypeID("")
{
    _selectedPriorities.resize(5, PRIORITY_INVALID);
}

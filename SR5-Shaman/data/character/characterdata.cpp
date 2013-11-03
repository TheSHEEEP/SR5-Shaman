#include "characterdata.h"

CharacterData* CharacterData::_instance = 0;

//---------------------------------------------------------------------------------
CharacterData::CharacterData()
    : _metatypeID("")
{
    _selectedPriorities.resize(5, PRIORITY_INVALID);
}

//---------------------------------------------------------------------------------
void
CharacterData::setPriority(int p_priorityIndex, Priority p_prio)
{
    // Check if there already is a priority at the passed index
    if (_selectedPriorities[p_priorityIndex] != PRIORITY_INVALID)
    {
        Priority oldPrio = _selectedPriorities[p_priorityIndex];

        // Check if the passed priority is already somewhere else and switch
        for (int i = 0; i < 5; ++i)
        {
            if (i != p_priorityIndex &&
                _selectedPriorities[i] == p_prio)
            {
                _selectedPriorities[i] = oldPrio;
                break;
            }
        }
    }

    // Set the priotiy
    _selectedPriorities[p_priorityIndex] = p_prio;
}

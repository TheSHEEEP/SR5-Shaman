#include "chosenresource.h"

//---------------------------------------------------------------------------------
ChosenResource::ChosenResource(const QString &p_originalID)
{
}


//---------------------------------------------------------------------------------
void
ChosenResource::addMount(ChosenResource* p_mount)
{
    _mounts.push_back(p_mount);
}

//---------------------------------------------------------------------------------
void
ChosenResource::removeMount(ChosenResource* p_mount)
{
    for (unsigned int i = 0; i < _mounts.size(); ++i)
    {
        if (_mounts[i] == p_mount)
        {
            _mounts.erase(_mounts.begin() + i);
            break;
        }
    }
}

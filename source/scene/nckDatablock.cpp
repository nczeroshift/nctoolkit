
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckDatablock.h"

_SCENE_BEGIN

Datablock::Datablock(Graph::Device *dev)
{
    m_Device = dev;
}

std::string Datablock::GetName()
{
    return m_Name;
}

void Datablock::SetName(std::string name)
{
    m_Name = name;
}

_SCENE_END

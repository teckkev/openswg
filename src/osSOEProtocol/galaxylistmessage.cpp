/**
 * Galaxy Open-Source Massively Multiplayer Game Simulation Engine
 * Copyright (C) 2007 OpenSWG Team <http://www.openswg.com>
 */

// *********************************************************************
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// To read the license please visit http://www.gnu.org/copyleft/gpl.html
// *********************************************************************

#include <osSOEProtocol/galaxylistmessage.h>
#include <osSOEProtocol/opcodes.h>

using namespace gsNetwork;
using namespace osSOEProtocol;

GalaxyListMessage::GalaxyListMessage()
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
}

GalaxyListMessage::GalaxyListMessage(std::shared_ptr<BinaryPacket> packet)
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
	m_serializedData = packet;
}

GalaxyListMessage::~GalaxyListMessage()
{}

std::shared_ptr<BinaryPacket> GalaxyListMessage::serialize()
{
    std::shared_ptr<BinaryPacket> packet(GS_NEW BinaryPacket);
    *packet << (uint16_t)SOE_CHL_DATA_A;
    *packet << (uint16_t)htons(getSequence());
    *packet << (uint16_t)3;
    *packet << (uint32_t)SMSG_SERVER_NAMES;
    *packet << (uint32_t)galaxies.size(); // Galaxy Count

    // Add galaxy data here.
	for (std::list<GalaxyCluster*>::iterator i = galaxies.begin(); i != galaxies.end(); ++i)
    {
       // if ((*i).getVersion() != session->getVersion())
         //   continue;

        *packet << (*i)->getGalaxyId();
        *packet << (uint16_t)(*i)->getGalaxyName().length();
        *packet << (*i)->getGalaxyName();
        *packet << (uint32_t)0xFFFF8F80; // Unknown
    }

    *packet << (uint32_t)8;
    *packet << (uint8_t)0;
    *packet << (uint16_t)0;
	return packet;
}

void GalaxyListMessage::unserialize()
{
}


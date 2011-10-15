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

#include <osSOEProtocol/connectionserverlistmessage.h>
#include <osSOEProtocol/opcodes.h>
#include <gsNetwork/gamesocket.h>

using namespace gsNetwork;
using namespace osSOEProtocol;

ConnectionServerListMessage::ConnectionServerListMessage()
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
}

ConnectionServerListMessage::ConnectionServerListMessage(BinaryPacketPtr packet)
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
	m_serializedData = packet;
}

ConnectionServerListMessage::~ConnectionServerListMessage()
{}

BinaryPacketPtr ConnectionServerListMessage::serialize()
{
    BinaryPacketPtr packet(GS_NEW BinaryPacket);
    *packet << (uint16_t)SOE_CHL_DATA_A;
    *packet << (uint16_t)htons(getSequence());
    *packet << (uint16_t)2;
    *packet << (uint32_t)SMSG_SERVER_ADDR;
	*packet << (uint32_t)connectionServers.size(); // Temporary holder for Connection server count

    // Add connection server list
	for (std::list<GalaxyServer*>::iterator i = connectionServers.begin(); i != connectionServers.end(); ++i)
    {
       // if ((*i).version != session->getVersion())
       //     continue;

        *packet << (*i)->getGalaxy()->getGalaxyId();
		*packet << (uint16_t)(*i)->getServerAddress().length();
		*packet << (*i)->getServerAddress();
		*packet << (uint16_t)(*i)->getClientPort();
		*packet << (uint16_t)(*i)->getPingPort();
		*packet << (uint32_t)(*i)->getGalaxy()->getPopulation();

        *packet << (uint32_t)0x00000CB2;
        *packet << (uint32_t)8;
        *packet << (uint32_t)0xFFFF8F80;
		*packet << (uint32_t)(*i)->getGalaxy()->getGalaxyStatus();
        *packet << (uint8_t)0;
    }

    *packet << (uint8_t)0;
    *packet << (uint16_t)0;

	return packet;
}

void ConnectionServerListMessage::unserialize()
{
}


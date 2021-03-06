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

#include <gsCore/timing.h>
#include <osSOEProtocol/movementmessage.h>
#include <osSOEProtocol/opcodes.h>
#include <osSOEProtocol/soepackettools.h>

using namespace gsNetwork;
using namespace osSOEProtocol;

MovementMessage::MovementMessage()
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
	m_lastUpdateTime = 0;
	movedX = false;
	movedY = false;
	movedZ = false;
}

MovementMessage::MovementMessage(std::shared_ptr<BinaryPacket> packet)
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
	m_serializedData = packet;
	m_lastUpdateTime = 0;
	movedX = false;
	movedY = false;
	movedZ = false;
}

MovementMessage::~MovementMessage()
{}

bool MovementMessage::canSendScheduled()
{
	uint64_t m_currentTime = gsCore::getTimeInMs();

	if (m_currentTime - m_lastUpdateTime > 300)
	{
		m_lastUpdateTime = m_currentTime;
		return true;
	}
	else
		return false;
}
		
std::shared_ptr<NetworkMessage> MovementMessage::clone()
{
	std::shared_ptr<MovementMessage> clone(new MovementMessage);
	clone->objectId = this->objectId; // Target (this is the character moving)
    clone->ticks = this->ticks; // Ticks?
    clone->timer = this->timer; // Times?
    clone->direction = this->direction;
    clone->quaternionX = this->quaternionX; // Orientation x
    clone->quaternionY = this->quaternionY; // Orientation y
    clone->quaternionZ = this->quaternionZ; // Orientation z
    clone->quaternionW = this->quaternionW; // Orientation w
    clone->positionX = this->positionX; // position x
    clone->positionY = this->positionY; // position y
    clone->positionZ = this->positionZ; // position z
    clone->speed = this->speed; // speed
	clone->orientation = this->orientation;

	return clone;
}

std::shared_ptr<BinaryPacket> MovementMessage::serialize()
{
    std::shared_ptr<BinaryPacket> packet(new BinaryPacket);
    *packet << (uint16_t)SOE_CHL_DATA_A;
    *packet << (uint16_t)htons(getSequence());
    *packet << (uint16_t)8;

	*packet << (uint32_t)SMSG_POS_UPDATE;
    *packet << objectId;

	*packet << (uint16_t)((positionX * 4.f)+0.5f);
	*packet << (uint16_t)((positionY * 4.f)+0.5f);
	*packet << (uint16_t)((positionZ * 4.f)+0.5f);

	*packet << (uint32_t)(timer+1);

	*packet << (uint8_t)1; 
    *packet << (uint8_t)direction;

    *packet << (uint8_t)0 << (uint16_t)0;

	return packet;
}

void MovementMessage::unserialize()
{
	objectId = m_serializedData->read<uint64_t>(); // Target (this is the character moving)
    ticks = m_serializedData->read<uint32_t>(); // Ticks?
    timer = m_serializedData->read<uint32_t>(); // Times?
    quaternionX = m_serializedData->read<float>(); // Quaternion x
    quaternionY = m_serializedData->read<float>(); // Quaternion y
    quaternionZ = m_serializedData->read<float>(); // Quaternion z
    quaternionW = m_serializedData->read<float>(); // Quaternion w
    positionX = m_serializedData->read<float>(); // position x
    positionY = m_serializedData->read<float>(); // position y
    positionZ = m_serializedData->read<float>(); // position z
    speed = m_serializedData->read<float>(); // speed
}


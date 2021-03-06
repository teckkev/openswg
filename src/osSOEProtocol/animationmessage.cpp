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

#include <osSOEProtocol/animationmessage.h>
#include <osSOEProtocol/opcodes.h>

#include <boost/asio.hpp>

using namespace gsNetwork;
using namespace osSOEProtocol;

AnimationMessage::AnimationMessage()
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
}

AnimationMessage::AnimationMessage(std::shared_ptr<BinaryPacket> packet)
: NetworkMessage()
{
	setPriority(0);
	setResend(true);
	setEncrypt(true);
	setCrc(true);
	m_serializedData = packet;
}

AnimationMessage::~AnimationMessage()
{}

std::shared_ptr<BinaryPacket> AnimationMessage::serialize()
{
    std::shared_ptr<BinaryPacket> packet(new BinaryPacket);

    *packet << (uint16_t)SOE_CHL_DATA_A;
    *packet << (uint16_t)htons(getSequence());

    *packet << (uint16_t)5;
    *packet << (uint32_t)AMSG_INT_PACKET;
    *packet << (uint32_t)27;
    *packet << (uint32_t)204;
    *packet << (uint64_t)objectId;
    *packet << (uint32_t)0;
    *packet << (uint32_t)animationCrc; // Animation crc?
    *packet << (uint64_t)objectId;
    *packet << (uint64_t)0;
    *packet << (uint8_t)posture;
	*packet << (uint8_t)0;
	*packet << (uint16_t)0;
	*packet << (uint32_t)0;
    *packet << (uint16_t)0;

    *packet << (uint8_t)0 << (uint16_t)0;

	return packet;
}

void AnimationMessage::unserialize()
{
}


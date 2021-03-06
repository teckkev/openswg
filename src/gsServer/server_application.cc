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

#include "gsServer/server_application.h"

#include <boost/thread/thread.hpp>

#ifdef ERROR
#undef ERROR
#endif
#include <glog/logging.h>

#include <gsCore/eventmanager.h>
#include <gsCore/processmanager.h>
#include <gsServer/servereventlistener.h>
#include <gsServer/sessionmanager.h>
#include <gsNetwork/events.h>
#include <gsNetwork/gamesocketfactory.h>
#include <gsNetwork/adminsocket.h>
#include <gsNetwork/clientsocket.h>
#include <gsNetwork/commsocket.h>
#include <gsNetwork/pingsocket.h>
#include <gsCore/datastore.h>

using namespace gsCore;
using namespace gsServer;
using namespace gsNetwork;

static uint32_t SOCKET_DISABLED = 0;

mysqlpp::Connection sStationDB;
mysqlpp::Connection sGalaxyDB;

ServerApplication::ServerApplication(std::string serverType, uint32_t serverId)
: m_serverType(serverType)
, m_serverId(serverId)
, io_service_()
, work_(io_service_)
, gsApplication::Application()
{}

ServerApplication::~ServerApplication()
{
	SAFE_DELETE(m_socketFactory);
}

void ServerApplication::shutdown()
{
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	
	if (Datastore::getStationDB().connected())
		Datastore::getStationDB().disconnect();

	if (Datastore::getGalaxyDB().connected())
		Datastore::getGalaxyDB().disconnect();
}

void ServerApplication::initialize(const std::string& configFilename)
{
    Application::initialize(configFilename);
    
	// Register the basic events needed to run the server application.
	// Additional events or overrides can be performed from the concrete
	// application instance.
	EventListenerPtr eventLog(GS_NEW EventSnooper());
	EventSnooper::registerEvents(eventLog);
        
	EventListenerPtr packetLogger(GS_NEW PacketLogListener());
	PacketLogListener::registerEvents(packetLogger);

    // Create containers for the configuration data.
    std::string mainLog;
    std::string packetLog;
    uint16_t adminSocket;
    uint16_t clientSocket;
    uint16_t commSocket;
    uint16_t pingSocket;

    // Try loading the configuration before going further.
    try {
        // Load the configuration file and get some basic
        // configuration variables.
        m_config = ConfigFile(configFilename.c_str());
    } catch(...) {
        LOG(FATAL) << "Could not find configuration file: " << configFilename;
		exit(EXIT_FAILURE);
    }

    // Read the configuration data.
    m_config.readInto<uint16_t>(adminSocket,  "admin_socket");
    m_config.readInto<uint16_t>(clientSocket, "client_socket");
    m_config.readInto<uint16_t>(commSocket,   "comm_socket");
    m_config.readInto<uint16_t>(pingSocket,   "ping_socket");

//    printf(" *** Server hostname: %s\n", Utility::GetLocalHostname().c_str());
//    printf(" *** Server IP: %s\n\n", Utility::GetLocalAddress().c_str());

    if (adminSocket != SOCKET_DISABLED)
    {
        sockets_.push_back(m_socketFactory->createGameSocket(gsNetwork::AdminSocket::gkName, io_service_, adminSocket));
        LOG(WARNING) << "Listening for admin messages on port: " << adminSocket;
    }

    if (clientSocket != SOCKET_DISABLED)
    {
        sockets_.push_back(m_socketFactory->createGameSocket(gsNetwork::ClientSocket::gkName, io_service_, clientSocket));
        LOG(WARNING) << "Listening for client messages on port: " << clientSocket;
    }

    if (commSocket != SOCKET_DISABLED)
    {
        sockets_.push_back(m_socketFactory->createGameSocket(gsNetwork::CommSocket::gkName, io_service_, commSocket));
        LOG(WARNING) << "Listening for server messages on port: " << commSocket;
    }

    if (pingSocket != SOCKET_DISABLED)
    {
        sockets_.push_back(m_socketFactory->createGameSocket(gsNetwork::PingSocket::gkName, io_service_, pingSocket));
        LOG(WARNING) << "Listening for ping messages on port: " << pingSocket;
    }
}

void ServerApplication::setSocketFactory(GameSocketFactory* factory)
{
    m_socketFactory = factory;
}

void ServerApplication::startNetwork()
{
    boost::thread t([this] () {
        io_service_.run();
    });
}

void ServerApplication::run()
{
	startNetwork();

	Application::run();
}

void ServerApplication::tick(uint64_t updateTimestamp)
{
    if (isRunning())
    {
		uint64_t deltaTimestamp = (updateTimestamp - m_lastUpdateTimestamp);

		if (deltaTimestamp > 25)
		{            
            m_eventManager->tick(updateTimestamp);            
            m_processManager->tick(updateTimestamp);

	        m_lastUpdateTimestamp = updateTimestamp;
		}
    }
}


//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Mesh Network wrapper class to provide a container to add specific
//! mesh network behaviour.

#include "Facilities_MeshNetwork.hpp"

#include "Debug.hpp"
#include "painlessMesh.h"


namespace Facilities {

const uint16_t MeshNetwork::PORT = 5555;
const uint32_t ids[] = { 0x3A588228,0x3A588750,0x3A587FDB,0xEBB2A2BD};
//! Construct only.
//! \note Does not construct and initialize in one go to be able to initialize after serial debug port has been opened.
MeshNetwork::MeshNetwork()
{
   m_mesh.onReceive(std::bind(&MeshNetwork::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   MY_DEBUG_PRINTF("%s\n", m_mesh.getStationIP().toString().c_str());
    m_mesh.onReceive(std::bind(&MeshNetwork::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   m_mesh.onNewConnection(std::bind(&MeshNetwork::droppedConnection, this, std::placeholders::_1));
    m_mesh.onDroppedConnection(std::bind(&MeshNetwork::newConnection, this, std::placeholders::_1));
    m_mesh.onChangedConnections(std::bind(&MeshNetwork::meshChanged,this));
    _internalId = -1;

}

// Initialize mesh network.
void MeshNetwork::initialize(const __FlashStringHelper *prefix, const __FlashStringHelper *password, Scheduler& taskScheduler)
{
   // Set debug messages before init() so that you can see startup messages.
   m_mesh.setDebugMsgTypes( ERROR | STARTUP );  // To enable all: ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE
   m_mesh.init( prefix, password, &taskScheduler, MeshNetwork::PORT, WIFI_AP_STA, 5, 0, 5);

 MY_DEBUG_PRINTF("%X\n",m_mesh.getNodeId());
    for (int i=0;i<4;i++)
    {
        if(ids[i]==m_mesh.getNodeId()){
            _internalId = i;
            break;
        }
            
    }
}

//! Update mesh; forward call to painlessMesh.
void MeshNetwork::update()
{
   m_mesh.update();
}

void MeshNetwork::sendBroadcast(String &message)
{
   MY_DEBUG_PRINT("Broadcasting message: "); MY_DEBUG_PRINTLN(message);
   m_mesh.sendBroadcast(message, false); // false: Do not include self.
}

MeshNetwork::NodeId MeshNetwork::getMyNodeId()
{
   return m_mesh.getNodeId();
}

void MeshNetwork::onReceive(receivedCallback_t receivedCallback)
{
   m_mesh.onReceive(receivedCallback);
}

void MeshNetwork::receivedCb(NodeId transmitterNodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received from node: %u; msg: %s\n", transmitterNodeId, msg.c_str());
}


void MeshNetwork::droppedConnection(uint32_t nodeId)
{
    updateNodeId();
}

void MeshNetwork::newConnection(uint32_t nodeId)
{
    updateNodeId();
}

void MeshNetwork::updateNodeId(void)
{
    std::list<uint32_t> nodeids = m_mesh.getNodeList();
    nodeids.push_back(m_mesh.getNodeId());
    if (nodeids.size() > 0)
    {
        nodeids.sort();
        int32_t count=0;
        for (auto const& i : nodeids) {
           
            if (i == m_mesh.getNodeId())
            {
                 MY_DEBUG_PRINTF(" %X\n",i);
                _globalMeshId = i;
                break;
            }

            count++;
        }
    }
}
void MeshNetwork::meshChanged(void)
{
    updateNodeId();
}



} // namespace Facilities

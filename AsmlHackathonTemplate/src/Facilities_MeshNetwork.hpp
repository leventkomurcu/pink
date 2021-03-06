//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Mesh Network wrapper class.

#ifndef __Facilities_MeshNetwork__
#define __Facilities_MeshNetwork__

#include "painlessMesh.h"
#include <functional>

namespace Facilities {

class MeshNetwork
{

public:
   typedef uint32_t NodeId;

   MeshNetwork();
   ~MeshNetwork() {};

   // Disallow copy-ing
	MeshNetwork(const MeshNetwork& other) = delete;
	MeshNetwork(MeshNetwork&& other) = delete;
	MeshNetwork& operator=(const MeshNetwork& other) = delete;

   void update();
   void initialize(const __FlashStringHelper *prefix, const __FlashStringHelper *password, Scheduler& taskScheduler);

   void sendBroadcast(String& message);
   NodeId getMyNodeId();

   void onReceive(receivedCallback_t receivedCallback);



   static const uint16_t PORT;

   painlessMesh       m_mesh;
int32_t _internalId;
uint32_t _globalMeshId;
   void receivedCb(NodeId transmitterNodeId, String& msg);
private:
   
   void updateNodeId(void);


   void droppedConnection(uint32_t nodeId);
    void newConnection(uint32_t nodeId);
    void meshChanged(void);


};

} // namespace Facilities

#endif // __Facilities_MeshNetwork__

#include <Arduino.h>

#include <Hash.h>
#include "Debug.hpp"
#include "painlessMesh.h"
#include "Facilities_MeshNetwork.hpp"
#include "Tasks_ExampleTransmitTask.hpp"
#include "Tasks_ExampleDisplayTask.hpp"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Translation unit local variables
namespace {

Scheduler                  taskScheduler;

Facilities::MeshNetwork    meshNetwork;
Tasks::ExampleTransmitTask exampleTransmitTask(meshNetwork);
Tasks::ExampleDisplayTask  exampleDisplayTask(meshNetwork);
}

AsyncWebServer server(80);
String image = "No image yet";

//! Called once at board startup.
void setup()
{
   MY_DEBUG_BEGIN(115200);
wdt_disable();
   // Create MeshNetwork
   meshNetwork.initialize(F("teampink"), F("teampink"), taskScheduler);

   // Create and add tasks.
   taskScheduler.addTask( exampleTransmitTask );
   taskScheduler.addTask( exampleDisplayTask );
   exampleTransmitTask.enable();
   exampleDisplayTask.enable();

    //Async webserver
    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
        list<uint32_t> nodeList = meshNetwork.m_mesh.getNodeList();

        StaticJsonBuffer<100> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["poolSize"] = (int)nodeList.size();
        root["currentTime"] = (int)meshNetwork.m_mesh.getNodeTime();
        
        if (request->hasArg("IMAGE")){
            image = request->arg("IMAGE");
            meshNetwork.m_mesh.sendBroadcast(image,true);
        }

        root["IMAGE"] = image;

        String response;
        root.printTo(response);
        request->send(200, "application/json", response);
        // if (request->hasArg("BROADCAST")){
        // String msg = request->arg("BROADCAST");
        // mesh.sendBroadcast(msg);
        // }
        MY_DEBUG_PRINTLN(F("GOT A REQUEST"));
    });

    server.on("/image", HTTP_POST, [&](AsyncWebServerRequest *request){
        int params = request->params();
        MY_DEBUG_PRINTLN(F("GOT PARAMS"));
        MY_DEBUG_PRINTLN(params);
        if(params) {
            image = request->getParam(0)->value();
            MY_DEBUG_PRINTLN(request->getParam(0)->name());
            MY_DEBUG_PRINTLN(image);
            meshNetwork.m_mesh.sendBroadcast(image,true);
            request->send(200);
        } else {
            request->send(204);
        }
        // if (request->hasArg("BROADCAST")){
        // String msg = request->arg("BROADCAST");
        // mesh.sendBroadcast(msg);
        // }
        MY_DEBUG_PRINTLN(F("GOT POST REQUEST"));
    });
    server.begin();

   MY_DEBUG_PRINTLN(F("My IP:"));
   MY_DEBUG_PRINTLN(meshNetwork.m_mesh.getAPIP());

   MY_DEBUG_PRINTLN(F("Setup completed"));
}

//! Called repeatedly after setup().
void loop()
{
   taskScheduler.execute();
   meshNetwork.update();
}

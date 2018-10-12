#include <Arduino.h>

#include <Hash.h>
#include "Debug.hpp"
#include "painlessMesh.h"
#include "Facilities_MeshNetwork.hpp"
#include "Tasks_ExampleTransmitTask.hpp"
#include "Tasks_ExampleDisplayTask.hpp"
#include <ESPAsyncWebServer.h>

// Translation unit local variables
namespace {

Scheduler                  taskScheduler;

Facilities::MeshNetwork    meshNetwork;
Tasks::ExampleTransmitTask exampleTransmitTask(meshNetwork);
Tasks::ExampleDisplayTask  exampleDisplayTask(meshNetwork);
}

AsyncWebServer server(80);

//! Called once at board startup.
void setup()
{
   MY_DEBUG_BEGIN(115200);

   // Create MeshNetwork
   meshNetwork.initialize(F("teampink"), F("teampink"), taskScheduler);

   // Create and add tasks.
   taskScheduler.addTask( exampleTransmitTask );
   taskScheduler.addTask( exampleDisplayTask );
   exampleTransmitTask.enable();
   exampleDisplayTask.enable();

    //Async webserver
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
        // if (request->hasArg("BROADCAST")){
        // String msg = request->arg("BROADCAST");
        // mesh.sendBroadcast(msg);
        // }
        MY_DEBUG_PRINTLN(F("GOT A REQUEST"));
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

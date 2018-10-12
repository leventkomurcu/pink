//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task to output something to the LED Matrix.
#include "Tasks_ExampleDisplayTask.hpp"

#include "Debug.hpp"
#include "Facilities_MeshNetwork.hpp"

#include <LEDMatrixDriver.hpp>

#include <functional>

namespace Tasks {


const int ExampleDisplayTask::LEDMATRIX_WIDTH = 31;
const int ExampleDisplayTask::LEDMATRIX_HEIGHT = 7;
const int ExampleDisplayTask::LEDMATRIX_SEGMENTS = 4;
const int ExampleDisplayTask::LEDMATRIX_INTENSITY = 5;
const int ExampleDisplayTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleDisplayTask::POLL_DELAY_MS = 100;


uint8_t image[4][32];
const int pid=0;
//! Initializes the LED Matrix display.
ExampleDisplayTask::ExampleDisplayTask(Facilities::MeshNetwork& mesh) :
   Task(POLL_DELAY_MS , TASK_FOREVER, std::bind(&ExampleDisplayTask::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
   m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);
    m_lmd.clear();


    image[0][15]=0xFF;
    image[1][15]=0xff;
    image[2][15]=0xff;
    image[3][15]=0xff;

    for (int i=0;i<32;i++)
    {
        image[1][i] |= 0x80;
        image[2][i] |= 0x01;
    }


     m_lmd.display();
   m_mesh.onReceive(std::bind(&ExampleDisplayTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));

}


//! Update display
void ExampleDisplayTask::execute()
{
    static int currentId=-1;

    if(m_mesh._internalId!=currentId)
    {
        currentId=m_mesh._internalId;
        
        if(m_mesh._internalId >=0)
        {
                m_lmd.clear();
        for (int x=0;x<31;x++)
            {
                m_lmd.setColumn(x,image[m_mesh._internalId][x]);
            }
            m_lmd.display();
        }

    }
    

  /* m_lmd.clear();
   for (int x=0;x<31;x++)
    {
        m_lmd.setColumn(x,image[0][x]);
    }
   m_lmd.display();*/
}

void ExampleDisplayTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
    if (msg.startsWith("C0"))
    {

    }else if (msg.startsWith("C1"))
    {



    }
    else if (msg.startsWith("C2"))
    {

    }
    else if (msg.startsWith("C3"))
    {

    }

   //MY_DEBUG_PRINTLN("Received data in ExampleDisplayTask");
   
}

} // namespace Tasks

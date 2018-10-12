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

char m_image[6][32][32];
int m_image_index = 0;
const int BLANK = 1;
const int CROSS = 0;
const int MAX_DISK = 2;
const int CIRCLE = 3;
const int DISK_5 = 4;

void init_figures() {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (i == j || i == 31 - j) {
                m_image[CROSS][i][j] = 1;
            } else {
                m_image[CROSS][i][j] = 0;
            }
        }
    }

    double mx = 15.5;
    double my = 15.5;
    double r = 15.955;
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            double dx = abs(x - mx);
            double dy = abs(y - my);
            if (dx * dx + dy * dy <= r*r) {
                m_image[MAX_DISK][x][y] = 1;
            } else {
                m_image[MAX_DISK][x][y] = 0;
            }
        }
    }

    double r5 = 4.955;
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            double dx = abs(x - mx);
            double dy = abs(y - my);
            if (dx * dx + dy * dy <= r5*r5) {
                m_image[DISK_5][x][y] = 1;
            } else {
                m_image[DISK_5][x][y] = 0;
            }
        }
    }

    mx = 15.5;
    my = 15.5;
    double r_in = 15.12;
    double r_out = 15.955;
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            double dx = abs(x - mx);
            double dy = abs(y - my);
            double d = dx * dx + dy * dy;
            if (d <= r_out*r_out && d >= r_in*r_in) {
                m_image[CIRCLE][x][y] = 1;
            } else {
                m_image[CIRCLE][x][y] = 0;
            }
        }
    }
}

uint8_t image[4][32];
const int pid=0;
//! Initializes the LED Matrix display.
ExampleDisplayTask::ExampleDisplayTask(Facilities::MeshNetwork& mesh) :
   Task(1 , TASK_FOREVER, std::bind(&ExampleDisplayTask::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
   m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);
   m_lmd.clear();
   m_lmd.display();
   init_figures();
   m_mesh.onReceive(std::bind(&ExampleDisplayTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
}

//! Update display
void ExampleDisplayTask::execute()
{
    static int currentId=-1;
    static int imageId;
    static int count = 0;
    if(m_mesh._internalId!=currentId)
    {
        currentId=m_mesh._internalId;
    }
    
    if (m_image_index != imageId)
    {   

        if (count < 1000)
        {
            imageId = BLANK;
            count ++;
        }
            
        else{

            imageId= m_image_index;
            count=0;
        }
    }

  /* m_lmd.clear();
   for (int x=0;x<31;x++)
    {
        m_lmd.setColumn(x,image[0][x]);
    }
   m_lmd.display();*/

   
    if (currentId >= 0) {
        m_lmd.clear();
        for (int cx = 0; cx < 32; cx++) {
            for (int cy = 0; cy < 8; cy++) {
                m_lmd.setPixel(cx, cy, m_image[imageId][cx][currentId * 8 + cy]);
            }
        }
        m_lmd.display();
    }
}

void ExampleDisplayTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
    if (msg.length() == 2) {
        // I0 I1 I2 ...
        if (msg[0] == 'I') {
            m_image_index = msg[1] - '0';
        }
    }
   //MY_DEBUG_PRINTLN("Received data in ExampleDisplayTask");
   
}

} // namespace Tasks

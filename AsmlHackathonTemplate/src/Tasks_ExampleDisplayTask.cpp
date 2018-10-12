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


const int ExampleDisplayTask::LEDMATRIX_WIDTH = 7;
const int ExampleDisplayTask::LEDMATRIX_HEIGHT = 31;
const int ExampleDisplayTask::LEDMATRIX_SEGMENTS = 4;
const int ExampleDisplayTask::LEDMATRIX_INTENSITY = 5;
const int ExampleDisplayTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleDisplayTask::POLL_DELAY_MS = 100;

int m_cross[32][32];
int m_full[32][32];

void init_figures() {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            if (i == j || i == 31 - j) {
                m_cross[i][j] = 1;
            } else {
                m_cross[i][j] = 0;
            }
        }
    }

    double mx = 15.5;
    double my = 15.5;
    double r = 15.8;
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            double dx = abs(x - mx);
            double dy = abs(y - my);
            if (dx * dx + dy * dy <= r*r) {
                m_full[x][y] = 1;
            } else {
                m_full[x][y] = 0;
            }
        }
    }
}

uint8_t image[4][8][32];
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

   init_figures();
   m_mesh.onReceive(std::bind(&ExampleDisplayTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));

}

int my_part = 1; // 0, 1, 2 or 3
int cnt = 0;

//! Update display
void ExampleDisplayTask::execute()
{
    cnt++;
    if (cnt % 10 == 0) {
        my_part = (my_part+1) % 4;
    }
    m_lmd.clear();
    for (int cx = 0; cx < 32; cx++) {
       for (int cy = 0; cy < 8; cy++) {
           m_lmd.setPixel(cx, cy, m_full[cx][my_part * 8 + cy]);
        }
    }
    m_lmd.display();
}

void ExampleDisplayTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   //MY_DEBUG_PRINTLN("Received data in ExampleDisplayTask");
   
}

} // namespace Tasks

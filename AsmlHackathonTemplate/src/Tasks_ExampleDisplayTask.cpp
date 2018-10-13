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
const int ExampleDisplayTask::LEDMATRIX_INTENSITY = 10;
const int ExampleDisplayTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleDisplayTask::POLL_DELAY_MS = 100;

char m_image[6][32][32];
int m_image_index = 3;
const int SUPERMAN = 1;
const int CROSS = 0;
const int MAX_DISK = 2;
const int CIRCLE = 3;
const int DISK_5 = 4;
const int FACE = 5;

const int SuperManInd[3][32] = {{9,10,11,11,11,12,12,12,12,13,13,13,13,
     14,14,14, 15,15, 16, 17,17, 18,18,18, 19, 20, 21,21, 22,22, 23,24},
                               {9,8,7,10,24,6,9,22,26, 5,9,22,27,
     6,9,26, 7,25, 8, 9,9, 10,13,20, 11, 12, 13,19, 14,18, 15, 16},
                      {24,25,8,22,26,7,11,24,27, 6,11,24,28,
     7,21,27, 22,26, 25, 10,24, 11,14,23, 22, 21, 14,20, 15, 19, 18, 17}};

void fill_face_circle(double mx, double my, double r, int how) {
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            double dx = abs(x - mx);
            double dy = abs(y - my);
            if (dx*dx + dy*dy <= r*r) {
                m_image[FACE][x][y] = how;
            }
        }
    }
}
 
void fill_face() {
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            m_image[FACE][x][y] = 0;
        }
    }
    double mx = 15.5;
    double my = 15.5;
    double r = 15.955;
    fill_face_circle(mx, my, r, 1);
    fill_face_circle(8, my - 4.5, 2.5, 0);
    fill_face_circle(8, my + 4.5, 2.5, 0);
    for (int x = 20; x < 22; x++) {
        int q = 10;
        for (int y = q; y < 32 - q; y++) {
            m_image[FACE][x][y] = 0;
        }
    }
    for (int x = 22; x < 24; x++) {
        int q = 12;
        for (int y = q; y < 32 - q; y++) {
            m_image[FACE][x][y] = 0;
        }
    }
}

void init_figures() {
     for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            m_image[SUPERMAN][i][j] = 0;
        }
    }
    for (int i = 0; i < 32; i++) {
        for (int j = SuperManInd[1][i]; j <= SuperManInd[2][i]; j++) {
                m_image[SUPERMAN][SuperManInd[0][i]][j-1] = 1;
        }
    }
    fill_face();
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
    _animationCount=3000;
}

void ExampleDisplayTask::UpdateImage(int currentId,int imageId)
{
    m_lmd.clear();
    for (int cx = 0; cx < 32; cx++) {
          for (int cy = 0; cy < 8; cy++) {
             m_lmd.setPixel(cx, cy, m_image[imageId][cx][currentId * 8 + cy]);
         }
    }
        m_lmd.display();
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
        if (count < _animationCount)
        {
            if (count <= (_animationCount/2))
            {
                if (count % (_animationCount/20)==0)
                    m_lmd.setIntensity(10-(count/(_animationCount/20)));
            }
            else
            {
                if (count == (_animationCount/2)+1)
                    UpdateImage(currentId,m_image_index);
                if ((count % (_animationCount/20))==0)
                    m_lmd.setIntensity((count/(_animationCount/20))-10);
            }
            count ++;
        }
        else{
            imageId= m_image_index;
            count=0;
        }
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

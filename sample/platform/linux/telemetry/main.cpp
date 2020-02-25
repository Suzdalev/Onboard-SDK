/*! @file telemetry/main.cpp
 *  @version 3.3
 *  @date Jun 05 2017
 *
 *  @brief
 *  main for Telemetry API usage in a Linux environment.
 *  Shows example usage of the new data subscription API.
 *
 *  @Copyright (c) 2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "telemetry_sample.hpp"
#include <thread>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <bcm2835.h>
#define PIN RPI_GPIO_P1_07
#define PITCH_CAL 0
#define YAW_CAL -90
#define CONTROL_MULTIPLIER 0.8


using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;




int pitchGV = 1024;
int rwbGV = 1024;
int btnC1GV = 1024;
int btnC2GV = 1024;
int camGV = 1024;

void PPMpulse(){
  bcm2835_gpio_write(PIN, HIGH);
  usleep(500);
  bcm2835_gpio_write(PIN, LOW);
}
int Btn(int& btn){
  if(btn>1100) return 1;
  return 0;
}
void send_control(int& pitchGV, int& rwbGV, int& btnC1GV, int& btnC2GV, int& camGV)
{
  string buffer = "";
  while(true){
   buffer = "p" + to_string(pitchGV + 476 + PITCH_CAL);
   bcm2835_i2c_write((char*)buffer, buffer.length());
   usleep(100);
   
   buffer = "y" + to_string(1500 - 400*(btnC1GV-364)/1320 +  400*(btnC2GV-364)/1320 +YAW_CAL);
   bcm2835_i2c_write((char*)buffer, buffer.length());
   usleep(100);
   
   buffer = "r" + to_string(1100 + 800*(rwbGV - 364)/1320);
   bcm2835_i2c_write((char*)buffer, buffer.length());
   usleep(100);
   
   buffer = "z" + to_string(1100 + 800*(camGV - 364)/1320);
   bcm2835_i2c_write((char*)buffer, buffer.length());
   usleep(100);
   
  }

}

int main(int argc, char** argv)
{

  // Setup OSDK.
  std::cout << "\nwait 5 sec before vehicle init \n";
  usleep(5000000);
  std::cout << "\n start init\n";
  LinuxSetup linuxEnvironment(argc, argv);
  Vehicle*   vehicle = NULL;
  bcm2835_init();
  bcm2835_i2c_begin();
  bcm2835_i2c_setSlaveAddress(0x04);
  do{ 
      
      vehicle = linuxEnvironment.getVehicle();
      if (vehicle == NULL)
      {
        std::cout << "Vehicle not initialized, exiting.\n";
        sleep(1);
      }
  }while(vehicle  == NULL);
  

   std::thread telemetry_thr(subscribeToData, vehicle, std::ref(pitchGV), std::ref(rwbGV), std::ref(btnC1GV), std::ref(btnC2GV), std::ref(camGV));
  telemetry_thr.detach();
  std::thread control_thr(send_control, std::ref(pitchGV), std::ref(rwbGV), std::ref(btnC1GV), std::ref(btnC2GV), std::ref(camGV));
  control_thr.detach();
  
  while(true){
          std::cout << "\n[main cycle] pitchGV = " << pitchGV << "\n" << std::endl;
          std::cout << "\n[main cycle] PWM = " << pitchGV + PITCH_CAL -24  << "\n" << std::endl;
          usleep(10000000);
  }
     
  bcm2835_close();
  return 0;
}

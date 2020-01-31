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
#define PIN RPI_GPIO_P1_1


using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;




int channelGV = 0;

void send_control(int& channelGV)
{
  
  while(true){
   
    bcm2835_gpio_write(PIN, 1);
    usleep(channelGV+476);
    bcm2835_gpio_write(PIN, 0);
    usleep(20000);
  }

}

int main(int argc, char** argv)
{

  // Setup OSDK.
  LinuxSetup linuxEnvironment(argc, argv);
  Vehicle*   vehicle = NULL;
  bcm2835_init();
  bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
  do{

      vehicle = linuxEnvironment.getVehicle();
      if (vehicle == NULL)
      {
        std::cout << "Vehicle not initialized, exiting.\n";
        sleep(1);
      }
  }while(vehicle  == NULL);
  

   std::thread telemetry_thr(subscribeToData, vehicle, std::ref(channelGV));
  telemetry_thr.detach();
  std::thread sbus_thr(send_control, std::ref(channelGV));
  sbus_thr.detach();

  while(true){
          std::cout << "\n[main cycle] channelGV = " << channelGV << "\n" << std::endl;
          usleep(100000);
  }
     

  return 0;
}

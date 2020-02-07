/*! @file telemetry_sample.cpp
 *  @version 3.3
 *  @date Jun 05 2017
 *
 *  @brief
 *  Telemetry API usage in a Linux environment.
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

#include <dji_telemetry.hpp>
#include "telemetry_sample.hpp"
#include  <signal.h>
#include  <stdlib.h>

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;





bool
subscribeToData(Vehicle* vehicle, int& pitchGV, int& rwbGV, int& btnC1GV, int& btnC2GV)
{
  int responseTimeout = 1;
  // We will subscribe to six kinds of data:
  // 1. Flight Status at 1 Hz
  // 2. Fused Lat/Lon at 10Hz
  // 3. Fused Altitude at 10Hz
  // 4. RC Channels at 50 Hz
  // 5. Velocity at 50 Hz
  // 6. Quaternion at 200 Hz

  // Please make sure your drone is in simulation mode. You can fly the drone
  // with your RC to
  // get different values.

  // Telemetry: Verify the subscription
  ACK::ErrorCode subscribeStatus;
  subscribeStatus = vehicle->subscribe->verify(responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    return false;
  }



  // Package 2: Subscribe to RC Channel and Velocity at freq 50 Hz
  int     pkgIndex                  = 0;
  int     freq                      = 50;
  TopicName topicList50Hz[] = {TOPIC_RC_FULL_RAW_DATA };
  int numTopic                  = sizeof(topicList50Hz) / sizeof(topicList50Hz[0]);
  bool enableTimestamp           = false;

  bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    pkgIndex, numTopic, topicList50Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }
  subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    // Cleanup before return
    vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
    return false;
  }


  // Wait for the data to start coming in.
  sleep(1);

  // Get all the data once before the loop to initialize vars
 
  
 
  
  TypeMap<TOPIC_RC_FULL_RAW_DATA>::type rcFuncFull;
  

  // Print in a loop for 2 sec
  while (true)
  {
    
   
    
    rcFuncFull = vehicle->subscribe->getValue<TOPIC_RC_FULL_RAW_DATA>();
    
    
    //std::cout << "Left RC wheel =" << rcFuncFull.lb2.leftWheel << "\n";
    pitchGV = rcFuncFull.lb2.leftWheel;
    rwbGV = rcFuncFull.lb2.rightWheelButton;
    btnC1GV = rcFuncFull.lb2.rcC1;
    btnC2GV = rcFuncFull.lb2.rcC2;
    //std::cout << "-------\n\n";
    usleep(50000);
    
  }


 
  vehicle->subscribe->removePackage(0, responseTimeout);


  return true;
}

void     INThandler(int);
static bool keepRunning = true;



void  INThandler(int sig)
{
    keepRunning = false;
}
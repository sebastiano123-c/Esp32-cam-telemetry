
#pragma once

#include "telemetry.h"

void startCameraServer();

//              (ROLL)
extern float PID_P_GAIN_ROLL;                    //Gain setting for the roll P-controller (1.3)
extern float PID_I_GAIN_ROLL;                  //Gain setting for the roll I-controller  (0.0002)
extern float PID_D_GAIN_ROLL;                   //Gain setting for the roll D-controller (10.0)
                                             
//              (PITCH)                                             
extern float PID_P_GAIN_PITCH;          //Gain setting for the pitch P-controller
extern float PID_I_GAIN_PITCH;          //Gain setting for the pitch I-controller
extern float PID_D_GAIN_PITCH;          //Gain setting for the pitch D-controller
                                              
//              (YAW)                                             
extern float PID_P_GAIN_YAW;                      //Gain setting for the pitch P-controller. (2.0)
extern float PID_I_GAIN_YAW;                     //Gain setting for the pitch I-controller. (0.04)
extern float PID_D_GAIN_YAW;                      //Gain setting for the pitch D-controller. (0.0)
//              (ALTITUDE)                                                                                          
extern float PID_P_GAIN_ALTITUDE;                     //Gain setting for the pitch P-controller. (2.0)
extern float PID_I_GAIN_ALTITUDE;                     //Gain setting for the pitch I-controller. (0.04)
extern float PID_D_GAIN_ALTITUDE;                      //Gain setting for the pitch D-controller. (0.0)
                                             
extern float rollAngle;
extern float pitchAngle;
extern float flightMode;
extern float batteryPercentage;
extern float altitudeMeasure;
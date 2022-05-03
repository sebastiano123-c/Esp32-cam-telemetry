/**
 * @file telemetry.cpp
 * @author @sebastiano123-c
 * @brief telemetry routines
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "telemetry.h"

float dataController[dataControllerSize];

String dataTransfer[dataTransferSize];

// serial
HardwareSerial SUART(1);

/**
 * @brief Starts the Serial for UART com.
 * 
 */
void beginUARTCOM(){
    SUART.begin(BAUD_RATE, SERIAL_8N1, 3, 1); // RX, TX
    delay(40);
}


/**
 * @brief Write data to DroneIno via UART com.
 * 
 */
void writeDataTransfer(){

  // print in csv format
  const char * stringToPrint = "";
  static char charToPrint[500];
  char * ptr = charToPrint;

  ptr += sprintf(ptr, "<");
  ptr += sprintf(ptr, "%.6f,", PID_P_GAIN_ROLL);
  ptr += sprintf(ptr, "%.6f,", PID_I_GAIN_ROLL);
  ptr += sprintf(ptr, "%.6f,", PID_D_GAIN_ROLL);
  ptr += sprintf(ptr, "%.6f,", PID_P_GAIN_YAW);
  ptr += sprintf(ptr, "%.6f,", PID_I_GAIN_YAW);
  ptr += sprintf(ptr, "%.6f,", PID_D_GAIN_YAW);
  ptr += sprintf(ptr, "%.6f,", GYROSCOPE_ROLL_FILTER);
  ptr += sprintf(ptr, "%.6f,", GYROSCOPE_ROLL_CORR);
  ptr += sprintf(ptr, "%.6f,", GYROSCOPE_PITCH_CORR);
  ptr += sprintf(ptr, "%.6f,", PID_P_GAIN_ALTITUDE);
  ptr += sprintf(ptr, "%.6f,", PID_I_GAIN_ALTITUDE);
  ptr += sprintf(ptr, "%.6f", PID_D_GAIN_ALTITUDE);
  ptr += sprintf(ptr, ">");

  ptr += 0;

  stringToPrint = (const char*)charToPrint;
  SUART.println(stringToPrint);

}


/**
 * @brief Read the data coming from DroneIno via UART.
 * 
 * @bug Not stable communication: sometimes variables jump.
 * 
 */
void readDataTransfer(){
    
    if(SUART.available() > 0){
        // declair index array
        int indices[dataTransferSize - 1];
        String str = "";
        
        // read from serial
        //Serial.printf("I'm reading...\n");
        str = SUART.readStringUntil('\n');


        // find position of the last <
        int posStart = str.lastIndexOf('<') + 1;
        
        // find positions of ","
        int i = 0;
        indices[i] = str.indexOf(',', posStart);
        for( i = 1; i < dataTransferSize - 1; i++){
            indices[i] = str.indexOf(',', indices[i-1]+1);
        }
        
        // if no index is found the string is invalid, thus do nothing
        if (indices[dataTransferSize - 2] == -1){

            // append the string to log file 
            // appendFile(SD_MMC, "/debug/log.txt", ("Invalid incoming string: "+str+"\n").c_str());

            // exit
            return;
        }

        // substring the data and convert it to floats
        i = 0;
        dataTransfer[i] = str.substring(posStart, indices[i]);
        for( i = 1; i < dataTransferSize - 1; i++){
            dataTransfer[i] = str.substring(indices[i-1] + 1, indices[i]);
        }
        dataTransfer[dataTransferSize - 1] = str.substring(indices[dataTransferSize - 2] + 1 );
        
        // fill data structure after receiving
        rollAngle         = dataTransfer[0].toFloat();
        pitchAngle        = dataTransfer[1].toFloat();
        flightMode        = dataTransfer[2].toInt();
        batteryPercentage = dataTransfer[3].toFloat();
        altitudeMeasure   = dataTransfer[4].toFloat();

        rollTrim          = dataTransfer[5].toFloat();
        pitchTrim         = dataTransfer[6].toFloat();
        yawTrim           = dataTransfer[7].toFloat();
        throttleTrim      = dataTransfer[8].toFloat();

        temperature       = dataTransfer[9].toFloat();

        latitude          = dataTransfer[10].toFloat();
        longitude         = dataTransfer[11].toFloat();
        timeUTC           = dataTransfer[12].c_str();
        
    }
}

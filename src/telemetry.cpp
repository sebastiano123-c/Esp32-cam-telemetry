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


// UART com
const int numChars = 1000;
char receivedChars[numChars];
char tempChars[numChars];                           // temporary array for use when parsing
boolean newData = false;


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
    static char charToPrint[1000];
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
    ptr += sprintf(ptr, ">\n");

    ptr += 0;

    stringToPrint = (const char*)charToPrint;

    SUART.print(stringToPrint);

}


/**
 * @brief Decode the incoming message in the form < a, b, ... >.
 * 
 */
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static int ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (SUART.available() > 0 && newData == false) {
       
        rc = SUART.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}


/**
 * @brief Converts the message < a, b, ... > into variables.
 * 
 */
void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    // fill data structure after receiving
    strtokIndx = strtok(tempChars,","); rollAngle         = strtokIndx;
    strtokIndx = strtok(NULL, ",");     pitchAngle        = strtokIndx;
    strtokIndx = strtok(NULL, ",");     flightMode        = strtokIndx;
    strtokIndx = strtok(NULL, ",");     batteryPercentage = strtokIndx;
    strtokIndx = strtok(NULL, ",");     altitudeMeasure   = strtokIndx;
    strtokIndx = strtok(NULL, ",");     rollTrim          = strtokIndx;
    strtokIndx = strtok(NULL, ",");     pitchTrim         = strtokIndx;
    strtokIndx = strtok(NULL, ",");     yawTrim           = strtokIndx;
    strtokIndx = strtok(NULL, ",");     throttleTrim      = strtokIndx;
    strtokIndx = strtok(NULL, ",");     temperature       = strtokIndx;
    strtokIndx = strtok(NULL, ",");     latitude          = strtokIndx;
    strtokIndx = strtok(NULL, ",");     longitude         = strtokIndx;
    strtokIndx = strtok(NULL, ",");     timeUTC           = strtokIndx;

}


/**
 * @brief Read the incoming data from the DroneIno as telemetry.
 * 
 */
void readDataTransfer() {
    recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();

        // update the data file log
        if (isConnectedSD) appendFile(SD_MMC, logFileName, receivedChars+'\n');

        newData = false;
    }
}

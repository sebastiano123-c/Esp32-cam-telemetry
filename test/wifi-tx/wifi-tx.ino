/**
 * @file wifi-tx.ino
 * @author @sebastiano123-c
 * 
 * @brief this sketch is a useful transmitter example.
 *    1- UART transmitter board pins are (TXb, RXb) and UART esp32-cam pins are (TX0, RX0).
 *      Connect:
 *        TXb -> RX0
 *        RXb -> TX0
 *        GND -> GND
 *    2- This sketch governs the tx board with two functions:
 *        void writeDataTransfer();    writes telemetry parameters using uart pins to the esp32-cam
 *        void readDataTransfer();     read PID parameters modified on the client and sent back to the transmitter
 *    3- On the esp32-cam side the latter functions are caught by:
 *        void readDataTransfer();     read telemetry parameters from the transmitter
 *        void writeDataTransfer();    writes PID parameters to the transmitter
 * 
 *  @param delayTime can be adjusted by the user to simulate the drone loop time
 *  @param dataTransferSize   elements number of telemetry data to sent
 *  @param dataControllerSize elements number of PID parameters to receive
 * 
 * @todo Increment in some way the timeUTC variable (using for instance the time.h lib, how cares if starts in 1980...)
 * 
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 

 * 
 */
#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial SUART(1); 

int delayTime = 200; // ms
int sendTelemetryLoopN = 0;

// UART com
const int numChars = 1000;
char receivedChars[numChars];
char tempChars[numChars];                           // temporary array for use when parsing
boolean newData = false;

// PID:
//              (ROLL)
float PID_P_GAIN_ROLL            = 1.3;                     //Gain setting for the roll P-controller (1.3)
float PID_I_GAIN_ROLL            = 0.001;                   //Gain setting for the roll I-controller  (0.0002)
float PID_D_GAIN_ROLL            = 8.0;                     //Gain setting for the roll D-controller (10.0)

//              (PITCH)
float PID_P_GAIN_PITCH           = PID_P_GAIN_ROLL;          //Gain setting for the pitch P-controller
float PID_I_GAIN_PITCH           = PID_I_GAIN_ROLL;          //Gain setting for the pitch I-controller
float PID_D_GAIN_PITCH           = PID_D_GAIN_ROLL;          //Gain setting for the pitch D-controller

//              (YAW)
float PID_P_GAIN_YAW             = 2.2;                      //Gain setting for the pitch P-controller. (2.0)
float PID_I_GAIN_YAW             = 0.01;                     //Gain setting for the pitch I-controller. (0.04)
float PID_D_GAIN_YAW             = 0.0;                      //Gain setting for the pitch D-controller. (0.0)

//              (YAW)
float PID_P_GAIN_ALTITUDE        = 2.2;                      //Gain setting for the pitch P-controller. (2.0)
float PID_I_GAIN_ALTITUDE        = 0.01;                     //Gain setting for the pitch I-controller. (0.04)
float PID_D_GAIN_ALTITUDE        = 0.01;                     //Gain setting for the pitch D-controller. (0.0)

// TELEMETRY
float angleRoll = 10.3;
float anglePitch = 1.2;
uint8_t flightMode = 3;
float batteryPercentage = 8.3;
float altitudeMeasure = 130.4;
int controllerRollInput = 1500;
int controllerPitchInput = 1500;
int controllerYawInput = 2000;
int controllerThrottleInput = 1000;
float latitudeGPS = 52.44;
float longitudeGPS = 13.34;
const char* timeUTC = "12:02:12.33";

void setup()
{
  Serial.begin(115200);
  SUART.begin(115200, SERIAL_8N1, 26, 25);
  delay(30);
}

void loop()
{
  // simulate some telemetry parameter updates
  angleRoll += 0.01;
  anglePitch += 0.013;
  latitudeGPS += latitudeGPS/1e7;
  longitudeGPS += longitudeGPS/1e7;


  switch(sendTelemetryLoopN){
    case 1:                     
      writeDataTransfer();
      sendTelemetryLoopN = 0;
      break;
    default:
      sendTelemetryLoopN++;
  }
  

  if(SUART.available()){
    readDataTransfer();
  }
  
 delay(delayTime);
}


/**
 * @brief writes telemetry parameters using UART communication
 * 
 */
void writeDataTransfer(){

  Serial.printf("I'm sending...\n");

  // fill data structure before send
  const char* stringToPrint = "";
  static char charSource[500];
  char *ptr = charSource;

  ptr += sprintf(ptr, "<%.6f,", angleRoll);
  ptr += sprintf(ptr, "%.6f,", anglePitch);
  ptr += sprintf(ptr, "%u,", flightMode);
  ptr += sprintf(ptr, "%f,", batteryPercentage);
  ptr += sprintf(ptr, "%f,", altitudeMeasure);
  ptr += sprintf(ptr, "%i,", controllerRollInput);
  ptr += sprintf(ptr, "%i,", controllerPitchInput);
  ptr += sprintf(ptr, "%i,", controllerYawInput);
  ptr += sprintf(ptr, "%i,", controllerThrottleInput);
  ptr += sprintf(ptr, "%f,", latitudeGPS);
  ptr += sprintf(ptr, "%f,", longitudeGPS);
  ptr += sprintf(ptr, "%s>", timeUTC);
  ptr += 0;

  stringToPrint = (const char*)charSource;
  
  // print in csv format
  SUART.println(stringToPrint);

  // print
  Serial.println(stringToPrint);
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
 * @brief COnverts the message < a, b, ... > into variables.
 * 
 */
void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    // fill data structure after receiving
    strtokIndx = strtok(tempChars,","); PID_P_GAIN_ROLL     = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_I_GAIN_ROLL     = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_D_GAIN_ROLL     = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_P_GAIN_PITCH    = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_I_GAIN_PITCH    = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_D_GAIN_PITCH    = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_P_GAIN_YAW      = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_I_GAIN_YAW      = atof(strtokIndx);
    strtokIndx = strtok(NULL, ",");     PID_D_GAIN_YAW      = atof(strtokIndx);

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
        newData = false;
    }
}
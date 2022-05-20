/**
 * @file SD.cpp
 * @brief SD routines
 * @link https://gist.github.com/youjunjer/b70b6e54ae7201a46387b8e73894ba51
 * @endlink
 */

#include "camSD.h"

const char *configFilePath = "/src/config.txt";
const char *flightDataPath = "/data/flightData";
const char *logFileName = "";

static char todayLogChar[200];
int numberOfDataFiles = 0;

// List dir in SD card
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  // Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    // Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    // Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      // Serial.print("  DIR : ");
      // Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      // Serial.print("  FILE: ");
      // Serial.print(file.name());
      // Serial.print("  SIZE: ");
      // Serial.println(file.size());
    }
    numberOfDataFiles += 1;
    file = root.openNextFile();
  }
  // Serial.printf("Im out");
}

// Create a dir in SD card
void createDir(fs::FS &fs, const char *path) {
  // Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    // Serial.println("Dir created");
  } else {
    // Serial.println("mkdir failed");
  }
}

// Write a file in SD card
void writeFile(fs::FS &fs, const char *path, const char *message) {
  // Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);

  if (!file) {
    // Serial.println("Failed to open file for writing");
    return;
  }

  // fwrite(fb->buf, 1, fb->len, file);
  if (file.print(message)) {
    // Serial.println("File written");
  } else {
    // Serial.println("Write failed");
  }
}

// Write a file in SD card
void appendFile(fs::FS &fs, const char *path, const char *message) {
  // Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);

  if (!file) {
    // Serial.println("Failed to open file for writing");
    file.close();
    return;
  }

  if (file.print(message)) {
    // Serial.println("File written");
  } else {
    // Serial.println("Write failed");
  }

  file.close();
}

/**
 * @brief Read the config flight file and initialize the PID
 *
 * @param fs the file object
 */
void readConfigFile(fs::FS &fs) {
  // open the file
  File file = fs.open(configFilePath);

  // read the config.txt file
  if (file) {
    // initialize the PID values with the one read on the SD file
    //              (ROLL)
    PID_P_GAIN_ROLL =
        file.parseFloat();  // Gain setting for the roll P-controller (1.3)
    PID_I_GAIN_ROLL =
        file.parseFloat();  // Gain setting for the roll I-controller  (0.0002)
    PID_D_GAIN_ROLL =
        file.parseFloat();  // Gain setting for the roll D-controller (10.0)

    //              (PITCH)
    PID_P_GAIN_PITCH =
        PID_P_GAIN_ROLL;  // Gain setting for the pitch P-controller
    PID_I_GAIN_PITCH =
        PID_I_GAIN_ROLL;  // Gain setting for the pitch I-controller
    PID_D_GAIN_PITCH =
        PID_D_GAIN_ROLL;  // Gain setting for the pitch D-controller

    //              (YAW)
    PID_P_GAIN_YAW =
        file.parseFloat();  // Gain setting for the pitch P-controller. (2.0)
    PID_I_GAIN_YAW =
        file.parseFloat();  // Gain setting for the pitch I-controller. (0.04)
    PID_D_GAIN_YAW =
        file.parseFloat();  // Gain setting for the pitch D-controller. (0.0)

    //              (GYROSCOPE)
    GYROSCOPE_ROLL_FILTER =
        file.parseFloat();  // read your gyroscope data after the calibration,
                            // try different values and choose the best one
    GYROSCOPE_ROLL_CORR =
        file.parseFloat();  // (0.) after set GYROSCOPE_ROLL_FILTER, put here
                            // the angle roll you read enabling DEBUG
    GYROSCOPE_PITCH_CORR =
        file.parseFloat();  // (-1.65.) after set GYROSCOPE_PITCH_FILTER, put
                            // here the angle pitch you read enabling DEBUG

    //              (ALTITUDE)
    PID_P_GAIN_ALTITUDE =
        file.parseFloat();  // Gain setting for the pitch P-controller. (2.0)
    PID_I_GAIN_ALTITUDE =
        file.parseFloat();  // Gain setting for the pitch I-controller. (0.04)
    PID_D_GAIN_ALTITUDE =
        file.parseFloat();  // Gain setting for the pitch D-controller. (0.0)

    // Serial.print(PID_P_GAIN_ROLL);
    // Serial.print(PID_I_GAIN_ROLL);
    // Serial.print(PID_D_GAIN_ROLL);
    // Serial.print(PID_P_GAIN_YAW);
    // Serial.print(PID_I_GAIN_YAW);
    // Serial.print(PID_D_GAIN_YAW);
    // Serial.print(GYROSCOPE_ROLL_FILTER);
    // Serial.print(GYROSCOPE_ROLL_CORR);
    // Serial.print(GYROSCOPE_PITCH_CORR);
    // Serial.print(PID_P_GAIN_ALTITUDE);
    // Serial.print(PID_I_GAIN_ALTITUDE);
    // Serial.print(PID_D_GAIN_ALTITUDE);
    // Serial.println();

    // update DroneIno PID parameters
    isChangedPID = 1;
  }
}

void updateConfigFile(fs::FS &fs) {
  File file = fs.open("/src/config.txt", FILE_WRITE);
  if (file) {
    static char message[1024];

    char *p = message;
    *p++ = ' ';

    // pid
    p += sprintf(p, "%.6f\n", PID_P_GAIN_ROLL);
    p += sprintf(p, "%.6f\n", PID_I_GAIN_ROLL);
    p += sprintf(p, "%.6f\n", PID_D_GAIN_ROLL);
    p += sprintf(p, "%.6f\n", PID_P_GAIN_YAW);
    p += sprintf(p, "%.6f\n", PID_I_GAIN_YAW);
    p += sprintf(p, "%.6f\n", PID_D_GAIN_YAW);
    p += sprintf(p, "%.6f\n", GYROSCOPE_ROLL_FILTER);
    p += sprintf(p, "%.6f\n", GYROSCOPE_ROLL_CORR);
    p += sprintf(p, "%.6f\n", GYROSCOPE_PITCH_CORR);
    p += sprintf(p, "%.6f\n", PID_P_GAIN_ALTITUDE);
    p += sprintf(p, "%.6f\n", PID_I_GAIN_ALTITUDE);
    p += sprintf(p, "%.6f\n", PID_D_GAIN_ALTITUDE);

    *p++ = 0;

    file.print(message);
    file.close();
  }
}

/**
 * @brief stores the flight data
 *
 */
void writeDataLogFlight(fs::FS &fs) {
  File file = SD_MMC.open(logFileName, FILE_APPEND);
  if (file) {
    if (rollTrim != NULL && pitchTrim != NULL && yawTrim != NULL &&
        throttleTrim != NULL) {
      static char stringToPrint[1024];

      char *ptr = stringToPrint;
      // *ptr++ = ' ';

      // telemetry
      ptr += sprintf(ptr, "%s,", rollAngle);
      ptr += sprintf(ptr, "%s,", pitchAngle);
      ptr += sprintf(ptr, "%s,", flightMode);
      ptr += sprintf(ptr, "%s,", batteryPercentage);
      ptr += sprintf(ptr, "%s,", altitudeMeasure);

      ptr += sprintf(ptr, "%s,", rollTrim);
      ptr += sprintf(ptr, "%s,", pitchTrim);
      ptr += sprintf(ptr, "%s,", yawTrim);
      ptr += sprintf(ptr, "%s,", throttleTrim);

      ptr += sprintf(ptr, "%s,", latitude);
      ptr += sprintf(ptr, "%s,", longitude);
      ptr += sprintf(ptr, "%s", timeUTC);

      *ptr++ = '\n';
      *ptr++ = 0;

      file.print(stringToPrint);
    }

    file.close();
  }
}

void setupSD() {
  // Serial.begin(115200);
  //  Serial.println("SDcard Testing....");

  // if( !SD_MMC.begin() ) { // fast 4bit mode
  if (!SD_MMC.begin("/sdcard", true)) {  // slow 1bit mode
    isConnectedSD = 0;
  } else {
    isConnectedSD = 1;

    // create two fundamental folders
    if (!SD_MMC.exists("/data"))
      createDir(
          SD_MMC,
          "/data");  // there will be saved flight data, images maybe videos

    // data subfolders
    if (!SD_MMC.exists(flightDataPath)) createDir(SD_MMC, flightDataPath);
    if (!SD_MMC.exists("/data/images")) createDir(SD_MMC, "/data/images");
    if (!SD_MMC.exists("/data/videos")) createDir(SD_MMC, "/data/videos");

    // check how many log there are
    listDir(SD_MMC, flightDataPath, 0);

    // write file name
    char *sptr = todayLogChar;
    // *sptr++ = '';
    sptr += sprintf(sptr, "%s", flightDataPath);
    sptr += sprintf(sptr, "/flight_%i.csv", numberOfDataFiles + 1);
    *sptr++ = 0;
    logFileName = (const char *)todayLogChar;
    numberOfDataFiles = 0;

    // create log file for this session
    writeFile(SD_MMC, logFileName, flightDataHeaderCSV);
  }
}

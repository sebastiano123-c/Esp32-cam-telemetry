<!-- # Esp32-cam-telemetry
Create a telemetry system for your drone using esp32-cam. -->
<!-- <pre align=center>
   
         ---------------       
         |      O      |          
         |             |   
         |             |          
         |  ESP32-CAM  |          
         ---------------       
  
</pre> -->

<!-- title -->
<h1 align="center">
  <b> 
    Esp32-cam-telemetry
  </b>
</h1>
<h3 align="center"> 
  <i>
    Create a telemetry system for your drone using esp32-cam.
  </i>
</h3>
<br>

<!-- badges -->
<p align="center">
  <img src="https://img.shields.io/badge/IDE-PlatformIO-orange" />
  <img src="https://img.shields.io/badge/PIO core-5-red" />
  <img src="https://img.shields.io/badge/cpp-11-blue" /> 
</p>

# Usage
Clone this repo
<pre><code>git clone git@github.com:sebastiano123-c/DroneIno.git
</code></pre>
or download the .zip file.

# Description
<!-- Try to build your own telemetry system starting from the code in this repository. -->
This code is intended to give a starting point for your telemetry system.
Based on the esp32-cam, this code allow you to:
- see telemetry data of the drone;
- send parameters to the drone;
- stream the camera video;
- store flight data on a SD card.

For example, I use this system on my [DroneIno](https://github.com/sebastiano123-c/DroneIno) project.

# Wires
The principle of working is: the drone board is connected via UART (RX/TX pins) to the esp32-cam;
Using esp32-cam wifi, it is generated a wifi access point (AP).
The connections are shown in the figure below.

<pre align=center>
 transmitter             esp32-cam                     
|----------|           |----------|                    
|       TX |-----------| RX0      |                    
|       RX |-----------| TX0      |  => wifi => web app
|      GND |-----------|GND0      |                    
|----------|           |----------|                    
</pre>

# Examples
In the [wifi-tx](https://github.com/sebastiano123-c/Esp32-cam-telemetry/test/wifi-tx/wifi-tx.ino) example, I use an esp32 board as transmitter.
If you don't use a esp32 board, make the proper changes on the code and on the connections.

Connect the RX1 (GPIO 26) of the esp32 to the TX0 (GPIO 1) of esp32-cam, and TX1 (GPIO 25) of the esp32 to the RX0 (GPIO 3) of esp32-cam.

Upload the [wifi-tx](https://github.com/sebastiano123-c/Esp32-cam-telemetry/test/wifi-tx/wifi-tx.ino) sketch on your transmitter board.
Then, upload the [main](https://github.com/sebastiano123-c/Esp32-cam-telemetry/blob/master/src/main.cpp) sketch on your esp32-cam.

# Wifi connection
After connecting to the network using the password `esp32cam`, dial in your browser's search bar `192.168.4.1`.
That's it.

# Telemetry
In the [main](https://github.com/sebastiano123-c/Esp32-cam-telemetry/blob/master/src/main.cpp) sketch, I use consider the following variables for telemetry: 
- roll
- pitch
- flight mode
- altitude
- battery
- four channels of the controller (_e.g._ roll, pitch, throttle, yaw)
- GPS latitude, longitude and UTC time.
The browser update rate is about 100ms.

# Talk with the drone
PID variables are easily set in the browser in the toggling menu.
When you change a certain PID parameter, the latter is sent via WiFi to the esp32-cam, and then back to the transmitter board via UART. 

# Streaming video
Press the button `start video` in the toggling menu and the live streaming will appear showing also the status of the telemetry.
You can set the video parameters in the toggling menu. 

# SD card
To better appreciate this telemetry system, I recommend to use also a SD card.
SD card _must_ be formatted in FAT32.

Take the SD card, and create a directory and a file called `/src/config.txt` in which you write _only_ the initial PID parameters.
When the drone starts, the parameters are uploaded to browser and sent back to the drone.

With SD card your flightData will be save in a file in the folder `/data/flight_i.csv`, which is automatically created.

The directory structure of the SD is
<pre>
\src
    config.txt
\data
    \videos
    \images
    \flightData
        flight_1.csv
        flight_2.csv
             :
        flight_n.csv
</pre>


# Browser 
I also put in the [test](https://github.com/sebastiano123-c/Esp32-cam-telemetry/tree/master/test) folder the HTML source.
If you modify it, please remember to full compress all css, HTML and js parts, otherwise the memory of the esp32-cam will overflow.

The index file must then be compressed using this [gzip](https://gchq.github.io/CyberChef/#recipe=Gzip('Dynamic%20Huffman%20Coding','index.html.gz','',false)To_Hex('0x',0)Split('0x',',0x')) procedure, and then put it in the camera [index](https://github.com/sebastiano123-c/src/camera_index.h).


# TelemetryAnalysis
To better visualize your flight data, telemetryAnalysis provide a versatile graphic tool for the SD recordings.
In [telemetryAnalysis](https://github.com/sebastiano123-c/Esp32-cam-telemetry/tree/master/telemetryAnalysis) folder I put a simple Python script and an executable file working in Windows.
The analysis includes:
- roll and pitch angle sequence;
- RC-controller inputs;
- altitude recordings;
- battery recordings.
For the [python](https://github.com/sebastiano123-c/Esp32-cam-telemetry/blob/master/telemetryAnalysis/telemetryAnalysis.py) script, I also menage a **GPS** map.

# Roadmap
There few things that must be done in the future:
* save video and photo;

# Author
Sebastiano Cocchi
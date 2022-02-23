# Esp32-cam-telemetry
Create a telemetry system for your drone using esp32-cam.

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
The principle of working is: your board is connected via UART to the esp32-cam which, using its internal wifi, generate a wifi access point.

<pre align=center>
 transmitter             esp32-cam                     
|----------|           |----------|                    
|       TX |-----------| RX0      |                    
|       RX |-----------| TX0      |  => wifi => web app
|      GND |-----------|GND0      |                    
|----------|           |----------|                    
</pre>

# Examples
In the [wifi-tx](https://github.com/sebastiano123-c/Esp32-cam-telemetry/test/wifi-tx/wifi-tx.ino) example I use an esp32 board as transmitter.
If you don't use a esp32 board as transmitter make the proper changes on the code and on the connections.

Connect the RX1 (GPIO 26) of the esp32 to the TX0 (GPIO 1) of esp32-cam, and TX1 (GPIO 25) of the esp32 to the RX0 (GPIO 3) of esp32-cam.

Upload the [wifi-tx](https://github.com/sebastiano123-c/Esp32-cam-telemetry/test/wifi-tx/wifi-tx.ino) sketch on your transmitter board.

# Telemetry
For simplicity, in this code I use 5 variables: roll, pitch, flight mode, altitude and battery which are updated at the every 100ms in the browser.

# Talk with drone
PID variables are easily set in the browser in the toggling menu.
When you change the number, the parameter is sent to the esp32-cam and then back to the transmitter board via uart. 

# Streaming video
Press the button `start video` in the toggling menu and the live streaming will appear showing also the status of the telemetry.
You can set the video parameters in the toggling menu. 

# SD card
To better appreciate this telemetry system, I recommend to use also SD card.
SD card _must_ be formatted in FAT32.

In the SD you create a directory and a file called `/src/config.txt` in which you write _only_ the initial PID parameters.
From this file, at the drone start, the parameters are uploaded to browser and sent back to the drone.

With SD card your flightData will be save in a file in the folder `/data/flight_i.txt`, which is automatically created.

The directory structure is something similar to the following

<pre>
\src
    config.txt
\data
    \videos
    \images
    \flightData
        flight_1.txt
        flight_2.txt
             :
        flight_n.txt
</pre>


# Wifi connection
After connecting to the network using the password `esp32cam`, dial in your browser's search bar `292.168.4.1`.
That's it.

# Browser 
I also left in the test folder the HTML source, remember to full compress it otherwise the memory of the esp32-cam will overflow.

The source file then must be compressed using in gzip and then putted in the camera [index](https://github.com/sebastiano123-c/src/camera_index.h).

# Roadmap
There few things that must be done in the future:
* create simple structure that the user can handle without any other modification on the code;
* save video and photo;
* make more flexible SD card reading

# Author
Sebastiano Cocchi
"""
    @file DroneInoFlightAnalysis.py
    @author Sebastiano Cocchi
    @brief Compute data analysis for DroneIno data flight.

    Run DroneInoFlightAnalysis.py python script and choose a flight data file produced by DroneIno.
    This will show a pitch and roll data for PID fine tuning.
    
"""

from tkinter import filedialog as fd
import numpy as np
import csv, os
import matplotlib.pyplot as plt
import gmplot                                    # plot on Google maps
import webbrowser
from datetime import datetime
import matplotlib.dates as mdates
myFmt = mdates.DateFormatter('%H:%M:%S')


# set false if you not use GPS
GPS_data = False
battery_fit = False


# create folder for GPS
GPSFolder = "data/GPS"
if not os.path.exists(GPSFolder):
    os.makedirs(GPSFolder)

# create register for edge 
edge_path="C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe"
webbrowser.register('edge', None, webbrowser.BackgroundBrowser(edge_path))


# main
while(1):

    filename = fd.askopenfilename()

    if(filename == ""): # exit on void filename
        break


    # create headers
    headers = []
    index = []
    pitch = []
    roll = []
    altitude = []
    battery = []
    rollRec = []
    pitchRec = []
    yawRec = []
    throttleRec = []
    latitude = []
    longitude = []
    time = []

    # if file is csv
    if filename[-3:] == "csv":

        # open the file
        with open(filename, encoding = "cp437", errors = 'remove') as csvfile:

                # read csv
                spamreader = csv.reader(csvfile, delimiter=',')

                tmp = 0
                first_line = True

                # read row by row
                for row in spamreader:
                    
                    # if first line then it is header
                    if (first_line) == True:
                        headers = row                                                        # roll; pitch; flightMode; battery; altitude
                        first_line = False
                        
                    # fill arrays
                    else:

                        if(GPS_data == False):
                            if(len(row) == 12):
                                index.append(tmp)
                                roll.append(float(row[0]))
                                pitch.append(float(row[1]))
                                battery.append(float(row[3]))
                                altitude.append(float(row[4]))
                                rollRec.append(float(row[5]))
                                pitchRec.append(float(row[6]))
                                yawRec.append(float(row[7]))
                                throttleRec.append(float(row[8]))
                            else:
                                print("line %i not valid, has %i \n"%(tmp,len(row)))
                        else:
                            lat, lon = float(row[9]), float(row[10])

                            if (lat != 0. and lon != 0.):# if there is no GPS data
                                index.append(tmp)
                                roll.append(float(row[0]))
                                pitch.append(float(row[1]))
                                battery.append(float(row[3]))
                                altitude.append(float(row[4]))
                                rollRec.append(float(row[5]))
                                pitchRec.append(float(row[6]))
                                yawRec.append(float(row[7]))
                                throttleRec.append(float(row[8]))
                                latitude.append(lat)
                                longitude.append(lon)
                                time.append(datetime.strptime(row[11][:-1], '%H:%M:%S.%f'))

                    tmp = tmp + 1
                
                # matplotlib
                fig, axs = plt.subplots(2,2)
                fig.suptitle(filename)
                
                
                # x axis data
                if(GPS_data == False):
                    dataX = index
                else:
                    dataX = time                    
                    axs[0,0].xaxis.set_major_formatter(myFmt)
                    axs[1,0].xaxis.set_major_formatter(myFmt)
                    axs[0,1].xaxis.set_major_formatter(myFmt)
                    axs[1,1].xaxis.set_major_formatter(myFmt)
                    

                # axs[0,0].set_ylim([-45, 45])
                axs[0,0].set_title("Roll/pitch angles")
                axs[0,0].plot(dataX, roll, "b-", label="roll")            
                axs[0,0].plot(dataX, pitch, "y-",label="pitch")
                axs[0,0].set_ylabel("Angle [°]")
                axs[0,0].legend(loc='best')

                axs[1,0].set_ylim([920, 2080])
                axs[1,0].set_title("Receiver channels pulse width")
                axs[1,0].plot(dataX, rollRec, ".", alpha=0.6, label="roll rec.")
                axs[1,0].plot(dataX, pitchRec, ".", alpha=0.6, label="pitch rec.")
                axs[1,0].plot(dataX, yawRec, ".", alpha=0.6, label="yaw rec.")
                axs[1,0].plot(dataX, throttleRec, ".", alpha=0.6, label="throttle rec.")
                axs[1,0].set_ylabel("time [us]")
                axs[1,0].legend(loc='best')



                axs[0,1].set_ylabel("Altitude [m]")
                axs[0,1].set_title("Altitude")
                axs[0,1].plot(dataX, altitude, 'g-', label="Alt.")
                axs[0,1].legend(loc='best')



                axs[1,1].set_title("Battery voltage")
                axs[1,1].plot(dataX, battery, label="battery")
                
                # battery curve linear fit
                if(battery_fit==True):
                    poptL = np.polyfit(index, battery, 1)
                    poly1d_fn = np.poly1d(poptL) 
                    axs[1,1].plot(dataX, poly1d_fn(index), label='fit')
                    
                axs[1,1].set_ylabel("Voltage [V]")
                axs[1,1].legend(loc='best')


                plt.setp( axs[0,0].xaxis.get_majorticklabels(), rotation=30 )
                plt.setp( axs[1,0].xaxis.get_majorticklabels(), rotation=30 )
                plt.setp( axs[0,1].xaxis.get_majorticklabels(), rotation=30 )
                plt.setp( axs[1,1].xaxis.get_majorticklabels(), rotation=30 )

                # axs[1,1].set_title("Battery voltage with throttle")
                # axs[1,1].plot(throttleRec, battery, label="battery")
                # axs[1,1].plot(throttleRec, poly1d_fn(throttleRec), label='fit')
                # axs[1,1].set_ylabel("Voltage [V]")
                # axs[1,1].legend(loc='best')          

                # statistics
                print("\n\n", filename)
                print("  - Battery:")
                print("    > tot. batt. consumption: ", max(battery) - min(battery), " V")
                if(battery_fit): print("    > batt. linear trend: m=%2.1e " % poptL[0], " b=%2.1e" % poptL[1])
                print("  - Altimeter:")
                print("    > tot. ascension: ", max(altitude) - min(altitude), " m")
                print("")


                if (GPS_data):
                    # GPS print on Google maps               
                    gmap3 = gmplot.GoogleMapPlotter(latitude[0],
                                                    longitude[0], 17)
                    
                    # scatter method of map object 
                    # scatter points on the google map
                    gmap3.scatter( latitude, longitude, '#FF0000',
                                                size = 1, marker = False )
                    
                    # Plot method Draw a line in
                    # between given coordinates
                    gmap3.plot(latitude, longitude, 
                            'cornflowerblue', edge_width = 1)
                    
                    gmap3.draw( GPSFolder+"/GPS.html" )

                    # open an HTML file on my own (Windows) computer
                    url = "file://"+os.path.realpath(GPSFolder+"/GPS.html")
                    webbrowser.get('edge').open(url, new=2)

                    print("\n  See GPS.html map in '"+GPSFolder+"' \n")
                
                plt.show()
            
    else:

        print("\nFile is not .csv")

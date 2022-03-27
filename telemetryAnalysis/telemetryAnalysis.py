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
                        row = [float(x) for x in row]

                        index.append(tmp)
                        roll.append(row[0])
                        pitch.append(row[1])
                        battery.append(row[3])
                        altitude.append(row[4])
                        rollRec.append(row[5])
                        pitchRec.append(row[6])
                        yawRec.append(row[7])
                        throttleRec.append(row[8])

                        if (row[9] != 0. and row[10] != 0.):# if there is no GPS data
                            latitude.append(row[9])
                            longitude.append(row[10])
                        
                    tmp = tmp + 1
                
                
                # battery curve linear fit
                poptL = np.polyfit(index, battery, 1)
                poly1d_fn = np.poly1d(poptL) 


                # matplotlib
                fig, axs = plt.subplots(2,2)
                fig.suptitle(filename)

                # axs[0,0].set_ylim([-45, 45])
                axs[0,0].set_title("Roll/pitch angles")
                axs[0,0].plot(index, roll, "b-", label="roll")            
                axs[0,0].plot(index, pitch, "y-",label="pitch")
                axs[0,0].set_ylabel("Angle [°]")
                axs[0,0].legend(loc='best')


                axs[1,0].set_ylim([920, 2080])
                axs[1,0].set_title("Receiver channels pulse width")
                axs[1,0].plot(index, rollRec, "-.", alpha=0.6, label="roll rec.")
                axs[1,0].plot(index, pitchRec, "-.", alpha=0.6, label="pitch rec.")
                axs[1,0].plot(index, yawRec, "-.", alpha=0.6, label="yaw rec.")
                axs[1,0].plot(index, throttleRec, "-.", alpha=0.6, label="throttle rec.")
                axs[1,0].set_ylabel("time [us]")
                axs[1,0].legend(loc='best')


                axs[0,1].set_ylabel("Altitude [m]")
                axs[0,1].set_title("Altitude")
                axs[0,1].plot(index, altitude, 'g-', label="Alt.")
                axs[0,1].legend(loc='best')


                axs[1,1].set_title("Battery voltage")
                axs[1,1].plot(index, battery, label="battery")
                axs[1,1].plot(index, poly1d_fn(index), label='fit')
                axs[1,1].set_ylabel("Voltage [V]")
                axs[1,1].legend(loc='best')

                # axs[1,1].set_title("Battery voltage with throttle")
                # axs[1,1].plot(throttleRec, battery, label="battery")
                # axs[1,1].plot(throttleRec, poly1d_fn(throttleRec), label='fit')
                # axs[1,1].set_ylabel("Voltage [V]")
                # axs[1,1].legend(loc='best')          

                # statistics
                print("\n\n", filename)
                print("  - Battery:")
                print("    > tot. batt. consumption: ", max(battery) - min(battery), " V")
                print("    > tot. batt. consumption: ", max(battery) , " V")
                print("    > batt. linear trend: m=%2.1e " % poptL[0], " b=%2.1e" % poptL[1])
                print("  - Altimeter:")
                print("    > tot. ascension: ", max(altitude) - min(altitude), " m")
                print("")

                plt.show()

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

                print("\n\n  See GPS.html map in '"+GPSFolder+"' \n")
            
    else:

        print("\nFile is not .csv")

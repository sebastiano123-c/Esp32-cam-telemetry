"""
    @file DroneInoFlightAnalysis.py
    @author Sebastiano Cocchi
    @brief Compute data analysis for DroneIno data flight.

    Run DroneInoFlightAnalysis.py python script and choose a flight data file produced by DroneIno.
    This will show a pitch and roll data for PID fine tuning.
    
"""

from tkinter import filedialog as fd
import plotly.graph_objects as go
import csv
import matplotlib.pyplot as plt

# # path to folder
# dirName = "data"

# # create the list of files in the dir
# fileList = os.listdir(dirName)


filename = fd.askopenfilename()

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
                    
                tmp = tmp + 1
            

            fig, axs = plt.subplots(2,2)
            fig.suptitle(filename)

            # axs[0,0].set_ylim([-45, 45])
            axs[0,0].set_title("Roll/pitch angles")
            axs[0,0].plot(index, roll, label="roll")            
            axs[0,0].plot(index, pitch, label="pitch")
            axs[0,0].set_ylabel("Angle [°]")
            axs[0,0].legend(loc='best')


            axs[1,0].set_ylim([920, 2080])
            axs[1,0].set_title("Receiver channels pulse width")
            axs[1,0].plot(index, rollRec, label="roll rec.")
            axs[1,0].plot(index, pitchRec, label="pitch rec.")
            axs[1,0].plot(index, yawRec, label="yaw rec.")
            axs[1,0].plot(index, throttleRec, label="throttle rec.")
            axs[1,0].set_ylabel("time [us]")
            axs[1,0].legend(loc='best')


            axs[0,1].set_ylabel("Altitude [m]")
            axs[0,1].set_title("Altitude")
            axs[0,1].plot(index, altitude, 'g-', label="Alt.")
            axs[0,1].legend(loc='best')


            axs[1,1].set_title("Battery voltage")
            axs[1,1].plot(index, battery, label="battery")
            axs[1,1].set_ylabel("Voltage [V]")
            axs[1,1].legend(loc='best')


            plt.show()

else:

    print("File is not .csv")

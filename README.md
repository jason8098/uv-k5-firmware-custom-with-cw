# F4HWN Firmware with CW Beacon
This is an implementation/demo of the CW beacon. A custom string can be set and it will chnage each charactors into the morse code tones while transmitting it.
PS: Bandscope was replcaed to this due to the capacity issue.

# Use Case
I made this mainly for the FOX HUNT beacon purposes. The modulation is FM, so most of all receivers, handhelds can receive it without any problem. 

# How it Works?
- App is opened using F+5 Key combination.
- Power levels can be adjusted using the up down arrow keys.
- Then press menu key to activate TX. It will convert the string set to morse codes and TX at a speed of 10WPM. At the moment, the customization of the string is only possible by chnaging the source code.
- Press Exit to stop the TX.

# How to Configure?
## Step 1
Open up a codespace for this repo.

## Step 2
Locate to "app/morse.c"

## Step 3
Edit the string at the following line (25).

    char* cwid_m = "DE N0CALL"; //Edit this Message

## Step 4
Compile the firmware by typing the following in the terminal:

    ./compile-with-docker.sh custom


## Step 5 
When the compilation is done, you will see a folder "compiled-firmware" generated in the Explorer section. Simply open that up and right click on any of the .bin file then Download.

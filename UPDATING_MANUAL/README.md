This is a short and easy manual on how to load joystick sketch to your Arduino board.
# STEP 1
Follow this link to https://www.arduino.cc and download and install latest Arduino IDE software
### First select SOFTWAREand choose DOWNLOADS
![Step1](PICS/1_download_arduino.jpg)
### Then select Windows installer
![Step1](PICS/2_download_arduino2.jpg)
### Install Arduino IDE, following installer instructions.

# STEP 2
Go to Joystick Shield main GitHub directory https://github.com/RGCustom/Joystick-Shield-RG-Custom and download firmware sketches
### First click on Clone or Download green buttons and select Download .ZIP
![Step2](PICS/3_download.jpg)

# STEP 3
Now download and install needed libraries 
### Libraries, included as submodules wont download on previous step, so they must be downloaded separately.
### Clicking on library will lead you to its location.
![Step2](PICS/4_download_libraries.jpg)
### Click Branch button and select master to get the latest library.
![Step2](PICS/4_download_libraries2.jpg)
### Donwload library same as you downloaded main sketch.
![Step2](PICS/3_download.jpg)
### Install librariesin your arduino libraries folder.
Arduino manual on installing libraries https://www.arduino.cc/en/Guide/Libraries#toc2
### ArduinoJoystick library has a default installation bat file. 
Default libraries location (you can just unzip them there) is C:\Users\ your user name \Documents\Arduino\libraries
or just go to My Documents > Arduino > libraries

# STEP 4
### Unzip main sketch you downloaded in to a folder.
Select folder with a proper firmware sketch (names may vary)
### Open sketch .INO file. 
![Step4](PICS/5_open_sketch.jpg)

Arduino IDE will open sketch.
### Open Tools menu and select Board -> Arduino Leonardo
![Step4](PICS/6_select_board.jpg)
### Also select corresponding port number in same menu.
![Step4](PICS/7_select_boardport.jpg)

# STEP 5
Prepare Arduino IDE to upload firmware.
### Select Preferences from File menu
![Step5](PICS/8_arduino_setup.jpg)
### And in Preferences window mark "Show verbose otput during: upload" checkbox
![Step5](PICS/9_arduino_setup2.jpg)

# STEP 6
### Verify sketch by pressing VERIFY button
![Step6](PICS/10_verify.jpg)
#### If everything is Ok, you will see smth like this
![Step6](PICS/11_verify_ok.jpg)
### After that hit upload button
![Step6](PICS/12_upload.jpg)

# STEP 7 -= SKIP THIS STEP IF ARDUINO ALREADY HAS UPLOADED JOYSTICK CODE =-
#### After firmware will compile, Arduino IDE will start looking for controller in bootloader state
### And will start scanning ports for arduino, showing smth like
PORTS {COM1, COM2, COM14, } / {COM1, COM2, COM14, } => {}
PORTS {COM1, COM2, COM14, } / {COM1, COM2, COM14, } => {}
### This is when you have to push RESET button o the back of your device.
![Step6](PICS/13_upload_reset.jpg)
### After upload will be finished, your device will reboot and will be updated.
![Step6](PICS/14_done.jpg)


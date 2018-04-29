//**************************************************************//
//  Name    : RG Custom Pro Micro shield code                       //
//  Author  : Konstantin `RenderG` Yakushev                     //
//  Date    : 28 Apr, 2018                                      //
//  Version : 2.0                                               //
//          :                                                   //
//****************************************************************
#include <Wire.h>
#include <Joystick.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2); // lcd setup
// Create the Joystick
Joystick_ Joystick;
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            A9

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//**************************************************
//      PUSHBUTTONS SETUP / НАСТРОЙКА КНОПОК
//**************************************************
const int PBnum = 22;        // Number of pushbuttons / количество кнопок
int PBreg[] = {1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3}; // shiftregisters with pushbuttons / регистры с кнопками
int PBpin[] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5}; // shiftregister pins with pushbuttons / ножки регистров с кнопками
int PBstate[PBnum]; // State of pushbuttons / состояние кнопок (для индикации)

//**************************************************
//  TOGGLE SWITCHES SETUP / НАСТРОЙКА ТУМБЛЕРОВ
//**************************************************
const int TGnum = 10;        // Number of toggle switches / количество тумблеров
int TGreg[] = {3,3,4,4,4,4,4,4,4,4}; // shiftregisters with toggleswitches / регистры с тумблерами
int TGpin[] = {6,7,0,1,2,3,4,5,6,7}; // shiftregister pins with toggleswitches / ножки регистров с тумблерами
int TGstate[TGnum]; // State of toggle switches (for indication on LCD or leds) / состояние тумблеров (для индикации)
int TGdelay(15); // Toggle switch pulse duration in ms / длинна импульса тумблера

//**************************************************
//       HATS SETUP / НАСТРОЙКА ХАТОК
//**************************************************
const int HATnum = 4;        // Number of POV HAT switches (one POV hat is 4 buttons, so multiply by 4) / количество хаток. Одна хатка - 4 кнопки, так что умножайте на 4.
int HATreg[] = {5,5,5,5}; // shiftregisters with hatswitches / регистры с хатками
int HATpin[] = {0,1,2,3}; // shiftregister pins with hatswitches / ножки регистров с хатками
int HATstate[HATnum]; // State of POV HAT switches (for indication on LCD or leds) / состояние хаток (для индикации)

//**************************************************
//      ENCODERS SETUP / НАСТРОЙКА ЭНКОДЕРОВ
//**************************************************
const int ENCnum = 2;        // Number of rotary encoders (each encoder L+R turns counts as one) / количество энкодеров. 
int ENCregA[] = {5,5}; // shiftregisters with encoders direction A / регистры с энкодерами с вращением в сторону А
int ENCpinA[] = {4,7}; // shiftregister pins encoders direction A / регистры с энкодерами с вращением в сторону B
int ENCregB[] = {5,5}; // shiftregisters with encoders direction B / регистры с энкодерами с вращением в сторону B
int ENCpinB[] = {5,6}; // shiftregister pins encoders direction B / ножки регистров с энкодерами с вращением в сторону B
int ENCA[ENCnum]; 
int ENCB[ENCnum]; 
int ENC_Aprev[ENCnum];  
int ENCtime[] = {4,4}; // Encoders timing in ms. increase for slower and decrease for faster. Тайминги энкодеров
unsigned long loopTime;


// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
const bool initAutoSendState = true;
//const bool initAutoSendState = false;

uint8_t joystickType = 0x08; // Default: 0x03 - Indicates the joystick's HID report ID. This value must be unique if you are creating multiple instances of Joystick. Do not use 0x01 or 0x02 as they are used by the built-in Arduino Keyboard and Mouse libraries.
uint8_t buttonCount = 32; //Button count
uint8_t hatSwitchCount = 2;


//**************************************************
//        AXIS INPUTS SETUP / НАСТРОЙКА ОСЕЙ
//**************************************************
const int xAxisFilter = 4;         // smoothing for X axis
const int yAxisFilter = 4;         // smoothing for Y axis
const int zAxisFilter = 4;         // smoothing for Z axis
const int rXAxisFilter = 4;        // smoothing for rX axis

int xAxisRead[xAxisFilter];         // readings for X axis
int yAxisRead[yAxisFilter];         // readings for Y axis
int zAxisRead[zAxisFilter];         // readings for Z axis
int rXAxisRead[rXAxisFilter];        // readings for rX axis

int xAxisIndex = 0;         // the index of the currentreadings for X axis
int yAxisIndex = 0;         // the index of the currentreadings for Y axis
int zAxisIndex = 0;         // the index of the currentreadings for Z axis
int rXAxisIndex = 0;        // the index of the currentreadings for rX axis

long xAxisTotal = 0;         // the running total for X axis
long yAxisTotal = 0;         // the running total for Y axis
long zAxisTotal = 0;         // the running total for Z axis
long rXAxisTotal = 0;        // the running total for rX axis

int xAxisAverage = 0;         // the average for X axis
int yAxisAverage = 0;         // the average for Y axis
int zAxisAverage = 0;         // the average for Z axis
int rXAxisAverage = 0;        // the average for rX axis

const int xAxis = A0;         // analog sensor for X axis
const int yAxis = A3;         // analog sensor for Y axis
const int zAxis = A2;         // analog sensor for Z axis
const int rXAxis = A1;        // analog sensor for rX axis

bool includeXAxis = true; //Indicates if the X Axis is available on the joystick.
bool includeYAxis = true; //Indicates if the Y Axis is available on the joystick.
bool includeZAxis = true; //Indicates if the Z Axis (in some situations this is the right X Axis) is available on the joystick.
bool includeRxAxis = true; //Indicates if the X Axis Rotation (in some situations this is the right Y Axis) is available on the joystick.
bool includeRyAxis = false; //Indicates if the Y Axis Rotation is available on the joystick.
bool includeRzAxis = false; //Indicates if the Z Axis Rotation is available on the joystick.
bool includeRudder = false; //Indicates if the Rudder is available on the joystick.
const int CalibrationPin = 8; // calibration toggle switch

// axis calibration variables:
int16_t sensorValueX = 0;         // the sensor X value
int sensorValueY = 0;         // the sensor Y value
int sensorValueZ = 0;         // the sensor Z value
int sensorValueRx = 0;        // the sensor Rx value
int sensorValueRy = 0;        // the sensor Rx value
int sensorValueRz = 0;        // the sensor Rx value

int sensorMinX = 0;           // minimum sensor value
int sensorMaxX = 1;         // maximum sensor value
int sensorMinY = 0;           // minimum sensor value
int sensorMaxY = 1;         // maximum sensor value
int sensorMinZ = 0;           // minimum sensor value
int sensorMaxZ = 1;         // maximum sensor value
int sensorMinRx = 0;           // minimum sensor value
int sensorMaxRx = 1;         // maximum sensor value
int sensorMinRy = 0;           // minimum sensor value
int sensorMaxRy = 1;         // maximum sensor value
int sensorMinRz = 0;           // minimum sensor value
int sensorMaxRz = 1;         // maximum sensor value*/


// variables will change:
int CalibrationState = 0;     // variable for reading calibration status

//**************************************************
//              SHIFT REGISTERS SETUP
//**************************************************
int latchPin = 4;
int dataPin = 14;
int clockPin = 15;

//Define number of registers
int num_registers = 5;

//Define array to hold the data
//for each shift register.

byte switchVar[] = {72, 73, 74, 75, 76};  //01001000,01001001,01001010,01001011,01001100

//Define array to hold the state
//for each shift register.

byte switchState[] = {72, 73, 74, 75, 76};  //01001000,01001001,01001010,01001011,01001100
byte lastState[] = {72, 73, 74, 75, 76};  //01001000,01001001,01001010,01001011,01001100


//**************************************************
//              DEBOUNCE SETUP
//**************************************************
// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 5;    // the debounce time; increase if the output flickers

//define an array that has a place for the values of

byte settingVal[] = {  1, 1, 1, 1, 1, 1, 1, 1};

unsigned long lastlooptime = 0;  // the last time the output pin was toggled
unsigned long looptime = 0;    // the debounce time; increase if the output flickers




void setup() {
  //start serial
 // Serial.begin(9600);

  //start Joystick
  Joystick.begin(initAutoSendState);
  
 //defining axis ranges
  Joystick.setXAxisRange(-32768, 32767);
  Joystick.setYAxisRange(-32768, 32767); 
  Joystick.setZAxisRange(-32768, 32767); 
  Joystick.setRxAxisRange(-32768, 32767);
  Joystick.setRyAxisRange(-32768, 32767);
  Joystick.setRzAxisRange(-32768, 32767); 
//  Joystick.setRudderAxisRange(0, 1023);
//  Joystick.setThrottleAxisRange(0, 1023); 

  //define pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);
 
  pixels.begin(); // This initializes the NeoPixel library.

//**************************************************
//              LCD INITIALISING STRING
//**************************************************
  lcd.init();
  lcd.backlight();// Enable LCD backlight
}

void loop() {
  looptime = millis();

//**************************************************
//              READING SHIFT REGISTERS
//**************************************************
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(latchPin, 1);
  //set it to 1 to collect parallel data, wait
  //delayMicroseconds(20);
  //set it to 0 to transmit data serially
  digitalWrite(latchPin, 0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first
  for (int z = 1; z <= num_registers; z++) {
    switchVar[z] = shiftIn(dataPin, clockPin);
  }
// *************************************************
//     PUSH BUTTONS SECTION
// *************************************************
    for (int i = 0; i <= (PBnum - 1); i++) {
      if (bitRead(switchVar[PBreg[i]], PBpin[i]) != bitRead(lastState[PBreg[i]], PBpin[i])) {
          // reset the debouncing timer
          lastDebounceTime = millis();
         }
         if ((millis() - lastDebounceTime) > debounceDelay) {
           // whatever the reading is at, it's been there for longer
           // than the debounce delay, so take it as the actual current state:
           // if the button state has changed:
           if (bitRead(switchVar[PBreg[i]], PBpin[i]) != bitRead(switchState[PBreg[i]], PBpin[i])) {
             switchState[PBreg[i]] = switchVar[PBreg[i]];
             // activate button if the new button state ON or OF
             if (bitRead(switchState[PBreg[i]], PBpin[i]) < 1) {
               //action on button press
               Joystick.setButton(i,1);
               //lcd.setCursor(1, 1);
               //lcd.print("ON");
               PBstate[i] = 1;
               }
              else {
               //action on button release
               Joystick.setButton(i,0);
               //lcd.setCursor(1, 1);
               //lcd.print("OFF");
               PBstate[i] = 0;
             }
            }
          }
        lastState[PBreg[i]] = switchVar[PBreg[i]];
       }
// *************************************************
//     PUSH BUTTONS SECTION ENDS HERE
// *************************************************

// *************************************************
//     TOGGLE SWITCHES SECTION
// *************************************************
    for (int i = 0; i <= (TGnum - 1); i++) {
      if (bitRead(switchVar[TGreg[i]], TGpin[i]) != bitRead(lastState[TGreg[i]], TGpin[i])) {
          // reset the debouncing timer
          lastDebounceTime = millis();
         }
         if ((millis() - lastDebounceTime) > debounceDelay) {
           // whatever the reading is at, it's been there for longer
           // than the debounce delay, so take it as the actual current state:
           // if the button state has changed:
           if (bitRead(switchVar[TGreg[i]], TGpin[i]) != bitRead(switchState[TGreg[i]], TGpin[i])) {
             switchState[TGreg[i]] = switchVar[TGreg[i]];
             // activate button if the new button state ON or OF
             if (bitRead(switchState[TGreg[i]], TGpin[i]) < 1) {
               //action on button press
               Joystick.setButton(i + PBnum,1);
               TGstate[i] = 1;
               //lcd.setCursor(1, 1);
               //lcd.print("ON");
               delay(TGdelay);
               Joystick.setButton(i + PBnum,0);
               }
              else {
               //action on button release
               Joystick.setButton(i + PBnum,1);
               //lcd.setCursor(1, 1);
               //lcd.print("OFF");
               delay(TGdelay);
               TGstate[i] = 0;
               Joystick.setButton(i + PBnum,0);
             }
            }
          }
        lastState[TGreg[i]] = switchVar[TGreg[i]];
       }
// *************************************************
//     TOGGLE SWITCH SECTION ENDS HERE
// *************************************************     

// *********************************************************
//     POV HATS SECTION Hats can have 9 positions.
//     -1 (nothing pressed) + degrees 0-315 with step of 45.
// *********************************************************
    for (int i = 0; i <= (HATnum - 1); i++) {
      if (bitRead(switchVar[HATreg[i]], HATpin[i]) != bitRead(lastState[HATreg[i]], HATpin[i])) {
          // reset the debouncing timer
          lastDebounceTime = millis();
         }
         if ((millis() - lastDebounceTime) > debounceDelay) {
           // whatever the reading is at, it's been there for longer
           // than the debounce delay, so take it as the actual current state:
            // if the button state has changed:
           if (bitRead(switchVar[HATreg[i]], HATpin[i]) != bitRead(switchState[HATreg[i]], HATpin[i])) {
             switchState[HATreg[i]] = switchVar[HATreg[i]];
             // activate button if the new button state ON or OF
             if (bitRead(switchState[HATreg[i]], HATpin[i]) < 1) {
               //action on button press
               Joystick.setHatSwitch(0, ((i + 1) * 90));
               HATstate[i] = ((i + 1) * 90);
               }
              else {
               //action on button release
               Joystick.setHatSwitch(0, -1);
               HATstate[i] = -1;
             }
            }
          }
        lastState[HATreg[i]] = switchVar[HATreg[i]];
       }
// *************************************************
//     HATS SECTION ENDS HERE
// *************************************************

// *************************************************
//     ENCODERS SECTION
// *************************************************
   for (int i = 0; i <= (ENCnum - 1); i++) {
      ENCA[i] = bitRead(switchVar[ENCregA[i]], ENCpinA[i]);     // Reads encoder state A 
      ENCB[i] = bitRead(switchVar[ENCregB[i]], ENCpinB[i]);     // Reads encoder state B 
       if((!ENCA[i]) && (ENC_Aprev[i])){    // If state changes
       if(ENCB[i]) {
        //Encoder rotates A
        Joystick.setHatSwitch(1, (i * 90));
        delay(ENCtime[i]);
        Joystick.setHatSwitch(1, -1);
       }   
       else { 
        //Encoder rotates B
        Joystick.setHatSwitch(1, ((i * 90)+180));
        delay(ENCtime[i]);
        Joystick.setHatSwitch(1, -1);
       }  
      }     
      ENC_Aprev[i] = ENCA[i];     // Save A state  
     }

   
// *************************************************
//     ENCODERS SECTION ENDS HERE
// *************************************************


 //*************************************************
  //             Axis controls
  //*************************************************
  //reading analogue controls

  sensorValueX = analogRead(xAxis);   // reading X axis
  sensorValueY = analogRead(yAxis);   // reading Y axis
  sensorValueZ = analogRead(zAxis);   // reading Z axis
  sensorValueRx = analogRead(rXAxis); // reading Rx axis

  // apply the calibration to the sensor reading
  sensorValueX = map(sensorValueX, sensorMinX, sensorMaxX, -32768, 32767);
  sensorValueY = map(sensorValueY, sensorMinY, sensorMaxY, -32768, 32767);
  sensorValueZ = map(sensorValueZ, sensorMinZ, sensorMaxZ, -32768, 32767);
  sensorValueRx = map(sensorValueRx, sensorMinRx, sensorMaxRx, -32768, 32767);

  // in case the sensor value is outside the range seen during calibration
  sensorValueX = constrain(sensorValueX, -32768, 32767);
  sensorValueY = constrain(sensorValueY, -32768, 32767);
  sensorValueZ = constrain(sensorValueZ, -32768, 32767);
  sensorValueRx = constrain(sensorValueRx, -32768, 32767);

  //*************************************************
  //             Axis controls smoothing
  //*************************************************
    xAxisTotal = xAxisTotal - xAxisRead[xAxisIndex];  // subtract the last reading:
    xAxisRead[xAxisIndex] = sensorValueX;  // read from the sensor:
    xAxisTotal = xAxisTotal + xAxisRead[xAxisIndex];  // add the reading to the total:
    xAxisIndex = xAxisIndex + 1; // advance to the next position in the array:
    if (xAxisIndex >= xAxisFilter) { // if we're at the end of the array...
      xAxisIndex = 0;    // ...wrap around to the beginning:
    }
    xAxisAverage = xAxisTotal / xAxisFilter; // calculate the average:

    yAxisTotal = yAxisTotal - yAxisRead[yAxisIndex];  // subtract the last reading:
    yAxisRead[yAxisIndex] = sensorValueY;  // read from the sensor:
    yAxisTotal = yAxisTotal + yAxisRead[yAxisIndex];  // add the reading to the total:
    yAxisIndex = yAxisIndex + 1; // advance to the next position in the array:
    if (yAxisIndex >= yAxisFilter) { // if we're at the end of the array...
      yAxisIndex = 0;    // ...wrap around to the beginning:
    }
    yAxisAverage = yAxisTotal / yAxisFilter; // calculate the average:

    zAxisTotal = zAxisTotal - zAxisRead[zAxisIndex];  // subtract the last reading:
    zAxisRead[zAxisIndex] = sensorValueZ;  // read from the sensor:
    zAxisTotal = zAxisTotal + zAxisRead[zAxisIndex];  // add the reading to the total:
    zAxisIndex = zAxisIndex + 1; // advance to the next position in the array:
    if (zAxisIndex >= zAxisFilter) { // if we're at the end of the array...
      zAxisIndex = 0;    // ...wrap around to the beginning:
    }
    zAxisAverage = zAxisTotal / zAxisFilter; // calculate the average:

    rXAxisTotal = rXAxisTotal - rXAxisRead[rXAxisIndex];  // subtract the last reading:
    rXAxisRead[rXAxisIndex] = sensorValueRx;  // read from the sensor:
    rXAxisTotal = rXAxisTotal + rXAxisRead[rXAxisIndex];  // add the reading to the total:
    rXAxisIndex = rXAxisIndex + 1; // advance to the next position in the array:
    if (rXAxisIndex >= rXAxisFilter) { // if we're at the end of the array...
      rXAxisIndex = 0;    // ...wrap around to the beginning:
    }
    rXAxisAverage = rXAxisTotal / rXAxisFilter; // calculate the average:

  Joystick.setXAxis(xAxisAverage);
  Joystick.setYAxis(yAxisAverage);
  Joystick.setZAxis(zAxisAverage);
  Joystick.setRxAxis(rXAxisAverage);
  //Joystick.setRyAxis(sensorValueRx);
  //Joystick.setRzAxis(sensorValueRx);


  /*int ledstrip = sensorValueX;
  ledstrip = map(ledstrip,127,-127,0,NUMPIXELS);
  // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  pixels.clear();
  pixels.show();
  for (int i = 0; i <= ledstrip; i++){
   pixels.setPixelColor(i-1, pixels.Color(10, 0, 0)); // RGB
    }
   pixels.show(); // This sends the updated pixel color to the hardware.*/

 

  //    *******************************************************
  //                 AXIS CALIBRATION (auto)
  //    
  //    *******************************************************

  // X axis calibration
  sensorValueX = analogRead(xAxis);

  // record the maximum sensor value
  if (sensorValueX > sensorMaxX) {
    sensorMaxX = sensorValueX;
  }
  // record the minimum sensor value
  if (sensorValueX < sensorMinX) {
    sensorMinX = sensorValueX;
  }

  // Y axis calibration
  sensorValueY = analogRead(yAxis);

  // record the maximum sensor value
  if (sensorValueY > sensorMaxY) {
    sensorMaxY = sensorValueY;
  }
  // record the minimum sensor value
  if (sensorValueY < sensorMinY) {
    sensorMinY = sensorValueY;
  }

  // Z axis calibration
  sensorValueZ = analogRead(zAxis);

  // record the maximum sensor value
  if (sensorValueZ > sensorMaxZ) {
    sensorMaxZ = sensorValueZ;
  }
  // record the minimum sensor value
  if (sensorValueZ < sensorMinZ) {
    sensorMinZ = sensorValueZ;
  }

  // Rx axis calibration
  sensorValueRx = analogRead(rXAxis);

  // record the maximum sensor value
  if (sensorValueRx > sensorMaxRx) {
    sensorMaxRx = sensorValueRx;
  }
  // record the minimum sensor value
  if (sensorValueRx < sensorMinRx) {
    sensorMinRx = sensorValueRx;
  }
      
  lastlooptime = millis() - looptime;

}

//------------------------------------------------end main loop

////// ----------------------------------------shiftIn function
///// just needs the location of the data pin and the clock pin
///// it returns a byte with each bit in the byte corresponding
///// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0

byte shiftIn(int myDataPin, int myClockPin) {
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);

  //we will be holding the clock pin high 8 times (0,..,7) at the
  //end of each time through the for loop

  //at the begining of each loop when we set the clock low, it will
  //be doing the necessary low to high drop to cause the shift
  //register's DataPin to change state based on the value
  //of the next bit in its serial information flow.
  //The register transmits the information about the pins from pin 7 to pin 0
  //so that is why our function counts down
  for (i = 7; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 0;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
      //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;

}

////// ----------------------------------------getBit
boolean getBit(byte myVarIn, byte whatBit) {
  boolean bitState;
  bitState = myVarIn & (1 << whatBit);
  return bitState;
}

////// A little extra function...
////// ----------------------------------------setBit
byte setBit(byte myVarIn, byte whatBit, boolean s) {
  boolean bitState;
  if (s) {
    myVarIn = myVarIn | (1 << whatBit);
  }
  else {
    myVarIn = myVarIn & ~(1 << whatBit);
  }
  return myVarIn;
}

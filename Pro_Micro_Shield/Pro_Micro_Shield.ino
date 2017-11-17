//**************************************************************//
//  Name    : RG Custom Pro Micro shield code                   //
//  Author  : Konstantin `RenderG` Yakushev                     //
//  Date    : 15 Nov, 2017                                      //
//  Version : 4.0                                               //
//          :                                                   //
//****************************************************************
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2); // lcd setup
#include <Joystick.h>
#include <Adafruit_NeoPixel.h>

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
int ENCtime[] = {4,4}; // Encoders timing in ms. Encrease for slower and decrease for faster. Тайминги энкодеров
unsigned long loopTime;


// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
//const bool testAutoSendMode = true;
const bool testAutoSendMode = false;

uint8_t hidReportId = 0x04; // Default: 0x03 - Indicates the joystick's HID report ID. This value must be unique if you are creating multiple instances of Joystick. Do not use 0x01 or 0x02 as they are used by the built-in Arduino Keyboard and Mouse libraries.
uint8_t buttonCount = 32; //Button count

//**************************************************
//        AXIS INPUTS SETUP / НАСТРОЙКА ОСЕЙ
//**************************************************

const int xAxis = A0;         // analog sensor for X axis
const int yAxis = A3;         // analog sensor for Y axis
const int zAxis = A2;         // analog sensor for Z axis
const int rXAxis = A1;        // analog sensor for rX axis
const int CalibrationPin = 8; // calibration toggle switch

// axis calibration variables:
int sensorValueX = 0;         // the sensor X value
int sensorValueY = 0;         // the sensor Y value
int sensorValueZ = 0;         // the sensor Z value
int sensorValueRx = 0;        // the sensor Rx value

int sensorMinX = 0;           // minimum sensor value
int sensorMaxX = 255;         // maximum sensor value
int sensorMinY = 0;           // minimum sensor value
int sensorMaxY = 255;         // maximum sensor value
int sensorMinZ = 0;           // minimum sensor value
int sensorMaxZ = 255;         // maximum sensor value
int sensorMinRx = 0;           // minimum sensor value
int sensorMaxRx = 359;         // maximum sensor value

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
unsigned long debounceDelay = 0;    // the debounce time; increase if the output flickers

//define an array that has a place for the values of

byte settingVal[] = {  1, 1, 1, 1, 1, 1, 1, 1};



void setup() {
  //start serial
 // Serial.begin(9600);

  //start Joystick
  Joystick.begin(false);

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
  lcd.print("Initialising");
  lcd.setCursor(0, 0);
  delay(300);
  lcd.print("Initialising.");
  lcd.setCursor(0, 0);
  delay(300);
  lcd.print("Initialising..");
  lcd.setCursor(0, 0);
  delay(300);
  lcd.print("Initialising...");
  lcd.setCursor(0, 0);
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  delay(300);
  lcd.print("-= RG CUSTOM =-");
  lcd.setCursor(0, 0);
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("BTN:");
  
}

void loop() {
//**************************************************
//              READING SHIFT REGISTERS
//**************************************************
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(latchPin, 1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
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
               Joystick.pressButton(i);
               PBstate[i] = 1;
               Joystick.sendState();
               lcd.setCursor(4, 1);
               lcd.print("ON ");
               }
              else {
               //action on button release
               Joystick.releaseButton(i);
               PBstate[i] = 0;
               Joystick.sendState();
               lcd.setCursor(4, 1);
               lcd.print("OFF");
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
               Joystick.pressButton(i + PBnum);
               Joystick.sendState();
               TGstate[i] = 1;
               delay(TGdelay);
               Joystick.releaseButton(i + PBnum);
               Joystick.sendState();
               }
              else {
               //action on button release
               Joystick.pressButton(i + PBnum);
               Joystick.sendState();
               delay(TGdelay);
               TGstate[i] = 0;
               Joystick.releaseButton(i + PBnum);
               Joystick.sendState();
             }
            }
          }
        lastState[TGreg[i]] = switchVar[TGreg[i]];
       }
// *************************************************
//     TOGGLE SWITCH SECTION ENDS HERE
// *************************************************     
 
// *************************************************
//     POV HATS SECTION
// *************************************************
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
               Joystick.sendState();
               }
              else {
               //action on button release
               Joystick.setHatSwitch(0, -1);
               Joystick.sendState();
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
     lastDebounceTime = millis();
     if(lastDebounceTime >= (loopTime)){ //Encoder time check
      ENCA[i] = bitRead(switchVar[ENCregA[i]], ENCpinA[i]);     // Reads encoder state A 
      ENCB[i] = bitRead(switchVar[ENCregB[i]], ENCpinB[i]);     // Reads encoder state B 
      if((!ENCA[i]) && (ENC_Aprev[i])){    // If state changes
       if(ENCB[i]) {
        //Encoder rotates A
       Joystick.setHatSwitch(1, (i * 90));
       Joystick.sendState();
       lcd.setCursor(13, 1);
       lcd.print(" ");
       lcd.print(i+1);
       lcd.print(">");
       }   
       else { 
        //Encoder rotates B
        Joystick.setHatSwitch(1, ((i * 90)+180));
       Joystick.sendState();
       lcd.setCursor(13, 1);
       lcd.print("<");
       lcd.print(i+1);
       lcd.print(" ");
       }   
      }   
     }
    ENC_Aprev[i] = ENCA[i];     // Save A state
   }
  loopTime = lastDebounceTime;
  if (lastDebounceTime % 15 == 0) {
       Joystick.setHatSwitch(1, -1);
       Joystick.sendState();
       lcd.setCursor(9, 1);
       lcd.print("ENC:   ");
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
  sensorValueX = map(sensorValueX, sensorMinX, sensorMaxX, -127, 127);
  sensorValueY = map(sensorValueY, sensorMinY, sensorMaxY, -127, 127);
  sensorValueZ = map(sensorValueZ, sensorMinZ, sensorMaxZ, -127, 127);
  sensorValueRx = map(sensorValueRx, sensorMinRx, sensorMaxRx, 0, 359);

  // in case the sensor value is outside the range seen during calibration
  sensorValueX = constrain(sensorValueX, -127, 127);
  sensorValueY = constrain(sensorValueY, -127, 127);
  sensorValueZ = constrain(sensorValueZ, -127, 127);
  sensorValueRx = constrain(sensorValueRx, 0, 359);

  Joystick.setXAxis(sensorValueX);
  Joystick.setYAxis(sensorValueY);
  Joystick.setZAxis(sensorValueZ);
  Joystick.setXAxisRotation(sensorValueRx);

  Joystick.sendState();

  int ledstrip = sensorValueX;
  ledstrip = map(ledstrip,127,-127,0,NUMPIXELS);
  // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  pixels.clear();
  pixels.show();
  for (int i = 0; i <= ledstrip; i++){
   pixels.setPixelColor(i-1, pixels.Color(10, 0, 0)); // RGB
    }
   pixels.show(); // This sends the updated pixel color to the hardware.
  


  
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

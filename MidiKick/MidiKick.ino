// -*-c++-*-
// MidiKick Your Ass!
//  This application has been designed to transform the signal coming
// from a Roland kick pedal into a MIDI note.
//
// Version: 1.0
// Author: Patrick E. Mazeau
// Features are:
// - possibility to change the note, the MIDI channel
// - possibility to choose Velocity curve (log, exp, linear)
// - possibility to fix max velocity
//
// This has been tested with the Roland kick pedal but could be adapted for other types of piezo
//
// Possible improvements:
// - memory: much too much
// - low kicks not taken into account: see if Arduino AD converted can be tweaked to increase resolution. The current
// signal after LP filter is 2.4 V peaks, not 5V as Arduino A0 input can convert
// - digital LP filter rather than electronic: had no time to experiement with that
//
// Licence:
// - Attribution 4.0 International (CC BY 4.0):
//   You are free to:
//     Share — copy and redistribute the material in any medium or format
//     Adapt — remix, transform, and build upon the material for any purpose, even commercially.
//   Under the following terms:
//     Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
//            You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
//     No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
//


#include <MemoryFree.h>
#include <math.h>
#include <Wire.h>
#include <Time.h>
#include <avr/wdt.h>

// using this RGB library linked to the Grove 16X2 LCD I got from seedstudio wiki.seedstudio.com
#include <rgb_lcd.h>

// my own tools for UI manipulation, not a library et
#include "ElementUI.h"
#include "UIMidiKick.h"
#include "Memory.h"

// versionSoftware of Midikick, eraseed when starting
char versionSoftware [] = "MidiKick 1.0";

// My LCD version is only RED, so possibility to dim erase. 
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// variable for EEPROM memory
EEmemory * myMemory = 0;

// strange, need to use an intermediate variable otherwise get compile error with static variable
rgb_lcd lcd;
rgb_lcd *toto = &lcd;
rgb_lcd  *BaseVisu::myLcd = toto;
BaseVisu *BaseVisu::activeItem = 0;

// 3 UI push buttons: 
// - PIN 2 Set
// - PIN 3 Right/-
// - PIN 4 Left/+
#define pinButtonSet 2
#define pinButtonMinus 3
#define pinButtonPlus 4

// ts keeps track of time when Arduino boots
unsigned long ts = millis();

// line1 and line2 used as buffers for 2 lines of LCD
char ligne1[17];
char ligne2[17];

// clearline: cleans line linno on LCD
void clearLine (int linno);

// additional characters for LCD: menuIcon, VuMeter

byte backslashIcon[8]={
		   B00000,
		   B10000,
		   B01000,
		   B00100,
		   B00010,
		   B00001,
		   B00000,
		   B00000,
};

byte menuIcon[8]={
		   B11111,
		   B10001,
		   B11111,
		   B10001,
		   B11111,
		   B10001,
		   B11111,
		   B00000,
};

byte logIcon[8]={
		   B00111,
		   B01000,
		   B01000,
		   B10000,
		   B10000,
		   B10000,
		   B10000,
		   B00000,
};
		 
byte expIcon[8]={
		   B00001,
		   B00001,
		   B00001,
		   B00001,
		   B00010,
		   B00010,
		   B11100,
		   B00000,
};

byte linIcon[8]={
		   B00000,
		   B00001,
		   B00010,
		   B00100,
		   B01000,
		   B10000,
		   B00000,
		   B00000,
};

byte vm1Icon[8]={
		   B00000,
		   B11111,
		   B11111,
		   B11111,
		   B11111,
		   B11111,
		   B11111,
		   B00000,
};

byte vm2Icon[8]={
		   B11000,
		   B11000,
		   B11000,
		   B11000,
		   B11000,
		   B11000,
		   B11000,
		   B11000,
};

byte vm3Icon[8]={
		   B00000,
		   B00000,
		   B00000,
		   B00000,
		   B00000,
		   B00000,
		   B00000,
		   B00000,
};


// Pin used for kick AD acquisition
#define kickPin  0

// analog threshold for kick sensing
#define PIEZOTHRESHOLD 10

// max value of kick after filtering, what I measured with the Roland kick
int maxPiezo = 220 ;

// variables for dealing with acquisition loop, threshold, slope detection
int val = 0;
float f = 0;
int maxFrappe = 0;
bool noteStatus = false;
// values contains the first sizeValues, for future use like digital LP filtering, or slope detection, or debug 
#define sizeValues 50
int values [sizeValues];
int i=0;
int k=0;
bool bassStart = false;
int numDescentes = 0;
int vuMeterDelay = 0;

// mainWindow: keeps track of the start screen
MainWindow * mainWindow = 0;


void setup() {
  // variable to check if it is first use, or request for reset
  bool estResetUsine = false;

  // MIDI INIT, set MIDI speed
  Serial.begin(31250);
  // for debugging uncomment following line, we print results on serial window
  // Serial.begin(38400);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  // creation of backslash character
  lcd.createChar(0, backslashIcon);
  // creation menu character
  lcd.createChar(1, menuIcon);
  // creation characters for velocity curves
  lcd.createChar(2, logIcon);
  lcd.createChar(3, expIcon);
  lcd.createChar(4, linIcon);
  // creation characters for vumeter
  lcd.createChar(5, vm1Icon);
  lcd.createChar(6, vm2Icon);
  lcd.createChar(7, vm3Icon);

  //***************************************************
  // EEPROM initialisation
  // - default values stored in EEPROM if first use, or factory reset
  // - adresses of EEPROM stored in myMemory
  estResetUsine = EEmemory::isNew();

  // check first use / factory reset, erase messages
  if ( estResetUsine ) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WELCOME!");
    lcd.setCursor(0,1);
    lcd.print("FACTORY RESET");
    delay (2000);
  }
  
  // Print a message to the LCD.
  lcd.clear();
  lcd.print(versionSoftware);
  lcd.setCursor(0,1);
  lcd.print("Init...");
  delay (2000);

  myMemory = new EEmemory();

  // GUI initialisation
  
  // button initialisation
  pinMode(pinButtonSet,INPUT_PULLUP);
  pinMode(pinButtonPlus,INPUT_PULLUP);
  pinMode(pinButtonMinus,INPUT_PULLUP);

  ts = millis();
  
  // WATCHDOG configuration
  // 8 seconds
  cli();
  wdt_reset();
  /*
    WDTCSR configuration:
    WDIE = 1: Interrupt Enable
    WDE = 1 :Reset Enable
    See table for time-out variations:
    WDP3 = 0 :For 2000ms Time-out
    WDP2 = 1 :For 2000ms Time-out
    WDP1 = 1 :For 2000ms Time-out
    WDP0 = 1 :For 2000ms Time-out
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) |
    (1<<WDP3) | (0<<WDP2) | (0<<WDP1) |
    (1<<WDP0);
  sei();

  // seems to be needed after factory reset...
  delay(100);
  
  // Global windows initialisation
  mainWindow = new MainWindow ();
  BaseVisu::activeItem = mainWindow;
  mainWindow->display();
}

void loop() {
  // ***********************
  // MIDI part
  //

  // char buf[10]; // for debug
   
  int velocity = 0;

  // kick value acquisition
  val = analogRead(kickPin);

  // experiemental: every X count we display something on VuMeter, otherzise too slow
  if ( vuMeterDelay == 210 ) {
    if (  BaseVisu::activeItem == mainWindow )
      mainWindow->itemVuMeter->display();
    vuMeterDelay=0;
  }
  else
    vuMeterDelay++;
  
  // kick is >= Threshold and we did not start acquisition yet
  if( val >= PIEZOTHRESHOLD && !bassStart) {
    bassStart = true ;
    maxFrappe= 0;
    noteStatus = false;
    i = 0;
  }
    
  // kick acquisition has started and still >= Threshold
  if( val >= PIEZOTHRESHOLD && bassStart) {
    // The kick curve has a peak, then oscillations
    // - when we start to see a descent, we have reached the max velocity and we send NoteOn
    // - we wait until we go below Threshold
    // - we store values in values to compute descent, i is the current index

    //sprintf(buf,"%i",val);
    //Serial.println(buf);
    if ( i < sizeValues) {
      values [i] = val;
      if ( val > maxFrappe) {
	maxFrappe = val;
      }
      if ( sensPente (i) == -1 ) {
	// First descent, we reached maximum, on send NOTE ON
	if ( numDescentes == 0 ) {
	  numDescentes++;
	  // we check the type of velocity curve and compute the right velocity value
	  if ( myMemory->curbVelocity.get() == VELOCITYLOG)
	    velocity = (int)(log10 (maxFrappe) / log10 (maxPiezo) * myMemory->maxVelocity.get());
	  if ( myMemory->curbVelocity.get() == VELOCITYEXP)
	    velocity =(int)(pow(2.71828, ((float)maxFrappe) / maxPiezo * log (myMemory->maxVelocity.get())));
	  if ( myMemory->curbVelocity.get() == VELOCITYLIN)
	    velocity =(int)(((float)maxFrappe) / maxPiezo * myMemory->maxVelocity.get());
	  
	  if ( velocity > myMemory->maxVelocity.get())
	    velocity = myMemory->maxVelocity.get();
	  noteOn(myMemory->midiCh.get()-1,myMemory->midiNote.get(), velocity );
	  noteStatus = true;
	  //char buf[10];
	  // snprintf(buf,10,"%i",val);
	  // Serial.println(buf);

 	}
      }
      i++;
    }
  }
    
  // acquisition< Threshold, and we started acquiring, we send NOTE OFF
  if( val < PIEZOTHRESHOLD && bassStart) {
    // snprintf(buf,10,"%i",val);
    // Serial.println(buf);

    if (noteStatus) {
      noteOff(myMemory->midiCh.get()-1,myMemory->midiNote.get(), 0);
    }
    bassStart = false ;
    numDescentes = 0;

    // short delay (experiemental) to avoid additional oscillations
    delay(50);
  }

  
  //************************
  // GUI part
  // We check which button has been called, send appropirate method to active GUI item (window, menu etc)

  // set button
  if (digitalRead(pinButtonSet) == LOW) {
    if ( BaseVisu::activeItem ) {
      BaseVisu::activeItem->set();
    }
    delay(200);
  }
  // PLUS button
  if(digitalRead(pinButtonPlus)==LOW) {
    if ( BaseVisu::activeItem) {
      BaseVisu::activeItem->plus();
    }
    delay(200);
  }
  
  // MINUS button
  if(digitalRead(pinButtonMinus)==LOW) {
    if ( BaseVisu::activeItem) {
      BaseVisu::activeItem->minus();
    }
    delay(200);
  }

  wdt_reset();
}

// Send a MIDI note-on message
void noteOn(byte channel, byte note, byte velocity) {
  // for debugging
  //midiMsg2( (0x90 | channel), note, velocity);
  midiMsg( (0x90 | channel), note, velocity);
}

// Send a MIDI note-off message
void noteOff(byte channel, byte note, byte velocity) {
  // for debugging
  //midiMsg2( (0x80 | channel), note, velocity);
  midiMsg( (0x80 | channel), note, velocity);
}

// Send a MIDI message
void midiMsg(byte cmd, byte data1, byte data2) {
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

// Print a MIDI message
void midiMsg2(byte cmd, byte data1, byte data2) {
   Serial.println( "Commande MIDI:");
   Serial.print(cmd, HEX);
   Serial.print(" ");
   Serial.print(data1, HEX);
   Serial.print(" ");
   Serial.print(data2, HEX);
   Serial.println( " ");
}

// sensPente: function computing the curve direction over n number of samples
// 1: UP
// -1: DOWN
// 0: not significant
int sensPente (int ind1) {
  if ( ind1 < 1 )
    return 0;
  if ( values [ind1] > values [ind1 -1])
    return 1;
  if ( values [ind1] <= values [ind1 -1])
    return -1;
  return 0;
  // if ( ind1 < 3 )
  //   return 0;
  // if ( values [ind1] > values [ind1 -1] &&
  //      values [ind1 -1] > values [ind1 -2] &&
  //      values [ind1 -2] > values [ind1 -3])
  //   return 1;
  // if ( values [ind1] < values [ind1 -1] &&
  //      values [ind1 -1] <values [ind1 -2] &&
  //      values [ind1 -2] < values [ind1 -3])
  //   return -1;
  // return 0;
}
 

// clearLine: clears line linno on LCD
void clearLine (int linno) {
    lcd.setCursor (0,linno);
    lcd.print (F("                "));
    lcd.setCursor (0,linno);
}

// interruption called in case of WATCHDOG
ISR (WDT_vect) {
  lcd.setCursor(0,1);
  lcd.print ("RESET!!!!       ");
}



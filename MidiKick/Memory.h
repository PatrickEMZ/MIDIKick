// -*-c++-*-
// Memory: class to manage access to EEPROM
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

#ifndef MEMORY_H
#define MEMORY_H

#include <EEPROM.h>

// default values for stored variables
// midi channel
#define MIDICH 10
// midi note for kick
#define MIDINOTE 35
// max velocity
#define MAXVELOCITY 127
// types of velocity curves
#define VELOCITYLOG 0
#define VELOCITYLIN 1
#define VELOCITYEXP 2
// default velocity
#define VELOCITYDEFAULT VELOCITYLOG
// MAGIC number for checking FACTORY RESET
#define MAGIC 16712


// classes to manage EEPROM variables
// Int
class EEint {
public:
  EEint (){};
  int adresse;
  virtual set(int _val){EEPROM.put (adresse, _val);};
  virtual int get (){int val=0;EEPROM.get (adresse, val); return val;};
};

// Long
class EElong {
public:
  EElong(){};
  int adresse;
  virtual set(long _val){EEPROM.put (adresse, _val);};
  virtual long get (){long val=0;EEPROM.get (adresse, val); return val;};
};

// memory class
class EEmemory {
public:
  EEmemory();
  static bool isNew ();
  init();
  initAdresses();
  reset();
  resetFactory();
  EEint midiCh;
  EEint midiNote;
  EEint maxVelocity;
  EEint curbVelocity;
};

#endif  

// -*-c++-*-

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

  

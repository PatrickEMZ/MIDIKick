// -*-c++-*-

#include <Arduino.h>
#include <EEPROM.h>
#include "Memory.h"
#include <avr/wdt.h>
#include "rgb_lcd.h"

extern rgb_lcd lcd;


EEmemory::EEmemory() {
  // test si besoin d'initialisation de la memory EEPROM
  // le premier int doit valoir 16712, cela veut dire qu'elle est initialisee

  EEint sequenceDebut;
  sequenceDebut.adresse = 0;

  if ( sequenceDebut.get() != MAGIC ) {
    lcd.clear();
    lcd.print("INIT EEPROM");
    // Serial.println("INIT EEPROM");
      
    // on initialise l'EEPROM
    sequenceDebut.set ( MAGIC );

    // initialisation des valeurs et ecriture en EEPROM
    init ();
  }
  else {
    // initialisation des adresses
    initAdresses();
  }
};  

boolean EEmemory::isNew (){
  EEint sequenceDebut;
  sequenceDebut.adresse = 0;

  if ( sequenceDebut.get() != MAGIC )
    return true;
  else
    return false;
};



EEmemory::init() {
  lcd.clear();
  lcd.print("EE::init");
  // Serial.println("EE::init");
  // initialisation des adresses
  initAdresses ();
  
  // on initialise les valeurs par defaut
  midiCh.set (MIDICH);
  midiNote.set (MIDINOTE);
  maxVelocity.set (MAXVELOCITY);
  curbVelocity.set (VELOCITYDEFAULT);
}

EEmemory::initAdresses() {
  lcd.clear();
  lcd.print("EE::initAdr");
  // Serial.println("EE::initAdr");
  // on initialise les valeurs par defaut
  midiCh.adresse = sizeof (int); // on demarre la premiere adresse apres la sequence de debut contenant MAGIC (int)
  midiNote.adresse = sizeof (int) + midiCh.adresse;
  maxVelocity.adresse =  sizeof (int) + midiNote.adresse;
  curbVelocity.adresse =  sizeof (int) + maxVelocity.adresse;
}

EEmemory::reset (){
  lcd.clear();
  lcd.print("EEmemory::reset");
  
  // forcer le reset
  wdt_enable(WDTO_60MS);
  while (1){};
}

// remise a etat usine, on efface MAGIC de l'EEPROM
EEmemory::resetFactory (){
  lcd.clear();
  lcd.print("EEmemory::resetFactory");

  EEint sequenceDebut;
  sequenceDebut.adresse = 0;
  sequenceDebut.set (0);

  // forcer le reset
  wdt_enable(WDTO_60MS);
  while (1){};
}

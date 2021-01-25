// -*-c++-*-
// UI for MidiKick
// Licence:
// - Attribution 4.0 International (CC BY 4.0):
//   You are free to:
//     Share ― copy and redistribute the material in any medium or format
//     Adapt ― remix, transform, and build upon the material for any purpose, even commercially.
//   Under the following terms:
//     Attribution ― You must give appropriate credit, provide a link to the license, and indicate if changes were made.
//            You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
//     No additional restrictions ― You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
//

#ifndef UIMIDIKICK_H
#define UIMIDIKICK_H

#include <Arduino.h>
#include <rgb_lcd.h>
#include <LinkedList.h>
#include "ElementUI.h"


//=============================================================
class ItemOK :public ItemDialog {
 public:
  ItemOK (Wwindow* _wwindow, int _pos) :ItemDialog (_wwindow, _pos) {};
  void display(){myLcd->setCursor(pos,1); myLcd->write('v');};
  void set();
};

//=============================================================
class ItemKO :public ItemDialog {
 public:
  ItemKO (Wwindow* _wwindow, int _pos) :ItemDialog (_wwindow, _pos){};
  void display(){myLcd->setCursor(pos,1); myLcd->write('x');};
  void set();
};

//=============================================================
class ItemInt :public ItemEditable {
 public:
  int valeur;
  ItemInt (Wwindow* _wwindow, int _pos, int _valeur) :ItemEditable (_wwindow, _pos){valeur = _valeur;};
  virtual void setValeur(int _valeur){valeur = _valeur;};
  virtual int getValeur(){return valeur;};
};

//=============================================================
class ItemMidiCh :public ItemInt {
public:
  ItemMidiCh (Wwindow* _wwindow, int _pos, int _valeur) :ItemInt (_wwindow, _pos, _valeur){};
  void display();
  void plus();
  void minus();
  void set();
};

//=============================================================
class ItemNote :public ItemInt {
public:
  ItemNote (Wwindow* _wwindow, int _pos, int _valeur) :ItemInt (_wwindow, _pos, _valeur){};
  void display();
  void plus();
  void minus();
  void set();
  static void midiToNote (int midiNote, char* noteChar, int nChar);
  int noteToMidi (char* charNote);
};

//=============================================================
class ItemVelocity :public ItemInt {
public:
  ItemVelocity (Wwindow* _wwindow, int _pos, int _valeur) :ItemInt (_wwindow, _pos, _valeur){};
  void display();
  void plus();
  void minus();
};

//=============================================================
class ItemTypeVelocity :public ItemInt {
public:
  ItemTypeVelocity (Wwindow* _wwindow, int _pos, int _valeur) :ItemInt (_wwindow, _pos, _valeur){};
  void display();
  void plus();
  void minus();
  void set();
};

//=============================================================
class ItemIntLimite :public ItemInt {
 public:
  int valMin;
  int valMax;
  char *aspect;
  ItemIntLimite (Wwindow* _wwindow, int _pos, int _val, int _min, int _max, char* _aspect);
  void display();
  void plus();
  void minus();
};

//=============================================================
class ItemBouton :public ItemDialog {
 public:
  int noChar;
  ItemBouton (Wwindow* _wwindow, int _pos, int _noChar) :ItemDialog (_wwindow, _pos){noChar=_noChar;};
  void display();
  void set();
};

//=============================================================
// span: number of characters VuMeter occupies on screen, first character is a one letter label, last character used to show peak
// peak: keep track of peak value
// delayDescent: counter to have a smooth descent of peak on screen
class ItemVuMeter :public ItemInt {
 public:
  ItemVuMeter (Wwindow* _wwindow, int _pos, int _span, int _valeur) :ItemInt (_wwindow, _pos, _valeur){span = _span; peak=0; delayDescent=10;};
  int span;
  int peak;
  int delayDescent;
  void display();
};

//=============================================================
class WwindowAvecRetour :public Wwindow {
public:
  void erase ();
};

//=============================================================
class WwindowSansRetour :public Wwindow {
public:
  void erase ();
};

// //=============================================================
// class WwindowMidi :public WwindowSansRetour {
// public:
//   WwindowMidi ();
//   ItemMidiCh *itemMidiCh;
//   ItemOK *itemOK;
//   ItemKO *itemKO;
//   void display();
//   int validate();
// };

// //=============================================================
// class WwindowNote :public WwindowSansRetour {
// public:
//   WwindowNote ();
//   ItemNote *itemNote;
//   ItemOK *itemOK;
//   ItemKO *itemKO;
//   void display();
//   int validate();
// };

//=============================================================
class WwindowVelocity :public WwindowSansRetour {
public:
  WwindowVelocity ();
  ItemVelocity *itemVelocity;
  ItemOK *itemOK;
  ItemKO *itemKO;
  void display();
  int validate();
};

//=============================================================
class MenuItemBase;

class MainWindow :public Wwindow {
public:
  MainWindow ();
  ItemNote *itemNote;
  ItemMidiCh *itemMidiCh;
  // ItemText *noteStatus;
  ItemBouton *boutonConf;
  MenuItemBase *menuConf;
  ItemVuMeter *itemVuMeter;
  ItemTypeVelocity *itemTypeVelocity;
  void display();
  int validate() {};
};

//=============================================================
class MenuItemBase :public MenuToWwindow {
 public:
  MenuItemBase ( MainWindow *_mainWindow);
  MainWindow *mainWindow;
  void display();
};

//=============================================================
class MenuExit :public MenuItem {
 public:
  MenuExit (char* _texte) :MenuItem(_texte){};
  void erase();
  void set() {erase();}
};

//=============================================================
class WwindowReset :public WwindowSansRetour {
public:
  WwindowReset ();
  ItemText *itemText1;
  ItemOK *itemOK;
  ItemKO *itemKO;
  void display();
  int validate();
};

//=============================================================
class WwindowResetUsine :public WwindowReset {
public:
  WwindowResetUsine ();
  int validate();
};





#endif

  

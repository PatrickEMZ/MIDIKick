// -*-c++-*-
// ElementUI Generic classes to manage GUI on Arduino + LCD display
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

#ifndef ELEMENTUI_H
#define ELEMENTUI_H

#include <Arduino.h>
#include <rgb_lcd.h>

#include <LinkedList.h>

// base class for all dialog/window elements
class BaseVisu {
public:
  BaseVisu(){};
  static rgb_lcd *myLcd;
  // static BaseVisu *itemActif;
  static BaseVisu *activeItem;
  virtual void display(){}; 
  virtual void erase(){};
  virtual void plus(){};
  virtual void minus(){};
  virtual void set(){};
};

class ItemDialog;
class MenuItem;

class Wwindow :public BaseVisu {
public:
  Wwindow(){labelActive = 0; menuActive = 0;};
  ItemDialog *labelActive;
  // menuActive contient le menu qui a affiché la wwindow
  MenuItem *menuActive;
  void plus();
  void minus();
  void set();
  virtual int validate(){};
  virtual void cancel(){};
};

class ItemDialog :public BaseVisu {
 public:
  ItemDialog (Wwindow* _wwindow, int _pos){pos=_pos; edition = false; right = 0; left=0; wwindow=_wwindow;};
  int pos;
  bool edition; // mode edition ou pas
  ItemDialog* right;
  ItemDialog* left;
  Wwindow* wwindow; 
  void plus(){if (right) { wwindow->labelActive = right; myLcd->setCursor(right->pos,1);}};
  void minus(){{if (left) { wwindow->labelActive = left; myLcd->setCursor(left->pos,1);}}};
  void set(){};
};

class ItemLabel :public ItemDialog {
 public:
  char label;
  ItemLabel (Wwindow* _wwindow, int _pos, char x) :ItemDialog (_wwindow, _pos){label=x;};
  void display();
};

class ItemText :public ItemDialog {
 public:
  char *label;
  ItemText (Wwindow* _wwindow, int _pos, char *_label);
  void setLabel (char *_label);
  void display();
};


class ItemEditable :public ItemDialog {
 public:
  ItemEditable (Wwindow* _wwindow, int _pos) :ItemDialog (_wwindow, _pos){};
  void set();
};

class ItemList :public ItemEditable {
public:
  ItemList (Wwindow* _wwindow, int _pos, LinkedList<char*> *_listTextOptions);
  LinkedList<char*> *listTextOptions;
  void display();
  void plus();
  void minus();
  void reset(){currentItem=0;};
  int currentItem;
};

class MenuItem :public BaseVisu {
public:
  MenuItem(char* _texte);
  char texte[17];
  MenuItem *right;
  MenuItem *left;
  MenuItem *up;
  MenuItem *down;
  void set();
  void plus(){if (right) {BaseVisu::activeItem = right; BaseVisu::activeItem->display();}};
  void minus(){if (left) {BaseVisu::activeItem = left; BaseVisu::activeItem->display();}};
  void display(){BaseVisu::activeItem = this; myLcd->setCursor(0,1); myLcd->print(F("                ")); myLcd->setCursor(0,1); myLcd->print(texte);}
  void erase() {  BaseVisu::activeItem = 0; myLcd->setCursor (0,1); myLcd->print (F("                ")); myLcd->setCursor (0,1);};
  virtual void displayUp();
};

class MenuToWwindow :public MenuItem {
public:
  MenuToWwindow(char* _texte, Wwindow *_wwindowMenu) :MenuItem (_texte) {wwindowMenu = _wwindowMenu; if (wwindowMenu) wwindowMenu->menuActive = this;};
  Wwindow *wwindowMenu;
  void set();
};


#endif

  

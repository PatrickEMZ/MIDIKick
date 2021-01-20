// -*-c++-*-

#include "ElementUI.h"



void ItemEditable::set(){
  myLcd->setCursor(pos,1);
  if (edition) {
    // on passe en mode fin edition
    myLcd->noBlink();
    myLcd->cursor();
  }
  else {
    // on passe en mode edition
    myLcd->noCursor();
    myLcd->blink();
  }
  edition = !edition;
};

MenuItem::MenuItem(char* _texte){
  if (_texte) {
    strncpy(texte, _texte, 16);
    texte[17] = NULL;
  }
  right = 0;
  left = 0;
  up = 0;
  down = 0;
}

void MenuItem::set(){
  if (down){
    // on affiche le menu courant sur la ligne superieure
    displayUp();
    down->display();
  }
};

void MenuItem::displayUp(){
  // on ecrit le nom du menu sur la premier ligne du LCD
  myLcd->setCursor(0,0);
  myLcd->print(F("                "));
  myLcd->setCursor(0,0);
  myLcd->print(texte);
};


void Wwindow::plus() {
  if (labelActive) labelActive->plus();
};

void Wwindow::minus(){
  if (labelActive) labelActive->minus();
};

void Wwindow::set(){
  if (labelActive) labelActive->set();
};


void ItemLabel::display(){
  myLcd->setCursor(pos,1);
  myLcd->write(label);
}

void ItemText::display(){
  myLcd->setCursor(pos,1);
  myLcd->print(label);
}

ItemText::ItemText (Wwindow* _wwindow, int _pos, char *_label) :ItemDialog (_wwindow, _pos){
  if (_label) {
    label = new char[strlen (_label) +1 ];
    strcpy (label, _label) ;
  }
  else
    label = 0;
};

void ItemText::setLabel(char *_label){
  if (label && _label) {
    snprintf(label, strlen(label)+1,"%s", _label);
  }
};


ItemList::ItemList (Wwindow* _wwindow, int _pos, LinkedList<char*> *_listTextOptions) :ItemEditable (_wwindow, _pos){
  listTextOptions = _listTextOptions;
  currentItem=0;
};


void ItemList::display() {
  myLcd->setCursor(pos,1);
  myLcd->print(listTextOptions->get(currentItem));
  myLcd->setCursor(pos,1);
};

void ItemList::plus(){
  char ligne []="                ";
  if (edition) {
    myLcd->setCursor (pos,1);
    ligne[strlen(listTextOptions->get(currentItem))]='\0';
    myLcd->print(ligne);
    myLcd->setCursor (pos,1);
    currentItem++;
    if (currentItem >= listTextOptions->size())
      currentItem=0;
    display();
  }
  else
    ItemDialog::plus();
};

void ItemList::minus(){
  char ligne []="                ";
  if (edition) {
    myLcd->setCursor (pos,1);
    ligne[strlen(listTextOptions->get(currentItem))]='\0';
    myLcd->print(ligne);
    myLcd->setCursor (pos,1);
    currentItem--;
    if (currentItem < 0)
      currentItem=listTextOptions->size()-1;
    display();
  }
  else
    ItemDialog::minus();
};


void MenuToWwindow::set(){
  if (wwindowMenu) {
    myLcd->setCursor(0,0);
    myLcd->print(F("                "));
    myLcd->setCursor(0,0);
    myLcd->print(texte);
    BaseVisu::activeItem = wwindowMenu;
    wwindowMenu->display();
  }
};

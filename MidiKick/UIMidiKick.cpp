// -*-c++-*-

#include <MemoryFree.h>
#include <TimeLib.h>
#include <avr/wdt.h>
#include "Memory.h"
#include "UIMidiKick.h"

// global variables
extern EEmemory *myMemory;
extern char versionSoftware [];
extern void noteOff(byte channel, byte note, byte velocity);
extern void noteOn(byte channel, byte note, byte velocity);
extern bool bassStart;
extern int numDescentes;
extern int val;
extern int maxPiezo;
extern char ligne2[17];


void ItemKO::set(){
  wwindow->erase();
}

void ItemOK::set(){
  if ( wwindow->validate() )
    wwindow->erase();
}
//=========================================================
void ItemMidiCh::display(){
  char midiChTexte[3];
  myLcd->setCursor(pos,1);
  sprintf(midiChTexte, "%02d",valeur);
  myLcd->print(midiChTexte);
  myLcd->setCursor(pos,1);
}

void ItemMidiCh::plus(){
  if (edition) {
    valeur++;
    if (valeur == 17) valeur =1;
    display();
  }
  else
    ItemDialog::plus();
};

void ItemMidiCh::minus(){
  if (edition) {
    valeur--;
    if (valeur == 0) valeur =16;
    display();
  }
  else
    ItemDialog::minus();
};

void ItemMidiCh::set(){
  myLcd->setCursor(pos,1);
  if (edition) {
    // on passe en mode fin edition
    myLcd->noBlink();
    myLcd->cursor();
    // on sauve la valeur en EEPROM
    myMemory->midiCh.set (valeur);
  }
  else {
    // on passe en mode edition
    // on envoie un note off au cas ou
    if( bassStart ) {
      noteOff(myMemory->midiCh.get()-1,myMemory->midiNote.get(), 0);    
      bassStart = false ;
      numDescentes = 0;
    }
    myLcd->noCursor();
    myLcd->blink();
  }
  edition = !edition;
};


//=========================================================
void ItemNote::display(){
  char noteChar [] = "C#10";
  ItemNote::midiToNote (valeur, noteChar, sizeof(noteChar));
  myLcd->setCursor(pos,1);
  myLcd->print(noteChar);
  myLcd->setCursor(pos,1);
}

void ItemNote::plus(){
  if (edition) {
    valeur++;
    if (valeur == 128) valeur =0;
    display();
  }
  else
    ItemDialog::plus();
};

void ItemNote::minus(){
  if (edition) {
    valeur--;
    if (valeur == -1) valeur =127;
    display();
  }
  else
    ItemDialog::minus();
};

void ItemNote::set(){
  myLcd->setCursor(pos,1);
  if (edition) {
    // on passe en mode fin edition
    myLcd->noBlink();
    myLcd->cursor();
    // on sauvegarde la valeur dans la memory
    myMemory->midiNote.set (valeur);
  }
  else {
    // on passe en mode edition
    // on envoie un noteoff au cas ou
    if( bassStart ) {
      noteOff(myMemory->midiCh.get()-1,myMemory->midiNote.get(), 0);    
      bassStart = false ;
      numDescentes = 0;
    }
    myLcd->noCursor();
    myLcd->blink();
  }
  edition = !edition;
};


void ItemNote::midiToNote (int midiNote, char *noteChar, int nChar) {
  if ((midiNote >= 0) && (midiNote < 128) && (noteChar != 0) && (nChar >= 5 ))
    {
      switch ( midiNote%12 )
  	{
  	case 0:
	  snprintf(noteChar, nChar, "C%i ", (int)(midiNote/12) ); break;
  	case 1:
	  snprintf(noteChar, nChar, "C#%i", (int)(midiNote/12) ); break;
  	case 2:
	  snprintf(noteChar, nChar, "D%i ", (int)(midiNote/12) ); break;
  	case 3:
	  snprintf(noteChar, nChar, "D#%i", (int)(midiNote/12) ); break;
        case 4:
	  snprintf(noteChar, nChar, "E%i ", (int)(midiNote/12) ); break;
        case 5:
	  snprintf(noteChar, nChar, "F%i ", (int)(midiNote/12) ); break;
        case 6:
	  snprintf(noteChar, nChar, "F#%i", (int)(midiNote/12) ); break;
        case 7:
	  snprintf(noteChar, nChar, "G%i ", (int)(midiNote/12) ); break;
        case 8:
	  snprintf(noteChar, nChar, "G#%i", (int)(midiNote/12) ); break;
        case 9:
	  snprintf(noteChar, nChar, "A%i ", (int)(midiNote/12) ); break;
        case 10:
	  snprintf(noteChar, nChar, "A#%i", (int)(midiNote/12) ); break;
        case 11:
	  snprintf(noteChar, nChar, "B%i ", (int)(midiNote/12) ); break;
  	}
    }
}
// format note is C1 or C#1 until C#10
int ItemNote::noteToMidi (char* charNote) {
  char note = 'C';
  char modif = '#';
  int midiNote = 0;
  // # est 35, 0 est 48, 1 49 etc
  if (charNote != 0)
    {
      switch ( charNote[0] )
	{
	case 'C': midiNote = 0; break;
	case 'D': midiNote = 2; break;
	case 'E': midiNote = 4; break;
	case 'F': midiNote = 5; break;
        case 'G': midiNote = 7; break;
        case 'A': midiNote = 9; break;
        case 'B': midiNote = 11; break;
	}
      if ( charNote[1] == '#')
	{
	  midiNote++;
	  midiNote = midiNote + 12 * (charNote[2] - '0');
	}
      else
	  midiNote = midiNote + 12 * (charNote[1] - '0');
    }
  return midiNote;
}

//=========================================================
void ItemVelocity::display(){
  char texte[5];
  myLcd->setCursor(pos,1);
  snprintf(texte, sizeof(texte), "%03d", valeur);
  myLcd->print(texte);
  myLcd->setCursor(pos,1);
}

void ItemVelocity::plus(){
  if (edition) {
    valeur++;
    if (valeur == 128) valeur =1;
    display();
  }
  else
    ItemDialog::plus();
};

void ItemVelocity::minus(){
  if (edition) {
    valeur--;
    if (valeur == 0) valeur =127;
    display();
  }
  else
    ItemDialog::minus();
};

//=========================================================
void ItemTypeVelocity::display(){
  uint8_t noChar = 0;
  myLcd->setCursor(pos,1);
  myLcd->print('V');
  myLcd->setCursor(pos+1,1);
  if ( valeur == VELOCITYLOG ) noChar = 2;
  if ( valeur == VELOCITYEXP ) noChar = 3;
  if ( valeur == VELOCITYLIN ) noChar = 4;
  myLcd->write (noChar);
  myLcd->setCursor(pos+1,1);
}

void ItemTypeVelocity::plus(){
  if (edition) {
    valeur++;
    if (valeur == VELOCITYEXP +1 ) valeur = VELOCITYLOG;
    display();
  }
  else
    ItemDialog::plus();
};

void ItemTypeVelocity::minus(){
  if (edition) {
    valeur--;
    if (valeur == VELOCITYLOG - 1) valeur = VELOCITYEXP;
    display();
  }
  else
    ItemDialog::minus();
};

void ItemTypeVelocity::set(){
  myLcd->setCursor(pos,1);
  if (edition) {
    // on passe en mode fin edition
    myLcd->noBlink();
    myLcd->cursor();
    // on sauve la valeur en EEPROM
    myMemory->curbVelocity.set (valeur);
  }
  else {
    // on passe en mode edition
    // on envoie un note off au cas ou
    if( bassStart ) {
      noteOff(myMemory->midiCh.get()-1,myMemory->midiNote.get(), 0);    
      bassStart = false ;
      numDescentes = 0;
    }
    myLcd->noCursor();
    myLcd->blink();
  }
  edition = !edition;
};

//=========================================================
void ItemBouton::display(){
  myLcd->setCursor(pos,1);
  myLcd->write((uint8_t) noChar);
  myLcd->setCursor(pos,1);
}

void ItemBouton::set(){
  myLcd->clear ();
  myLcd->setCursor(0,0);
  myLcd->print(strcpy(ligne2,"Configuration"));
  myLcd->noCursor();
  BaseVisu::activeItem = ((MainWindow*)(wwindow))->menuConf;
  BaseVisu::activeItem->display();
};

//=========================================================

void WwindowAvecRetour::erase() {
  myLcd->setCursor (0,1);
  myLcd->print (F("                "));
  myLcd->setCursor (0,1);
  myLcd->noCursor();
  labelActive = 0;
  
  // on reaffiche le menu si on a ete appele par un menu
  if ( menuActive ) {
    BaseVisu::activeItem = menuActive;
    // on reaffiche le menu du dessus egalement
    if (menuActive->up)
      menuActive->up->displayUp();
    menuActive->display();
  }
  else {
    BaseVisu::activeItem = 0;
    labelActive = 0;
  }
};

//=========================================================

void WwindowSansRetour::erase() {
  myLcd->clear();
  myLcd->noCursor();
  if (menuActive) {
    BaseVisu::activeItem = ((MenuItemBase*)(menuActive))->mainWindow;
    BaseVisu::activeItem->display();
  }
};

//=========================================================
WwindowVelocity::WwindowVelocity() {
  itemVelocity = new ItemVelocity (this, 0, 0);
  itemOK = new ItemOK (this, 13);
  itemKO = new ItemKO (this, 15);
  itemVelocity->right = itemOK;
  itemOK->right = itemKO;
  itemKO->right = itemVelocity;
  itemVelocity->left = itemKO;
  itemOK->left = itemVelocity;
  itemKO->left = itemOK;
  labelActive = 0;
}

void WwindowVelocity::display() {
  myLcd->setCursor (0,1);
  myLcd->print (F("                "));
  myLcd->setCursor (0,1);
  itemVelocity->setValeur ( myMemory->maxVelocity.get() );
  itemVelocity->display();
  itemOK->display();
  itemKO->display();
  labelActive = itemVelocity;
  myLcd->setCursor (labelActive->pos,1);
  myLcd->cursor();
}

int WwindowVelocity::validate() {
  myMemory->maxVelocity.set (itemVelocity->getValeur());
  return 1;
}



//=========================================================

MainWindow::MainWindow() {
  itemNote = new ItemNote (this, 0, 0);
  itemMidiCh = new ItemMidiCh (this, 5, 0);
  // noteStatus = new ItemText (this, 8, "   ");
  boutonConf = new ItemBouton (this, 15, 1);
  menuConf = new MenuItemBase (this);
  itemVuMeter = new ItemVuMeter (this, 0, 16, 0);
  itemTypeVelocity = new ItemTypeVelocity (this, 8, 0);
  
  itemNote->right = itemMidiCh;
  itemMidiCh->right = itemTypeVelocity;
  itemTypeVelocity->right = boutonConf;
  boutonConf->right = itemNote;
  boutonConf->left = itemTypeVelocity;
  itemTypeVelocity->left = itemMidiCh;
  itemMidiCh->left = itemNote;
  itemNote->left = boutonConf;
  labelActive = 0;
}

void MainWindow::display() {
  myLcd->setCursor (0,1);
  myLcd->print (F("                "));
  myLcd->setCursor (0,1);
  itemMidiCh->setValeur ( myMemory->midiCh.get() );
  itemMidiCh->display();
  itemNote->setValeur ( myMemory->midiNote.get() );
  itemNote->display();
  itemTypeVelocity->setValeur ( myMemory->curbVelocity.get() );
  itemTypeVelocity->display();
  //noteStatus->display();
  boutonConf->display();
  itemVuMeter->display();
  labelActive = itemNote;
  
  myLcd->setCursor (labelActive->pos,1);
  myLcd->cursor();
}



//=============================================================
// construction of differenrt menus
MenuItemBase::MenuItemBase (MainWindow *_mainWindow) :MenuToWwindow ("1. Max Velocity", new WwindowVelocity ()){
  MenuToWwindow *l1c2;
  MenuExit *l1c9;
  // we keep track of the menu which created this window, in order to use this to
  // erase the window which launched the menu when it closes
  wwindowMenu->menuActive = this;
  mainWindow = _mainWindow;
  l1c2 = new MenuToWwindow ("2. Factory Reset", new WwindowResetUsine ());
  l1c9 = new MenuExit ("3. Exit");
  this->right = l1c2;
  l1c2->right = l1c9;
  l1c9->right = this;
  this->left = l1c9;
  l1c9->left = l1c2;
  l1c2->left = this;
  l1c2->up = this;
  l1c9->up = this;
};

void MenuItemBase::display (){
  MenuItem::display ();
}

//=============================================================

void MenuExit::erase (){
  myLcd->clear();
  myLcd->noCursor();
  BaseVisu::activeItem = ((MenuItemBase*)(up))->mainWindow;
  BaseVisu::activeItem->display();
}



//=============================================================
WwindowReset::WwindowReset() {
  itemText1 = new ItemText (this, 0, "RESET?");
  itemOK = new ItemOK (this, 13);
  itemKO = new ItemKO (this, 15);
  itemOK->right = itemKO;
  itemKO->right = itemOK;
  itemOK->left = itemKO;
  itemKO->left = itemOK;
  itemKO->wwindow = this;
  itemOK->wwindow = this;
  labelActive = 0;
}

void WwindowReset::display() {

  myLcd->setCursor (0,1);
  myLcd->print (F("                "));
  myLcd->setCursor (0,1);
  itemText1->display();
  itemOK->display();
  itemKO->display();
  labelActive = itemOK;
  myLcd->setCursor (labelActive->pos,1);
  myLcd->cursor();
}

int WwindowReset::validate() {
  myLcd->clear();
  myLcd->setCursor (0,0);
  myLcd->noCursor ();
  myLcd->print("RESET!");
  myMemory->reset();
  return 1;
}

//=============================================================
WwindowResetUsine::WwindowResetUsine() {
  itemText1 = new ItemText (this, 0, "Factory Rst?");
  itemOK = new ItemOK (this, 13);
  itemKO = new ItemKO (this, 15);
  itemOK->right = itemKO;
  itemKO->right = itemOK;
  itemOK->left = itemKO;
  itemKO->left = itemOK;
  itemKO->wwindow = this;
  itemOK->wwindow = this;
  labelActive = 0;
}

int WwindowResetUsine::validate() {
  myLcd->clear();
  myLcd->setCursor (0,0);
  myLcd->noCursor ();
  myLcd->print("RESET USINE!");
  myMemory->resetFactory();
  return 1;
}

//=========================================================
void ItemVuMeter::display(){
  myLcd->noCursor();
  myLcd->setCursor(pos,0);
  myLcd->print('V');
  if ( val >0 && maxPiezo > 0)
    valeur = (int)(log10 (val) / log10 (maxPiezo) * myMemory->maxVelocity.get() / 127 * span-2);
  else
    valeur =0;
  for (int i=1;i<span-1;i++){
      myLcd->setCursor(pos+i,0);
      myLcd->write( i <= valeur ? (uint8_t)5 : (uint8_t)7);
    }
  // write peak
  // peak goes up with val, goes down slowly
  if ( valeur >= peak)
    peak = valeur+1;
  else
    // in descent: move peak every delayDescent
    if (delayDescent >0)
      delayDescent--;
    else
      {
	delayDescent = 2 ;
	if ( peak > 1) {
	  // erase last peak on screen
	  myLcd->setCursor(peak,0);
	  myLcd->write( (uint8_t)7);
	  peak--;
	}
      }
  myLcd->setCursor(peak,0);
  myLcd->write( (uint8_t)6);
  
  if (wwindow->labelActive) {
    myLcd->setCursor(wwindow->labelActive->pos, 1);
    myLcd->cursor();
  }
}

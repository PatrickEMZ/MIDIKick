# MIDIKick
Arduino based device to convert a Roland kick drum KT-10 signal into MIDI notes 

The motivation for this project comes from a personal need: I play drums in a band and during COVID we had a fex opportunities to rehearse at one place. However, I cannot take all my drum gear with me.
Especially the my bass drum. Having a Roland kick trigger pedal KT-10 and aan Elektron Digitakt I planned to use both. But Digitakt only has a MIDI input. Hence the need for an interafce.
And I find it fun to do during this lockdown / curfew time!

The simple architecture is represented in the following image
![Architecture MidiKick](/images/Architecture.png)

Here is the simple schematics associated with this project:
![Schematics MidiKick](/design/MidiKickFritzin.png)

The LowPass filter and level limiter are very simple, and values have been chose by measuring the effects. A digital LPF would have been better, but I have limited time to do this and have zero experience on Arduino. I suspect real time issues, so you are welcome to try this!

The core algortithm is quite straightforward:
- acquisition of value
- if value is gretaer thean threshold, wait for maximum (we estimate when the acquired value starts to go down) and send MIDI NOTE ON
- when value goes below threshold, send MIDI NOTE OFF
- wait for short delay to absorb remaining oscillations
![Algorithm principles](/images/AcquisitionPrinciples.png)

Additional features are:
- change of NOTE: you can choose which note to play C0 to G10
- choice of MIDI channel :1 to 16
- choice of velocity curve: log, exponential, linear
- max velocity 1 to 128

The UI is based on some GUI tools I had designed before. Probably not optimal, but it works!

What you will need to build this:
* Arduino Uno (could probably work with a smaller Arduino)
* 1 female Jack 1/4 (6.3 mm)
* 1 female DIN 5 (MIDI)
* Resistors
  * 1 x 1 Mohm
  * 1 x 33 Kohm
  * 1 x 10 Kohm
  * 1 x 1Kohm
* Capacitor
  * 1 x 330 nF
* Diodes
  * 1 x Zener 4.7 V
* 1 LCD Display 16 x 2
* 3 x push buttons , 1 contact
* wires to connect all components
* 1 x USB power supply for Arduino
* 1 x MIDI cable
* 1 x male male 1/4 cable for kick drum connection

This design can certainly be enhanced, especially if you would like to adapt it to other piezos. There are mutiple examples you can get inspiration from, you are welcome to do this.

Licence: the code, all graphics and documents are under "Attribution 4.0 International (CC BY 4.0)" licence [licence](https://creativecommons.org/licenses/by/4.0/legalcode)

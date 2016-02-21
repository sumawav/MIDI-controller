/* SOUMA MONDAL
    Analog inputs and digital buttons
    For Teensy2
*/
/* USB MIDI AnalogControlChange Example
   You must select MIDI from the "Tools > USB Type" menu
   http://www.pjrc.com/teensy/td_midi.html
   This example code is in the public domain.
*/

#include <Bounce.h>

// MIDI constants
const int CHANNEL =               1; // the MIDI channel to send messages
const int TOP_BTN_MIDI_CC =       161;
const int MOMENTARY_1_MIDI_CC =   168;
const int MOMENTARY_2_MIDI_CC =   169;
const int TOGGLE_1_MIDI_CC =      158;
const int TOGGLE_2_MIDI_CC =      159;
const int KNOB_1_MIDI_CC =        149;
const int KNOB_2_MIDI_CC =        150;
const int MIDI_NOTE_ON =          127;
const int MIDI_NOTE_OFF =         0;


// Button constants
const int LED_PIN =           11; // Onboard LED for Teensy 2.0
const int DEBOUNCE_INTERVAL = 5;  // debounce interval
const int MOMENTARY_DELAY =   5;  // delay for momentary button

// Initialize debounce via Debounce library
Bounce top_button =   Bounce(6, DEBOUNCE_INTERVAL);
Bounce momentary_1 =  Bounce(7, DEBOUNCE_INTERVAL);
Bounce momentary_2 =  Bounce(8, DEBOUNCE_INTERVAL);
Bounce toggle_1 =     Bounce(9, DEBOUNCE_INTERVAL);
Bounce toggle_2 =     Bounce(10, DEBOUNCE_INTERVAL);

void setup()
{
  pinMode(10, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

// Initialize potentiometers
int knob_1 = analogRead(A9);
int knob_2 = analogRead(A8);
int old_knob_1 = -1;
int old_knob_2 = -1;

//Initialize momentary button state
bool m_state_1 = false;
bool m_state_2 = false;

//Initialize momentary button delay
int m_delay_1 = 0;
int m_delay_2 = 0;

elapsedMillis msec = 0;

void loop()
{

  if (msec >= 20) {
    msec = 0;
    top_button.update();
    momentary_1.update();
    momentary_2.update();
    toggle_1.update();
    toggle_2.update();
    knob_1 = analogRead(A9) / 8;
    knob_2 = analogRead(A8) / 8;

    // Knob 1
    if ( knob_1 != old_knob_1 ) {
      knob_1 = analogRead(A9) / 8;
      old_knob_1 = knob_1;
      usbMIDI.sendControlChange(KNOB_1_MIDI_CC, knob_1, CHANNEL);
    }

    // Knob 2
    if ( knob_2 != old_knob_2 ) {
      knob_2 = analogRead(A8) / 8;
      old_knob_2 = knob_2;
      usbMIDI.sendControlChange(KNOB_2_MIDI_CC, knob_2, CHANNEL);
    }

    // Top Button
    if (top_button.fallingEdge()) {
      usbMIDI.sendNoteOn(TOP_BTN_MIDI_CC, MIDI_NOTE_ON, CHANNEL);
    } else if (top_button.risingEdge()) {
      usbMIDI.sendNoteOff(TOP_BTN_MIDI_CC, MIDI_NOTE_OFF, CHANNEL);
    }

    // Momentary Button 1
    if (!m_state_1) {
      if (momentary_1.fallingEdge() || momentary_1.risingEdge()) {
        m_state_1 = true;
        m_delay_1 = MOMENTARY_DELAY;
        usbMIDI.sendNoteOn(MOMENTARY_1_MIDI_CC, MIDI_NOTE_ON, CHANNEL);
      }
    } else {
      if (m_delay_1 < 1) {
        m_state_1 = false;
        usbMIDI.sendNoteOff(MOMENTARY_1_MIDI_CC, MIDI_NOTE_OFF, CHANNEL);
      } else {
        m_delay_1--;
      }
    }

    // Momentary Button 2
    if (!m_state_2) {
      if (momentary_2.fallingEdge() || momentary_2.risingEdge()) {
        m_state_2 = true;
        m_delay_2 = MOMENTARY_DELAY;
        usbMIDI.sendNoteOn(MOMENTARY_2_MIDI_CC, MIDI_NOTE_ON, CHANNEL);
      }
    } else {
      if (m_delay_2 < 1) {
        m_state_2 = false;
        usbMIDI.sendNoteOff(MOMENTARY_2_MIDI_CC, MIDI_NOTE_OFF, CHANNEL);
      } else {
        m_delay_2--;
      }
    }

    // Toggle Button 1
    if (toggle_1.fallingEdge()) {
      usbMIDI.sendNoteOn(TOGGLE_1_MIDI_CC, MIDI_NOTE_ON, CHANNEL);
    } else if (toggle_1.risingEdge()) {
      usbMIDI.sendNoteOff(TOGGLE_1_MIDI_CC, MIDI_NOTE_OFF, CHANNEL);
    }

    // Toggle Button 2
    if (toggle_2.fallingEdge()) {
      usbMIDI.sendNoteOn(TOGGLE_2_MIDI_CC, MIDI_NOTE_ON, CHANNEL);
    } else if (toggle_2.risingEdge()) {
      usbMIDI.sendNoteOff(TOGGLE_2_MIDI_CC, MIDI_NOTE_OFF, CHANNEL);
    }

  } // if (msec >= 20)

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
  // ignore incoming messages
  }
}


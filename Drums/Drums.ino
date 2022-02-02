#include <MIDI.h>
#include <FastLED.h>

#define DATA_PIN    11
#define NUM_LEDS    10
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
CRGB leds[NUM_LEDS];

#define HIHAT 0
#define SNARE 1
#define TOM1 2
#define TOM2 3
#define TOM3 4
#define CRASH1 5
#define CRASH2 6
#define RIDE 7
#define KICK 8

MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte note, byte velocity) {
  int drum = checkNote(note);
  if(drum > -1){
    FastLED.show();
  } else {
    colorLEDs(96);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  int drum = checkNote(note);
  if(drum > -1){
    leds[drum] = CHSV(0, 0, 0);
    FastLED.show();
  }
  //resetLEDS();
}

int checkNote(byte note){
  if(note == 8 || note == 22){ leds[HIHAT] = CHSV(28, 255, 255); return HIHAT; } // red
  else if(note == 38 || note == 40){ leds[SNARE] = CHSV(0, 255, 255); return SNARE; } // red
  else if(note == 50 || note == 82){ leds[TOM1] = CHSV(56, 255, 255); return TOM1; } // yellow
  else if(note == 47 || note == 80){ leds[TOM2] = CHSV(112, 255, 255); return TOM2; } // blue
  else if(note == 43 || note == 75){ leds[TOM3] = CHSV(168, 255, 255); return TOM3; } // green
  else if(note == 49){ leds[CRASH1] = CHSV(84, 255, 255); return CRASH1; } // yellow
  else if(note == 57){ leds[CRASH2] = CHSV(140, 255, 255); return CRASH2; } // blue
  else if(note == 51 || note == 59){ leds[RIDE] = CHSV(196, 255, 255); return RIDE; } // green
  else if(note == 36){ leds[KICK] = CHSV(224, 255, 255); return KICK; } // orange
  else { return -1; }
}

void resetLEDS() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(0, 0, 0);
  }
  FastLED.show();
}

void colorLEDs(int clr) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(clr, 255, 255);
  }
  FastLED.show();
}


void setup() {
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  resetLEDS();

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  MIDI.read();
}

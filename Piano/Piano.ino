#include <usbmidi.h>
#include <FastLED.h>

#define DATA_PIN    11
#define NUM_LEDS    10
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
CRGB leds[NUM_LEDS];


#define MIDI_NOTE_OFF   0b10000000
#define MIDI_NOTE_ON    0b10010000
#define MIDI_CONTROL    0b10110000
#define MIDI_PITCH_BEND 0b11100000
#define NUM_NOTES       88
byte notes[NUM_NOTES];


void resetLEDS() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(0, 0, 0);
  }
  FastLED.show();
}

void setupLEDs() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // FastLED
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  resetLEDS();
}

void setupMIDI() {
  Serial.begin(115200); // MIDI baudrate
}

void noteOn(double key) {
  digitalWrite(LED_BUILTIN, HIGH);
  
  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
}

void noteOff(double key) {
  digitalWrite(LED_BUILTIN, LOW);
  
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
}

void handleMIDI() {
  //Handle USB communication
  USBMIDI.poll();

  // While there's MIDI USB input available...
  while (USBMIDI.available()) {
    noteOn(0);
    //Parse MIDI
    u8 command=0, channel=0, key=0, velocity=0, pblo=0, pbhi=0;

    //Skip to beginning of next message (silently dropping stray data bytes)
    while(!(USBMIDI.peek() & 0b10000000)) USBMIDI.read();

    command = USBMIDI.read();
    channel = (command & 0b00001111)+1;
    command = command & 0b11110000;

    switch(command) {
      case MIDI_NOTE_ON:
      case MIDI_NOTE_OFF:
        if(USBMIDI.peek() & 0b10000000) continue; key      = USBMIDI.read();
        if(USBMIDI.peek() & 0b10000000) continue; velocity = USBMIDI.read();
        break;
      case MIDI_PITCH_BEND:
        if(USBMIDI.peek() & 0b10000000) continue; pblo = USBMIDI.read();
        if(USBMIDI.peek() & 0b10000000) continue; pbhi = USBMIDI.read();
        int pitchbend = (pblo << 7) | pbhi;
        break;
    }

    // Lights
    if(command == MIDI_NOTE_ON && velocity > 0) noteOn(key);
    if(command == MIDI_NOTE_OFF || velocity == 0) noteOff(key);
  }
  noteOff(0);
}

void setup() {
  setupLEDs();
  setupMIDI();
  noteOn(0);
  delay(500);
  noteOff(0);
}

void loop() {
  noteOn(0);
  delay(200);
  noteOff(0);
  delay(200);
  handleMIDI();
}

#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 6
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 107
#define BRIGHTNESS 255
#define DELAYVAL 200

int r_pot = A0;
int g_pot = A1;
int b_pot = A2;
int r_value = 0;
int g_value = 0;
int b_value = 0;
int previous_r_value = 0;
int previous_g_value = 0;
int previous_b_value = 0;
int state = 0;
int state_just_changed = 0;

// Set to the value of '1' to enable debugging info to serial
int debugging_enabled = 0;

CRGB leds[NUM_LEDS];

void setup() {

  delay(500);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(115200);
  previous_r_value = map(analogRead(r_pot), 0, 1023, 0, 255);
  previous_g_value = map(analogRead(g_pot), 0, 1023, 0, 255);
  previous_b_value = map(analogRead(b_pot), 0, 1023, 0, 255);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(previous_r_value, previous_g_value, previous_b_value);
    FastLED.show();
  }

  pinMode(2, INPUT_PULLUP);

  if (debugging_enabled == 0){
    Serial.println("Debugging is set to off");
  }
}

void loop() {

  state = digitalRead(2);
  if (digitalRead(2)) {
    pride();
    FastLED.show();

    if (debugging_enabled == 1){
      Serial.print("toggled on | \t");
      Serial.print("pot_r:");
      Serial.print(analogRead(r_pot));
      Serial.print(", pot_g:");
      Serial.print(analogRead(g_pot));
      Serial.print(", pot_b:");
      Serial.println(analogRead(b_pot));
    }

  } else {

    if (debugging_enabled == 1){
      Serial.print("toggled off | \t");
    }

    r_value = map(analogRead(r_pot), 0, 1023, 0, 255);
    g_value = map(analogRead(g_pot), 0, 1023, 0, 255);
    b_value = map(analogRead(b_pot), 0, 1023, 0, 255);

    if ((r_value != previous_r_value) || (g_value != previous_g_value) || (b_value != previous_b_value)) {

      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(r_value, g_value, b_value);
        previous_r_value = r_value;
        previous_g_value = g_value;
        previous_b_value = b_value;
      }
      FastLED.show();
    }
    
    if (debugging_enabled == 1){
      Serial.print("R:");
      Serial.print(r_value);
      Serial.print(", G:");
      Serial.print(g_value);
      Serial.print(", B:");
      Serial.print(b_value);
      Serial.print(" | \t");
      Serial.print("pot_r:");
      Serial.print(analogRead(r_pot));
      Serial.print(", pot_g:");
      Serial.print(analogRead(g_pot));
      Serial.print(", pot_b:");
      Serial.println(analogRead(b_pot));
    }
    delay(DELAYVAL);
  }
}

// Taken from Pride2015 example of FastLED library:
// https://github.com/FastLED/FastLED/blob/master/examples/Pride2015/Pride2015.ino
// by Mark Kriegsman
// Animated, ever-changing rainbows.
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint16_t hue16 = sHue16;  //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;
    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    CRGB newcolor = CHSV(hue8, sat8, bri8);
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;
    nblend(leds[pixelnumber], newcolor, 64);
  }
}

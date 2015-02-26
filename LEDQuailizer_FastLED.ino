#include <FastLED.h>
#include <AudioAnalyzer.h>
#include "utils.h"

//#define RENE //so lassen, dann sollte es klappen

//#include "rene.h"
//#include "thomas_old_strip.h"
#include "thomas_new_strip.h"

//Status Leds

#define STATUS_DATA_PIN 11
#define STATUS_CLOCK_PIN 10
#define STATUS_NUM_LEDS 10

#define INI_BRIGHTNESS  255
#define INI_FRAMES_PER_SECOND 50
#define INI_CUT_OFF_LEVEL_0 30
#define INI_CUT_OFF_LEVEL_1 50
#define INI_CUT_OFF_LEVEL_2 70
#define INI_CUT_OFF_LEVEL_3 90
#define INI_CUT_OFF_LEVEL_4 150
#define INI_CUT_OFF_LEVEL_5 150
#define INI_CUT_OFF_LEVEL_6 100

CRGB leds[NUM_LEDS];
CRGB statusLeds[STATUS_NUM_LEDS];
CRGBPalette16 gPal, nPal;

long t1_millis, t2_millis, t3_millis, t4_millis;
long _t1_millis;
long t1_millis_avg, t2_millis_avg, t3_millis_avg , t4_millis_avg;

#define PAR_NUMEROF_CONFIGS   p[0].pInt
#define PAR_DEBUG_LEVEL       p[1].pInt
//#define PAR_FRAMES_PER_SECOND p[2].pInt

int FreqVal[7];
int FreqValLevel[7];
double FreqVqlAvg[7];
int overload;
int frame = 0;

configuration *modeConfig;
configuration *mainConfig;

void setup() {
  //delay(3000); // sanity delay
  Serial.begin(57600);

#ifdef CLK_PIN
  FastLED.addLeds<CHIPSET, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
#else
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
#endif

  FastLED.addLeds<WS2801, STATUS_DATA_PIN, STATUS_CLOCK_PIN, BGR>(statusLeds, STATUS_NUM_LEDS);

  FastLED.setBrightness( INI_BRIGHTNESS );

  FreqVqlAvg[0] = INI_CUT_OFF_LEVEL_0;
  FreqVqlAvg[1] = INI_CUT_OFF_LEVEL_1;
  FreqVqlAvg[2] = INI_CUT_OFF_LEVEL_2;
  FreqVqlAvg[3] = INI_CUT_OFF_LEVEL_3;
  FreqVqlAvg[4] = INI_CUT_OFF_LEVEL_4;
  FreqVqlAvg[5] = INI_CUT_OFF_LEVEL_5;
  FreqVqlAvg[6] = INI_CUT_OFF_LEVEL_6;

  for (int i = 0; i < 7; i++) {

    FreqVqlAvg[i] = FreqVqlAvg[i];

  }

  gPal = HeatColors_p;
  nPal = PartyColors_p;
  setupButtons();
  setupPotis();

  Audio.Init();

  initConfig();
}

void loop()
{
  frame++;

  _t1_millis = t1_millis;
  t1_millis = millis();
  t1_millis_avg = (9 * t1_millis_avg + (t1_millis - _t1_millis)) / 10;

  readPotis();
  readButtons();
  Audio.ReadFreq(FreqVal);
  renderStatus();

  switch (modeConfig->mode)
  {
    case AUDIO_FIRE_1:
      AudioFireMode1(); break;
    case AUDIO_FIRE_2:
      AudioFireMode2(); break;
    case FIRE_LIGHT:
      Fire(); break;
  }

  t2_millis = millis();
  t2_millis_avg = (9 * t2_millis_avg + (t2_millis - t1_millis)) / 10;

  readSerial();

  t3_millis = millis();
  t3_millis_avg = (9 * t3_millis_avg + (t3_millis - t2_millis)) / 10;

  FastLED.show(); // display this frame

  t4_millis = millis();
  t4_millis_avg = (9 * t4_millis_avg + (t4_millis - t3_millis)) / 10;

  
  FastLED.delay(max((1000 / INI_FRAMES_PER_SECOND - t4_millis-t1_millis),0));
}







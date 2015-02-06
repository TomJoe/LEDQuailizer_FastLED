#include <FastLED.h>
#include <AudioAnalyzer.h>
#include "utils.h"

//#define RENE //so lassen, dann sollte es klappen

//#incluie "rene.h"
#include "thomas_old_strip.h"
//#include "thomas_neu_strip.h"

//Status Leds

#define STATUS_DATA_PIN 11
#define STATUS_CLOCK_PIN 10
#define STATUS_NUM_LEDS 10

#define INI_BRIGHTNESS  255
#define INI_FRAMES_PER_SECOND 50
#define INI_CUT_OFF_LEVEL_0 60
#define INI_CUT_OFF_LEVEL_1 70
#define INI_CUT_OFF_LEVEL_2 70
#define INI_CUT_OFF_LEVEL_3 90
#define INI_CUT_OFF_LEVEL_4 150
#define INI_CUT_OFF_LEVEL_5 150
#define INI_CUT_OFF_LEVEL_6 100
//mit dem Wert kannst rumspielen - 1024

CRGB leds[NUM_LEDS];
CRGB statusLeds[NUM_LEDS];
CRGBPalette16 gPal, nPal;

#define PAR_NUMEROF_CONFIGS   p[0].pInt
#define PAR_DEBUG_LEVEL       p[1].pInt
#define PAR_FRAMES_PER_SECOND p[2].pInt

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

#ifdef RENE

  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

#else

  FastLED.addLeds<CHIPSET, LED_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

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
  Audio.Init();
  
  initConfig();
}

void loop()
{
  frame++;
  
  
  Audio.ReadFreq(FreqVal);
  renderStatus();
  

  //AudioFire(1);
  AudioFireMode2(modeConfig);
  //Fire(0);
  
  if (overload > 0) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    overload--;
    if (overload == 1)
      Serial.println("overload!");
  }

  if ((frame % (5 * INI_FRAMES_PER_SECOND) == 0) && (mainConfig->PAR_DEBUG_LEVEL == 0) ) {

    Serial.println("Average: ");
    for (int i = 0; i < 7; i++) {

      Serial.print((int)FreqVqlAvg[i]);
      if (i < 6)  Serial.print(",");
      else Serial.println();

    }
    
    Serial.println();
  }

  FastLED.show(); // display this frame
  readSerial();
  FastLED.delay(1000 / INI_FRAMES_PER_SECOND);
}







#include <FastLED.h>
#include <AudioAnalyzer.h>
#include "utils.h"

//#define RENE //so lassen, dann sollte es klappen

#ifdef RENE

#define DATA_PIN      5 //Anpassen
#define COLOR_ORDER   GRB //Anpassen
#define CHIPSET       WS2811 //CHIPSET anpassen  
#define NUM_LEDS      50 //Anpassen

#else

#define LED_PIN     8 //Anpassen
#define CLK_PIN     7 //brauchst du nicht
#define COLOR_ORDER BGR //Anpassen
#define CHIPSET     WS2801 //CHIPSET anpassen
#define NUM_LEDS    80 //Anpassen

#endif

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
CRGBPalette16 gPal, nPal;

#define PAR_NUMEROF_CONFIGS   p[0].pInt
#define PAR_DEBUG_LEVEL       p[1].pInt
#define PAR_FRAMES_PER_SECOND p[2].pInt

#ifdef RENE

Analyzer Audio = Analyzer(6, 7, 0); //Strobe pin ->6  RST pin ->7 Analog Pin ->0 //Anpassen

#else

Analyzer Audio = Analyzer(6, 5, A0)   ; //Strobe pin ->4  RST pin ->3 Analog Pin ->5 //Anpassen

#endif

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
  // Add entropy to random number generator; we use a lot of it.
  frame++;
  random16_add_entropy( random());

  //AudioFire(1);
  AudioFireMode2(modeConfig);
  //Fire(0);
 //if (FreqVal[i] > 1000) overload = 5;

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







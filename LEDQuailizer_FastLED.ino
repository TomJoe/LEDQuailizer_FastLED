#include <FastLED.h>
#include <AudioAnalyzer.h>

//#define RENE //so lassen, dann sollte es klappen

#ifdef RENE

#define DATA_PIN      5 //Anpassen
#define COLOR_ORDER   GRB //Anpassen
#define CHIPSET       WS2811 //CHIPSET anpassen  
#define NUM_LEDS      50 //Anpassen

#else

#define LED_PIN     5 //Anpassen
#define CLK_PIN     6 //brauchst du nicht
#define SPI_SPEED   DATA_RATE_MHZ(1)
#define COLOR_ORDER BGR //Anpassen
#define CHIPSET     WS2801 //CHIPSET anpassen
#define NUM_LEDS    80 //Anpassen

#endif

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 50
#define DECAY_PER_FRAME 0.3 //mit dem Wert kannst rumspielen 0.0 - 1.0
#define INI_CUT_OFF_LEVEL_0 0//50
#define INI_CUT_OFF_LEVEL_1 0//50
#define INI_CUT_OFF_LEVEL_2 0//70
#define INI_CUT_OFF_LEVEL_3 0//90
#define INI_CUT_OFF_LEVEL_4 0//150
#define INI_CUT_OFF_LEVEL_5 0//150
#define INI_CUT_OFF_LEVEL_6 0//100
//mit dem Wert kannst rumspielen - 1024

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal, nPal;

#ifdef RENE

Analyzer Audio = Analyzer(6, 7, 0); //Strobe pin ->6  RST pin ->7 Analog Pin ->0 //Anpassen

#else

Analyzer Audio = Analyzer(4, 3, A5)   ; //Strobe pin ->4  RST pin ->3 Analog Pin ->5 //Anpassen

#endif

int FreqVal[7];
int FreqValLevel[7];
double FreqVqlAvg[7];
int FreqValMinCutOffLevel[7];

void setup() {
  //delay(3000); // sanity delay
  Serial.begin(57600);

#ifdef RENE

  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

#else

  FastLED.addLeds<CHIPSET, LED_PIN, CLK_PIN, COLOR_ORDER, SPI_SPEED>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

#endif

  FastLED.setBrightness( BRIGHTNESS );

  FreqValMinCutOffLevel[0] = INI_CUT_OFF_LEVEL_0;
  FreqValMinCutOffLevel[1] = INI_CUT_OFF_LEVEL_1;
  FreqValMinCutOffLevel[2] = INI_CUT_OFF_LEVEL_2;
  FreqValMinCutOffLevel[3] = INI_CUT_OFF_LEVEL_3;
  FreqValMinCutOffLevel[4] = INI_CUT_OFF_LEVEL_4;
  FreqValMinCutOffLevel[5] = INI_CUT_OFF_LEVEL_5;
  FreqValMinCutOffLevel[6] = INI_CUT_OFF_LEVEL_6;

  for (int i = 0; i < 7; i++) {

    FreqVqlAvg[i] = FreqValMinCutOffLevel[i];

  }
  gPal = HeatColors_p;
  nPal = PartyColors_p;
  Audio.Init();
}

void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());

  //AudioFire(1);
  AudioFireMode2(2);

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}






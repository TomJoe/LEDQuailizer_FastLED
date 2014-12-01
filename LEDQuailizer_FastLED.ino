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
#define DECAY_PER_FRAME 0.1 //mit dem Wert kannst rumspielen 0.0 - 1.0
#define MIN_CUT_OFF_LEVEL 50 //mit dem Wert kannst rumspielen - 1024
#define CUT_OFF_DECY_PER_FRAME 1 // momentan funktionslos

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal, nPal;

#ifdef RENE

Analyzer Audio = Analyzer(6, 7, 0); //Strobe pin ->6  RST pin ->7 Analog Pin ->0 //Anpassen

#else

Analyzer Audio = Analyzer(4, 3, A5)   ; //Strobe pin ->4  RST pin ->3 Analog Pin ->5 //Anpassen

#endif

int FreqVal[7];
int FreqValLevel[7];
int FreqValCutOff[7];

void setup() {
  //delay(3000); // sanity delay
  Serial.begin(57600);

#ifdef RENE

  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

#else

  FastLED.addLeds<CHIPSET, LED_PIN, CLK_PIN, COLOR_ORDER, SPI_SPEED>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

#endif

  FastLED.setBrightness( BRIGHTNESS );
  for (int i = 0; i < 7; i++) FreqValCutOff[i] = MIN_CUT_OFF_LEVEL;
  gPal = HeatColors_p;
  //gPal = PartyColors_p;
  Audio.Init();
}

void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());

  AudioFire(); //Diese Funktion ein / auskommentieren fur Audio

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void AudioFire() {
  static int frame = 0;
  frame++;

  Audio.ReadFreq(FreqVal);
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (1.5 * FreqValCutOff[i])) {
      FreqValLevel[i] = 128;
    }

    FreqValCutOff[i] = ((FRAMES_PER_SECOND - 1) * FreqValCutOff[i] + FreqVal[i]) / FRAMES_PER_SECOND;
    if (FreqValCutOff[i] < MIN_CUT_OFF_LEVEL) FreqValCutOff[i] = MIN_CUT_OFF_LEVEL;

    Serial.print(FreqValCutOff[i]);//Transimit the DC value of the seven bands
    if (i < 6)  Serial.print(",");
    else Serial.println();

    double middleIdx;
    double pulseLength;
    double palStart;
    double palStep = -1.0;
    double brightnessDecay = 0.5;


    switch (i) {
      case 0:
      case 1:
        middleIdx = (NUM_LEDS / 4) * (1 + i*2);
        pulseLength = (NUM_LEDS / 4);
        palStart = 255;
        palStep = -5.0;
        brightnessDecay = 0.8;
        break;

      case 2:
  
      case 3:
     
      case 4:
        
      case 5:
        
      case 6:
        middleIdx = (NUM_LEDS / 10.0) * ((2 * i) - 3);
        pulseLength = NUM_LEDS / 5;
        palStart = 128;
        palStep = -10.0;
        brightnessDecay = 0.4;
        break;
    }
    //double middleIdx = (NUM_LEDS / 14.0) * (1 + 2 * i);

    //double pulseLength = FreqValCache[i] / 50;
    //double pulseLength = NUM_LEDS / 14;

    //double palStart = 240.0; //good in FirePalette
    //double palStart = 32 * i;
    //double palStep = -abs((240.0/(pulseLength)));
    //double palStep = abs((50.0/(pulseLength)));
    //double palStep = 0;
    //double brightnessDecay = 0.5;

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, gPal, FreqValLevel[i] , brightnessDecay, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, gPal, FreqValLevel[i] , brightnessDecay, BLEND);

    FreqValLevel[i] =  FreqValLevel[i] * DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;
    //FreqValCutOff[i] -= CUT_OFF_DECY_PER_FRAME;
    //if(FreqValLevel[i] < MIN_CUT_OFF_LEVEL) FreqValLevel[i] = MIN_CUT_OFF_LEVEL;
  }

}



void fill_palette_float(CRGB* pLeds , int pLedStartIdx , int pLedEndIdx , double pPalStart, double pPalStep, const CRGBPalette16& pGPal, uint8_t pBrightness, double pBrightnessDecay, TBlendType blendType) {

  if (pLedStartIdx < pLedEndIdx) {
    for (; pLedStartIdx < pLedEndIdx; pLedStartIdx++) {
      if (pLedStartIdx >= 0 && pLedStartIdx < NUM_LEDS) pLeds[pLedStartIdx] =  pLeds[pLedStartIdx] + ColorFromPalette( pGPal, pPalStart, pBrightness, blendType);
      pPalStart += pPalStep;
      pBrightness *= pBrightnessDecay;
    }
  } else {
    for (; pLedStartIdx > pLedEndIdx; pLedStartIdx--) {
      if (pLedStartIdx >= 0 && pLedStartIdx < NUM_LEDS) pLeds[pLedStartIdx] = pLeds[pLedStartIdx] + ColorFromPalette( pGPal, pPalStart, pBrightness, blendType);
      pPalStart += pPalStep;
      pBrightness *= pBrightnessDecay;
    }
  }

}

#include <FastLED.h>
#include <AudioAnalyzer.h>

#define LED_PIN     5 //Anpassen
#define CLK_PIN     6 //brauchst du nicht
#define SPI_SPEED   DATA_RATE_MHZ(1)
#define COLOR_ORDER BGR //Anpassen
#define CHIPSET     WS2801 //CHIPSET anpassen
#define NUM_LEDS    50 //Anpassen

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 50
#define DECAY_PER_FRAME 5 //mit dem Wert kannst rumspielen 0.0 - 1.0
#define CUT_OFF_LEVEL 50 //mit dem Wert kannst rumspielen - 1024

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal, nPal;

Analyzer Audio = Analyzer(4,3,A5); //Strobe pin ->3  RST pin ->4 Analog Pin ->5 //Anpassen
int FreqVal[7];
int FreqValCache[7];

void setup() {
  //delay(3000); // sanity delay
  Serial.begin(57600);
  FastLED.addLeds<CHIPSET, LED_PIN, CLK_PIN, COLOR_ORDER, SPI_SPEED>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  gPal = HeatColors_p;
  nPal = PartyColors_p;
  Audio.Init();
}

void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());

  //FireLight(); //Diese Funktion ein / auskommentieren fur nette Animation
  AudioFire(); //Diese Funktion ein / auskommentieren fur Audio

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void AudioFire() {
  static int frame = 0;
  frame++;
  
  Audio.ReadFreq(FreqVal);
  fill_solid(leds, NUM_LEDS, 0);
  
  for(int i = 0; i < 7; i++){
    if((FreqVal[i] - CUT_OFF_LEVEL) > FreqValCache[i]) FreqValCache[i] = FreqVal[i] - CUT_OFF_LEVEL;
    Serial.print(max(FreqVal[i]-CUT_OFF_LEVEL,0));//Transimit the DC value of the seven bands
    if(i<6)  Serial.print(",");
    else Serial.println();
    
    double middleIdx = (NUM_LEDS/14.0)*(1+2*i);
    
    //double pulseLength = FreqValCache[i] / 50;
    double pulseLength = NUM_LEDS/14;
    int brightness = FreqValCache[i];
  
    double palStart = 240.0;
    double palStep = -abs((240.0/(pulseLength))); 

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, gPal, brightness, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, gPal, brightness, BLEND);
    
    FreqValCache[i] =  FreqValCache[i] - DECAY_PER_FRAME;
    if(FreqValCache[i] < 0) FreqValCache[i] = 0; 
  }

}

void FireLight() {
  static int frame = 0;
  frame++;
   fill_solid(leds, NUM_LEDS, 0);
  //if(frame > FRAMES_PER_SECOND * 20) frame = 0;

  double pulseLength = 10.0  + (10 * sin(frame / 500.0));
  double middleIdx = NUM_LEDS/2 + (NUM_LEDS/2 * sin(frame / 200.0));

  double palStart = 128.0;
  double palStep = -abs((128.0/(pulseLength)));

 
  fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, gPal, BRIGHTNESS, BLEND);
  fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, gPal, BRIGHTNESS, BLEND);
  
  pulseLength = 10.0  * + (10 * sin(frame / 400.0));
  middleIdx = NUM_LEDS/2 + (NUM_LEDS/2 * sin(frame / 25.0));

  palStart = 128.0;
  palStep = -abs((128.0/(pulseLength)));
  
  fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, nPal, BRIGHTNESS, BLEND);
  fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, nPal, BRIGHTNESS, BLEND);
  
  

}

void fill_palette_float(CRGB* pLeds , int pLedStartIdx , int pLedEndIdx , double pPalStart, double pPalStep, const CRGBPalette16& pGPal, uint8_t pBrightness, TBlendType blendType) {

  if (pLedStartIdx < pLedEndIdx) {
    for (; pLedStartIdx < pLedEndIdx; pLedStartIdx++) {
      if(pLedStartIdx >= 0 && pLedStartIdx < NUM_LEDS) pLeds[pLedStartIdx] =  pLeds[pLedStartIdx] + ColorFromPalette( pGPal, pPalStart, pBrightness, blendType);
      pPalStart += pPalStep;
    }
  } else {
    for (; pLedStartIdx > pLedEndIdx; pLedStartIdx--) {
      if(pLedStartIdx >= 0 && pLedStartIdx < NUM_LEDS) pLeds[pLedStartIdx] = pLeds[pLedStartIdx] + ColorFromPalette( pGPal, pPalStart, pBrightness, blendType);
      pPalStart += pPalStep;
    }
  }

};
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    leds[j] = HeatColor( heat[j]);
  }
}


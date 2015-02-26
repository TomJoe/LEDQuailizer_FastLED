// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define DEF_MIDTEMP 230
#define PAR_MIDTEMP p[0].pInt

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define DEF_NOISE 0.5
#define PAR_NOISE p[1].pDouble

#define DEF_BRIGHTNESS 100
#define PAR_BRIGHTNESS p[2].pInt

void initFireLight(configuration* thisConfig){
  
  thisConfig->PAR_MIDTEMP = DEF_MIDTEMP;
  thisConfig->PAR_NOISE = DEF_NOISE;
  thisConfig->PAR_BRIGHTNESS = DEF_BRIGHTNESS;  
}

void readAnalogInput() {
  
  modeConfig->PAR_MIDTEMP = readPoti(0, modeConfig->PAR_MIDTEMP, 0, 255);
  
  modeConfig->PAR_NOISE = readPoti(1, modeConfig->PAR_NOISE, 0, 1.0);
  
  modeConfig->PAR_BRIGHTNESS = readPoti(2, modeConfig->PAR_BRIGHTNESS, 0, 255);
  
}

void Fire()
{
  readAnalogInput();
  double x,f;
  for(int i = 0; i < NUM_LEDS; i++){
    x = i * 360 / NUM_LEDS;
    f = millis() /100;
    
    leds[i] = ColorFromPalette( gPal,
    (
    max(modeConfig->PAR_MIDTEMP -  modeConfig->PAR_NOISE * (sin8(x + (f)) + 0.5*sin8(x + 2 * f) + .25*sin8(x + 4 * f)),0)
    )
    , modeConfig->PAR_BRIGHTNESS, BLEND);
  }
  
}






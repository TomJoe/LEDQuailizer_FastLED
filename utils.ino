#include "EEPROM.h"

#define MAGIC_BYTE_START 0xabcd
#define MAGIC_BYTE_END 0xdcba

#define MAIN_CONFIG  0
#define AUDIO_FIRE_1 1
#define AUDIO_FIRE_2 2
#define FIRE_LIGHT   3


typedef struct parametersInt{

  int p1, p2, p3, p4, p5, p6;
 
};

typedef struct parametersDouble{

  double p1, p2, p3, p4, p5, p6;
};

typedef struct config{
  int magicByteStart;
  int lenght;
  int mode;
  union {
  parametersInt intP;
  parametersDouble doubleP;
  };
  int magicByteEnd;
};

config mainConfig;

//fill a strip "pLeds" from "pLedStartIdx" to "pLedEndIdx" uding a 256color-Palette "gPal"
// starting with color at palette index "pPalStart" and modulating color for each next Pixel with "pPalStep"
// using Brightness "pBrightness" and modulating brightness for each next pixel with "pBrightnessDecy"

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

void readConfig(){
  //read Main Config
  for(int i=0; i < sizeof(mainConfig); i++){
    byte a = EEPROM.read(i);
    *((byte*)&mainConfig+i) = a;
  }
  if(mainConfig.magicByteStart == MAGIC_BYTE_START && mainConfig.magicByteEnd == MAGIC_BYTE_END){
    Serial.println("Config loaded");
    Serial.print("number of Configs: ");Serial.println(mainConfig.intP.p1);
  }else{
    Serial.println("No valid config found. Creating new Config.");
    
  }
}

void readSerial(){  
  char str[80];
  int x = Serial.available();   
  if(x>0){
    delay(100);
    x = Serial.available();
    int y = 0;
    for(x = Serial.available(); x>0 ; x--) {
      // read the incoming byte:
      str[y++] = Serial.read();
      delay(1);
    }
    str[y] = '\0';

    Serial.print("Input: ");
    Serial.println(str);

    if(str[0] == char('p')){
     
      Serial.println(atoi(str+1));
    }

    Serial.println(">");
  }
}


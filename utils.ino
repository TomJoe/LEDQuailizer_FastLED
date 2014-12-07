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



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

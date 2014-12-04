
void AudioFire(int mode) {
  static int frame = 0;
  frame++;

  Audio.ReadFreq(FreqVal);
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (1.5 * FreqVqlAvg[i])) {
      FreqValLevel[i] = 128;
    }

    FreqVqlAvg[i] = ((9) * FreqVqlAvg[i] + FreqVal[i]) / 10;
    if (FreqVqlAvg[i] < FreqValMinCutOffLevel[i] ) FreqVqlAvg[i] = FreqValMinCutOffLevel[i] ;

    Serial.print(FreqVqlAvg[i]);//Transimit the DC value of the seven bands
    /*if (i < 6)  Serial.print(",");
    else Serial.println();*/

    double middleIdx;
    double pulseLength;
    double palStart;
    double palStep;
    double brightnessDecay;


    switch (i) {
      case 0:
      case 1:
        middleIdx = (NUM_LEDS / 4) * (1 + i * 2);
        pulseLength = (NUM_LEDS / 4);
        palStart = 255;
        palStep = -5.0;
        brightnessDecay = 0.7;
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

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, gPal, FreqValLevel[i] , brightnessDecay, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, gPal, FreqValLevel[i] , brightnessDecay, BLEND);

    FreqValLevel[i] =  FreqValLevel[i] * DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;

  }

}


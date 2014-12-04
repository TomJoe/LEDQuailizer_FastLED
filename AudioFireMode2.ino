void AudioFireMode2(int mode) {

  static int frame = 0;
  static int state[7];
  static int hold[7];
  frame++;

  Audio.ReadFreq(FreqVal);
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (1.2 * FreqVqlAvg[i])) {
      FreqValLevel[i] = 128;
      if (hold[i] == 0) {
        state[i]++;
        if (state[i] > 16) state[i] = 0;
      }
      hold[i] = 1;
    } else {
      hold[i] = 0;
    }

    FreqVqlAvg[i] = (249.0 * FreqVqlAvg[i] + FreqVal[i]) / 250.0;
    if (FreqVqlAvg[i] < FreqValMinCutOffLevel[i] ) FreqVqlAvg[i] = FreqValMinCutOffLevel[i] ;

    Serial.print((int)FreqVqlAvg[i]);//Transimit the DC value of the seven bands
  
    if (i < 6)  Serial.print(",");
    else Serial.println();

    double middleIdx;
    double pulseLength;
    double palStart;
    double palStep;
    double brightnessDecay;


    switch (i) {
      case 0:
      case 1:
      case 2:
        middleIdx = (NUM_LEDS / 6) * ((2*i) + 1);
        pulseLength = (double)FreqVal[i] / (double)FreqVqlAvg[i] * 5;
        palStart = 255 - (32 * i);
        palStep = 0;
        brightnessDecay = 0.8;
        break;      

      case 3:
      case 4:
      case 5:
      case 6:
        middleIdx = (NUM_LEDS / 16.0) * (state[i]);
        pulseLength = (double)FreqVal[i] / (double)FreqVqlAvg[i] * 2;
        palStart = (i * 32);
        palStep = +2.0;
        brightnessDecay = 0.8;
        break;
    }

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);

    FreqValLevel[i] =  FreqValLevel[i] * DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;
  }

}

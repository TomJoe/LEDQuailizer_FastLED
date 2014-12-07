#define FREQ_AVG_2_TRIGGER_RATION 1.2
#define FILTER_LENGTH 250.0

void AudioFireMode2(int mode) {

  
  static int state[7];
  static int hold[7];
  frame++;

  Audio.ReadFreq(FreqVal);
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (FREQ_AVG_2_TRIGGER_RATION * FreqVqlAvg[i])) {
      FreqValLevel[i] = 128;
      if (hold[i] == 0) {
        state[i]++;
        if (state[i] > 16) state[i] = 0;
      }
      hold[i] = 1;
    } else {
      hold[i] = 0;
    }

    FreqVqlAvg[i] = ((FILTER_LENGTH - 1.0) * FreqVqlAvg[i] + FreqVal[i]) / FILTER_LENGTH;
    //if (FreqVqlAvg[i] < FreqValMinCutOffLevel[i] ) FreqVqlAvg[i] = FreqValMinCutOffLevel[i] ;

    double middleIdx;
    double pulseLength;
    double palStart;
    double palStep;
    double brightnessDecay;


    switch (i) {
      case 0:
      case 1:
      case 2:
        middleIdx = (NUM_LEDS / 2) * (i);
        pulseLength = (NUM_LEDS/8) * (double)FreqVal[i] / (double)FreqVqlAvg[i];
        palStart = 255 - (64 * i);
        palStep = 0;
        brightnessDecay = 0.9;
        break;      

      case 3:
      case 4:
      case 5:
      case 6:
        middleIdx = (NUM_LEDS / 16.0) * (state[i]);
        pulseLength = (double)FreqVal[i] / (double)FreqVqlAvg[i] * 3;
        palStart = (i * 32);
        palStep = +2.0;
        brightnessDecay = 0.8;
        break;
    }

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);

    if(FreqVal[i] > 1000) overload = 5;

    FreqValLevel[i] =  FreqValLevel[i] * DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;
  }

}

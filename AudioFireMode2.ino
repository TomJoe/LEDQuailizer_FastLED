#define INI_FREQ_AVG_2_TRIGGER_RATION 1.2
#define INI_FILTER_LENGTH 250.0
#define INI_DECAY_PER_FRAME 0.3 
#define INI_STREAK_LENGTH 0.25

#define GF "

#define PAR_FREQ_AVG_2_TRIGGER_RATION p[0].pDouble
#define PAR_FILTER_LENGTH p[1].pDouble
#define PAR_DECAY_PER_FRAME p[2].pDouble
#define PAR_STREAK_LENGTH p[3].pDouble

void initAudioFireMode2(configuration *thisConfig) {
  thisConfig->mode = MODE_AUDIO_FIRE_MODE_2;
  thisConfig->PAR_FREQ_AVG_2_TRIGGER_RATION = INI_FREQ_AVG_2_TRIGGER_RATION;
  thisConfig->PAR_FILTER_LENGTH = INI_FILTER_LENGTH;
  thisConfig->PAR_DECAY_PER_FRAME = INI_DECAY_PER_FRAME;
  thisConfig->PAR_STREAK_LENGTH = INI_STREAK_LENGTH;
}

void printConfigAudioFireMode2(configuration *thisConfig) {
  char[100] = "\"PAR_FREQ_AVG_2_TRIGGER_RATION\"";
  Serial.print("PAR_FREQ_AVG_2_TRIGGER_RATION: ");Serial.print(GF PAR_FREQ_AVG_2_TRIGGER_RATION GF);Serial.println(thisConfig->PAR_FREQ_AVG_2_TRIGGER_RATION);
  Serial.print("PAR_FILTER_LENGTH: ");Serial.println(thisConfig->PAR_FILTER_LENGTH);
  Serial.print("PAR_DECAY_PER_FRAME: ");Serial.println(thisConfig->PAR_DECAY_PER_FRAME);
  Serial.print("PAR_STREAK_LENGTH: ");Serial.println(thisConfig->PAR_STREAK_LENGTH);
}

void AudioFireMode2(configuration *thisConfig) {

  static int state[7];
  static int hold[7];
  frame++;

  Audio.ReadFreq(FreqVal);
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (thisConfig->PAR_FREQ_AVG_2_TRIGGER_RATION * FreqVqlAvg[i])) {
      FreqValLevel[i] = 128;
      if (hold[i] == 0) {
        state[i]++;
        if (state[i] > 16) state[i] = 0;
      }
      hold[i] = 1;
    } else {
      hold[i] = 0;
    }

    FreqVqlAvg[i] = ((thisConfig->PAR_FILTER_LENGTH - 1.0) * FreqVqlAvg[i] + FreqVal[i]) / thisConfig->PAR_FILTER_LENGTH;
    //if (FreqVqlAvg[i] < FreqValMinCutOffLevel[i] ) FreqVqlAvg[i] = FreqValMinCutOffLevel[i] ;

    double middleIdx;
    double pulseLength;
    double palStart;
    double palStep;
    double brightnessDecay;


    switch (i) {
      case 0:
        break;
      case 1:
      case 2:
        middleIdx = (NUM_LEDS) * (i-1);
        pulseLength = (double) NUM_LEDS * thisConfig->PAR_STREAK_LENGTH * (double)FreqVal[i] / (double)FreqVqlAvg[i];
        palStart = 255 - (64 * i);
        palStep = 0;
        brightnessDecay = 0.9;
        break;

      case 3:
      case 4:
      case 5:
        middleIdx = (NUM_LEDS / 16.0) * (state[i]);
        pulseLength = (double)FreqVal[i] / (double)FreqVqlAvg[i] * 3;
        palStart = (i * 32);
        palStep = +2.0;
        brightnessDecay = 0.8;
        break;
      case 6:
        break;
    }

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);

    FreqValLevel[i] =  FreqValLevel[i] * thisConfig->PAR_DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;
  }

}


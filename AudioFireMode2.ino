#define INI_FREQ_AVG_2_TRIGGER_RATION 1.2
#define INI_FILTER_LENGTH 250.0
#define INI_DECAY_PER_FRAME 0.3
#define INI_STREAK_LENGTH 0.25

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

void readAnalogInput(int potiNumber) {
  int sensorPin;
  int value;
  double mappedParaValue;
  switch (potiNumber) {
    case 1:
      value = analogRead(A1);
      mappedParaValue = mapFloat(value, 10, 1000, 0.5, 2.0);
      if (modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION != mappedParaValue) {
        modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION  = mappedParaValue;
        Serial.print("1: value: "); Serial.print(value); Serial.print(" / mapped: "); Serial.println(mappedParaValue);
      }
      break;
    case 2:
      value = analogRead(A2);
      mappedParaValue = mapFloat(value, 10, 1000, 0.1, 1.0);
      if (modeConfig->PAR_DECAY_PER_FRAME != mappedParaValue) {
        modeConfig->PAR_DECAY_PER_FRAME  = mappedParaValue;
        Serial.print("2: value: "); Serial.print(value); Serial.print(" / mapped: "); Serial.println(mappedParaValue);
      }
      break;
    case 3:
      value = analogRead(A3);
      mappedParaValue = mapFloat(value, 10, 1000, 0.0, 1.0);
      if (modeConfig->PAR_STREAK_LENGTH != mappedParaValue) {
        modeConfig->PAR_STREAK_LENGTH  = mappedParaValue;
        Serial.print("3: value: "); Serial.print(value); Serial.print(" / mapped: "); Serial.println(mappedParaValue);
      }
      break;

  }

}

void printConfigAudioFireMode2(configuration *thisConfig) {

  Serial.print("PAR_FREQ_AVG_2_TRIGGER_RATION: "); Serial.println(thisConfig->PAR_FREQ_AVG_2_TRIGGER_RATION);
  Serial.print("PAR_FILTER_LENGTH: "); Serial.println(thisConfig->PAR_FILTER_LENGTH);
  Serial.print("PAR_DECAY_PER_FRAME: "); Serial.println(thisConfig->PAR_DECAY_PER_FRAME);
  Serial.print("PAR_STREAK_LENGTH: "); Serial.println(thisConfig->PAR_STREAK_LENGTH);
}

void AudioFireMode2(configuration *thisConfig) {

  static int state[7];
  static int hold[7];
  frame++;

  Audio.ReadFreq(FreqVal);
  readAnalogInput(1);
  readAnalogInput(2);
  readAnalogInput(3);
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (thisConfig->PAR_FREQ_AVG_2_TRIGGER_RATION * FreqVqlAvg[i])) {
      FreqValLevel[i] = 255;
      if (hold[i] == 0) {
        state[i] = random(NUM_LEDS);
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
      case 1:
      case 2:
        middleIdx = (NUM_LEDS / 2) * (i);
        pulseLength = (double) NUM_LEDS * thisConfig->PAR_STREAK_LENGTH * (double)FreqVal[i] / (double)FreqVqlAvg[i];
        palStart = 32 * state[i] ;
        palStep = 0;
        brightnessDecay = 1.0;
        break;

      case 3:
      case 4:
      case 5:
      case 6:
        middleIdx = state[i];
        pulseLength = (double) NUM_LEDS / 20 * (double)FreqVal[i] / (double)FreqVqlAvg[i] ;
        palStart = (i * 32);
        palStep = 0;
        brightnessDecay = 1.0;
        break;
    }

    fill_palette_float(leds , middleIdx , middleIdx + pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);
    fill_palette_float(leds , middleIdx , middleIdx - pulseLength , palStart, palStep, nPal, FreqValLevel[i] , brightnessDecay, BLEND);

    FreqValLevel[i] =  FreqValLevel[i] * thisConfig->PAR_DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;
  }

}


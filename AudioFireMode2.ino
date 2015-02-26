#define INI_FREQ_AVG_2_TRIGGER_RATION 1.3
#define INI_FILTER_LENGTH 250.0
#define INI_DECAY_PER_FRAME 0.3
#define INI_STREAK_LENGTH 0.1

#define PAR_FREQ_AVG_2_TRIGGER_RATION p[0].pDouble
#define PAR_FILTER_LENGTH p[1].pDouble
#define PAR_DECAY_PER_FRAME p[2].pDouble
#define PAR_STREAK_LENGTH p[3].pDouble

void initAudioFireMode2(configuration *modeConfig) {
  modeConfig->mode = MODE_AUDIO_FIRE_MODE_2;
  modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION = INI_FREQ_AVG_2_TRIGGER_RATION;
  modeConfig->PAR_FILTER_LENGTH = INI_FILTER_LENGTH;
  modeConfig->PAR_DECAY_PER_FRAME = INI_DECAY_PER_FRAME;
  modeConfig->PAR_STREAK_LENGTH = INI_STREAK_LENGTH;
}

void readAnalogInput(int potiNumber) {
  int sensorPin;
  int value;
  double mappedParaValue;
  switch (potiNumber) {
    case 1:
      value = analogRead(A1);
      mappedParaValue = mapFloat(value, 10, 1000, 0.8, 1.5);
      if (modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION != mappedParaValue) {
        modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION  = mappedParaValue;
        #ifdef VERBOSE
        Serial.print("1: value: "); Serial.print(value); Serial.print(" / mapped: "); Serial.println(mappedParaValue);
        #endif
      }
      break;
    case 2:
      value = analogRead(A2);
      mappedParaValue = mapFloat(value, 10, 1000, 0.1, 1.0);
      if (modeConfig->PAR_DECAY_PER_FRAME != mappedParaValue) {
        modeConfig->PAR_DECAY_PER_FRAME  = mappedParaValue;
        #ifdef VERBOSE
        Serial.print("2: value: "); Serial.print(value); Serial.print(" / mapped: "); Serial.println(mappedParaValue);
        #endif
      }
      break;
    case 3:
      value = analogRead(A3);
      mappedParaValue = mapFloat(value, 10, 1000, 0.1, 1.0);
      if (modeConfig->PAR_STREAK_LENGTH != mappedParaValue) {
        modeConfig->PAR_STREAK_LENGTH  = mappedParaValue;
        #ifdef VERBOSE
        Serial.print("3: value: "); Serial.print(value); Serial.print(" / mapped: "); Serial.println(mappedParaValue);
        #endif
      }
      break;

  }

}

void printConfigAudioFireMode2(configuration *modeConfig) {
  
  #ifdef VERBOSE
  Serial.print("PAR_FREQ_AVG_2_TRIGGER_RATION: "); Serial.println(modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION);
  Serial.print("PAR_FILTER_LENGTH: "); Serial.println(modeConfig->PAR_FILTER_LENGTH);
  Serial.print("PAR_DECAY_PER_FRAME: "); Serial.println(modeConfig->PAR_DECAY_PER_FRAME);
  Serial.print("PAR_STREAK_LENGTH: "); Serial.println(modeConfig->PAR_STREAK_LENGTH);
  #endif
  
}

void AudioFireMode2() {

  static int state[7];
  static int hold[7];
  
  /*readAnalogInput(1);
  readAnalogInput(2);
  readAnalogInput(3);*/
  
  fill_solid(leds, NUM_LEDS, 0);

  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > (modeConfig->PAR_FREQ_AVG_2_TRIGGER_RATION * FreqVqlAvg[i])) {
      FreqValLevel[i] = 255;
      if (hold[i] == 0) {
        state[i] = random(NUM_LEDS);
      }
      hold[i] = 1;
    } else {
      hold[i] = 0;
    }

    FreqVqlAvg[i] = ((modeConfig->PAR_FILTER_LENGTH - 1.0) * FreqVqlAvg[i] + FreqVal[i]) / modeConfig->PAR_FILTER_LENGTH;
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
        pulseLength = (double) NUM_LEDS * modeConfig->PAR_STREAK_LENGTH * (double)FreqVal[i] / (double)FreqVqlAvg[i];
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

    FreqValLevel[i] =  FreqValLevel[i] * modeConfig->PAR_DECAY_PER_FRAME;
    if (FreqValLevel[i] < 0) FreqValLevel[i] = 0;
  }

}


#include "EEPROM.h"

#define PAR_DEBUG_LEVEL p[1].pInt
#define PAR_NUMEROF_CONFIGS p[0].pInt
#define PAR_CUTOFF_LEVEL p[2].pInt

#define btn1 2
#define btn2 3
#define btn3 4

boolean holdPoti[3];
boolean lockPoti[3];
int buttonDebounce[3];
int buttonState[3];
int analog[3];

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


void setupPotis() {
  for (int i = 0; i < 3; i++) {
    holdPoti[i] = false;
    lockPoti[i] = false;
  }
}

void readPotis() {
  analog[0] = (2 * analog[0] + analogRead(A1)) / 3;
  analog[1] = (2 * analog[1] + analogRead(A2)) / 3;
  analog[2] = (2 * analog[2] + analogRead(A3)) / 3;
}

void setupButtons() {
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
}


void readButtons() {

  int currButton;
  for (int i = 0; i < 3; i++) {
    switch (i) {
      case 0:
        currButton = digitalRead(btn1);
        break;
      case 1:
        currButton = digitalRead(btn2);
        break;
      case 2:
        currButton = digitalRead(btn3);
        break;
    }

    if (currButton == LOW ) buttonDebounce[i] = min(buttonDebounce[i]++, LONG_PRESS);
    else buttonDebounce[i] = 0;

    if (buttonDebounce[i] > SHORT_PRESS) buttonState[i] = BUTTON_PRESSED;
    if (buttonDebounce[i] >= LONG_PRESS) buttonState[i] = BUTTON_LONGPRESSED;
  }

  if (buttonState[0] == BUTTON_PRESSED) {
    lockPoti[0] = true; lockPoti[1] = true; lockPoti[2] = true;
  }

  if (buttonState[0] == BUTTON_LONGPRESSED) {
    lockPoti[0] = false; lockPoti[1] = false; lockPoti[2] = false;
    holdPoti[0] = true; holdPoti[1] = true; holdPoti[2] = true;
  }

}

double readPoti(int potiNbr, double currentValue, double mapLow, double mapHigh) {
  double returnValue = currentValue;
  double newValue = mapFloat(analog[potiNbr], 10, 1000, mapLow, mapHigh);

  if (potiNbr >= 3 || potiNbr < 0) {
    returnValue = 0;
  } else {
    if (!lockPoti[potiNbr]) {
      if (holdPoti[potiNbr] && (abs((currentValue - newValue) / (mapHigh - mapLow)) < 0.05)) holdPoti[potiNbr] = false;
#ifdef VERBOSE
      Serial.print("cV: "); Serial.print(currentValue); Serial.print(" /nV: "); Serial.println(newValue);
#endif
      if (!holdPoti[potiNbr]) returnValue = newValue;
    }
  }
  return returnValue;
}

int renderStatus() {
  if ((millis() % 1000 <= 100) && (mainConfig->PAR_DEBUG_LEVEL == 0) ) {

#ifdef VERBOSE
    Serial.println("Average: ");
#endif

    for (int i = 0; i < 7; i++) {

      Serial.print((int)FreqVqlAvg[i]);
      if (i < 6)  Serial.print(",");
      else Serial.println();

    }
    Serial.println();
    
    Serial.print(t1_millis_avg); Serial.print(",");Serial.print(t2_millis_avg); Serial.print(",");Serial.print(t3_millis_avg); Serial.print(",");Serial.print(t4_millis_avg); Serial.println();

  }
  
  
  CRGB statusColor = CRGB(0, 0, 0);
  for (int i = 0; i < 7; i++) {
    if (FreqVal[i] > mainConfig->PAR_CUTOFF_LEVEL) statusColor = CRGB(0, FreqVal[i] / 4, 0);
    if (FreqVal[i] > 800) statusColor = CRGB(100, 100, 0);
    if (FreqVal[i] > 1000) statusColor = CRGB(100, 0, 0);
    statusLeds[i] = statusColor;
  }

  for (int i = 0; i < 3; i++) {

    statusLeds[7 + i] = CRGB(0, 50, 0);
    if (holdPoti[i]) statusLeds[7 + i] = CRGB(50, 50, 0);
    if (lockPoti[i]) statusLeds[7 + i] = CRGB(50, 0, 0);
  }
}

int saveConfig() {
  int rtn = RETURN_FAILED;

  //save main config
  for (int i = 0; i < sizeof(configuration) ; i++) {
    char a = *((byte*)mainConfig + i);
    EEPROM.write(i, a);
  }

  //save current modeConfig
  for (int i = 0; i < sizeof(configuration) ; i++) {
    char a = *((byte*)modeConfig + i);
    EEPROM.write(i + mainConfig->mode * sizeof(configuration), a);
  }

#ifdef VERBOSE
  Serial.println("Config saved");
#endif

  rtn = 0;

  return rtn;
}

//
// read a config from EEPROM
//

int readConfig(int configNumber) {
  int rtn = RETURN_FAILED;

  if (modeConfig == NULL) modeConfig = (configuration*) malloc(sizeof(configuration));

  if (configNumber <= MAX_CONFIG_NBR) {

    for (int i = 0; i < sizeof(configuration); i++) {
      char a = EEPROM.read(i + configNumber * sizeof(configuration));
      *((byte*)modeConfig + i) = a;
    }

    rtn = configNumber;

#ifdef VERBOSE
    Serial.print("Nbr of config loaded:"); Serial.println(configNumber);
#endif

  }

  return rtn;
}

configuration* addConfig() {
  configuration *rtn = NULL;

  if (modeConfig == NULL) modeConfig = (configuration*) malloc(sizeof(configuration));

  if (mainConfig->PAR_NUMEROF_CONFIGS < MAX_CONFIG_NBR) {
    mainConfig->mode = ++mainConfig->PAR_NUMEROF_CONFIGS;

    modeConfig->magicByteStart = MAGIC_BYTE_START;
    modeConfig->magicByteEnd = MAGIC_BYTE_END;

    modeConfig->mode = MODE_FIRE_LIGHT;

    initModeConfig(modeConfig);

#ifdef VERBOSE
    Serial.print("Config added: "); Serial.println(mainConfig->mode);
#endif

    rtn = modeConfig;

  }

  return rtn;
}

//
//readConfig
//

void initConfig() {
  //read Main Config

#ifdef VERBOSE
  Serial.print("*mainConfig : "); Serial.println((int) mainConfig, HEX);
#endif

  if (mainConfig == NULL) mainConfig = (configuration*) malloc(sizeof(configuration));

#ifdef VERBOSE
  Serial.print("*mainConfig : "); Serial.println((int) mainConfig, HEX);
  Serial.print("sizeof(configuration) : "); Serial.println(sizeof(configuration));
  Serial.print("sizeof(*mainConfig) : "); Serial.println(sizeof(*mainConfig) );
#endif
  // read MainConfig

  for (int i = 0; i < sizeof(configuration); i++) {
    char a = EEPROM.read(i);
    *((byte*)mainConfig + i) = a;
  }

  if (mainConfig->magicByteStart == MAGIC_BYTE_START && mainConfig->magicByteEnd == MAGIC_BYTE_END) {

#ifdef VERBOSE
    Serial.println("Config loaded");
    Serial.print("number of Configs: "); Serial.println(mainConfig->PAR_NUMEROF_CONFIGS);
#endif

    if (mainConfig->PAR_NUMEROF_CONFIGS > 0) readConfig(mainConfig->PAR_NUMEROF_CONFIGS);

  } else {
#ifdef VERBOSE
    Serial.println("No valid config found. Creating new Config.");
    Serial.println("type 'save' to save config");
#endif

    mainConfig->magicByteStart = MAGIC_BYTE_START;
    mainConfig->magicByteEnd = MAGIC_BYTE_END;

    mainConfig->mode = 0; //configuration to use on launch
    mainConfig->PAR_NUMEROF_CONFIGS = 0; //Number of Configs
    mainConfig->PAR_CUTOFF_LEVEL = 100;

    addConfig();
  }

  printConfig();
}

//
//readSerial for Commands
//

void readSerial() {
  char str[40];
  int x = Serial.available();
  if (x > 0) {
    delay(100);
    x = Serial.available();
    int y = 0;
    for (x = Serial.available(); x > 0 ; x--) {
      // read the incoming byte:
      str[y++] = Serial.read();
      delay(1);
    }
    str[y] = '\0';

    Serial.print("Input: ");
    Serial.println(str);

    if (strncmp(str, "pi", 2) == 0) {
      printConfig();
      int paraNbr = atoi(str + 2); Serial.println(paraNbr);
      int paraValue = atoi(strrchr(str, ':') + 1); Serial.println(paraValue);
      modeConfig->p[paraNbr].pInt = paraValue;
      printConfig();
    }

    if (strncmp(str, "pd", 2) == 0) {
      printConfig();
      int paraNbr = atoi(str + 2); Serial.println(paraNbr);
      double paraValue = atof(strrchr(str, ':') + 1); Serial.println(paraValue);
      modeConfig->p[paraNbr].pDouble = paraValue;
      printConfig();
    }

    if (strcmp(str, "save") == 0) saveConfig();

    if (strcmp(str, "print") == 0) printConfig();

    if (strcmp(str, "add") == 0) addConfig();

    if (strcmp(str, "init") == 0) initModeConfig(modeConfig);

    if (strncmp(str, "debug", 5) == 0) {
      Serial.println(atoi(str + 5));
      mainConfig->PAR_DEBUG_LEVEL = atoi(str + 5);
    }

    Serial.println(">");
  }
}

//
//init cofiguration
//

void initModeConfig(configuration * thisConfig) {
  switch (thisConfig->mode) {
    case MODE_AUDIO_FIRE_MODE_1:
      initAudioFireMode1(thisConfig);
      break;
    case MODE_AUDIO_FIRE_MODE_2:
      initAudioFireMode2(thisConfig);
      break;
    case MODE_FIRE_LIGHT:
      initFireLight(thisConfig);
      break;
    default:
      thisConfig->mode = MODE_AUDIO_FIRE_MODE_2;
      initModeConfig(thisConfig);
      break;
  }
}

void printConfig() {

#ifdef VERBOSE
  Serial.println("MainConfig");
  Serial.print("configNbr:    "); Serial.println(mainConfig->mode, HEX);
  Serial.print("# of Configs: "); Serial.println(mainConfig->PAR_NUMEROF_CONFIGS);
  Serial.print("sizeOfConfig: "); Serial.println(sizeof(configuration) * (mainConfig->PAR_NUMEROF_CONFIGS + 1));
  Serial.print("Debug_Level: "); Serial.println(mainConfig->PAR_DEBUG_LEVEL);
  Serial.println("-");
  Serial.println("ModeConfig");
  Serial.print("ModeNumber : "); Serial.println(modeConfig->mode, HEX);
  for (int i = 0; i < 10; i++) {
    Serial.print("pi"); Serial.print(i); Serial.print(":"); Serial.print(modeConfig->p[i].pInt); Serial.print(" / "); Serial.print("pd"); Serial.print(i); Serial.print(":"); Serial.println(modeConfig->p[i].pDouble);
  }
#endif

  switch (modeConfig->mode) {
    case MODE_AUDIO_FIRE_MODE_1:
      //printConfigAudioFireMode1(modeConfig);
      break;
    case MODE_AUDIO_FIRE_MODE_2:
      printConfigAudioFireMode2(modeConfig);
      break;
    case MODE_FIRE_LIGHT:
      //printConfiginitFireLight(modeConfig);
      break;
    default:
      break;
  }
}

double mapFloat(double input, double startIn, double endIn, double startOut, double endOut) {
  double mappedValue;

  if (startIn < endIn) {
    if (input > endIn) input = endIn;
    if (input < startIn) input = startIn;
  }

  if (startIn > endIn) {
    if (input < endIn) input = endIn;
    if (input > startIn) input = startIn;
  }

  mappedValue = startOut + (input - startIn) * (endOut - startOut) / (endIn - startIn);

  return mappedValue;
}


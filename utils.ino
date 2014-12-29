#include "EEPROM.h"

#define PAR_DEBUG_LEVEL p[1].pInt
#define PAR_NUMEROF_CONFIGS p[0].pInt

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

  Serial.println("Config saved");

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
    Serial.print("Nbr of config loaded:"); Serial.println(configNumber);
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

    modeConfig->mode = MODE_AUDIO_FIRE_MODE_2;

    initModeConfig(modeConfig);

    Serial.print("Config added: "); Serial.println(mainConfig->mode);


    rtn = modeConfig;

  }

  return rtn;
}

//
//readConfig
//

void initConfig() {
  //read Main Config
  Serial.print("*mainConfig : "); Serial.println((int) mainConfig, HEX);
  if (mainConfig == NULL) mainConfig = (configuration*) malloc(sizeof(configuration));
  Serial.print("*mainConfig : "); Serial.println((int) mainConfig, HEX);
  Serial.print("sizeof(configuration) : "); Serial.println(sizeof(configuration));
  Serial.print("sizeof(*mainConfig) : "); Serial.println(sizeof(*mainConfig) );

  // read MainConfig

  for (int i = 0; i < sizeof(configuration); i++) {
    char a = EEPROM.read(i);
    *((byte*)mainConfig + i) = a;
  }

  if (mainConfig->magicByteStart == MAGIC_BYTE_START && mainConfig->magicByteEnd == MAGIC_BYTE_END) {
    Serial.println("Config loaded");
    Serial.print("number of Configs: "); Serial.println(mainConfig->PAR_NUMEROF_CONFIGS);

    if (mainConfig->PAR_NUMEROF_CONFIGS > 0) readConfig(mainConfig->PAR_NUMEROF_CONFIGS);

  } else {
    Serial.println("No valid config found. Creating new Config.");
    Serial.println("type 'save' to save config");

    mainConfig->magicByteStart = MAGIC_BYTE_START;
    mainConfig->magicByteEnd = MAGIC_BYTE_END;

    mainConfig->mode = 0; //configuration to use on launch
    mainConfig->PAR_NUMEROF_CONFIGS = 0; //Number of Configs

    addConfig();
  }

  printConfig();
}

//
//readSerial for Commands
//

void readSerial() {
  char str[80];
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
  
  mappedValue = startOut + (input - startIn) * (endOut - startOut)/(endIn - startIn);

  return mappedValue;
}

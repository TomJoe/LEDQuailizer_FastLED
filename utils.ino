#include "EEPROM.h"

#define PAR_DEBUG_LEVEL p02.pInt
#define PAR_NUMEROF_CONFIGS p01.pInt

configuration *modeConfig;

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

int saveConfig(int configNumber) {
  int rtn = RETURN_FAILED;

  if (configNumber == 0) {
    //save all configs
    for (int i = 0; i < ((mainConfig->PAR_NUMEROF_CONFIGS + 1) * sizeof(configuration)) ; i++) {
      char a = *((byte*)mainConfig + i);
      EEPROM.write(i, a);
    }
    Serial.println("Config saved");
    rtn = 0;
  } else {
    //save one config
    rtn = configNumber;
  };

  return rtn;
}

//
// read a config from EEPROM
//

int readConfig(int configNumber) {
  int rtn = RETURN_FAILED;

  if (configNumber <= MAX_CONFIG_NBR) {

    for (int i = mainConfig->PAR_NUMEROF_CONFIGS * sizeof(configuration); i < ((mainConfig->PAR_NUMEROF_CONFIGS + 1) * sizeof(configuration)); i++) {
      char a = EEPROM.read(i);
      *((byte*)&modeConfig + i) = a;
    }
    rtn = configNumber;
    Serial.print("Nbr of config loaded:"); Serial.println(configNumber);
  }

  return rtn;
}

int addConfig() {
  int rtn = RETURN_FAILED;

  if (mainConfig->PAR_NUMEROF_CONFIGS < MAX_CONFIG_NBR) {

    rtn = ++mainConfig->PAR_NUMEROF_CONFIGS;
    mainConfig->mode = mainConfig->PAR_NUMEROF_CONFIGS;

    modeConfig->magicByteStart = MAGIC_BYTE_START;
    modeConfig->magicByteEnd = MAGIC_BYTE_END;

    Serial.print("Config added: "); Serial.println(mainConfig->mode);
  }

  return rtn;
}

//
//readConfig
//

void initConfig() {
  //read Main Config
  for (int i = 0; i < sizeof(mainConfig); i++) {
    char a = EEPROM.read(i);
    *((byte*)&mainConfig + i) = a;
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

    Serial.println("MainConfig");
    Serial.print("configNbr:    "); Serial.println(mainConfig->mode);
    Serial.print("# of Configs: "); Serial.println(mainConfig->PAR_NUMEROF_CONFIGS);
    Serial.print("sizeOfConfig: "); Serial.println(sizeof(configuration));
  }
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

    if (str[0] == char('p')) {
      Serial.println(atoi(str + 1));
    }

    if (strcmp(str, "save") == 0) saveConfig(0);

    if (strcmp(str, "add") == 0) addConfig();

    if (strcmp(str, "init") == 0) initModeConfig();

    Serial.println(">");
  }
}

//
//init cofiguration
//

void initModeConfig() {
  switch(mainConfig->mode) {
  case MODE_AUDIO_FIRE_MODE_1:
    initAudioFireMode1(modeConfig);
    break;
  case MODE_AUDIO_FIRE_MODE_2:
    initAudioFireMode2(modeConfig);
    break;
  case MODE_FIRE_LIGHT:
    initFireLight(modeConfig);
    break;
  default:
    mainConfig->mode = MODE_AUDIO_FIRE_MODE_2;
    initModeConfig();
    break;
  }
}

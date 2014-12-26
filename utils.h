#define MAGIC_BYTE_START 0xafcf
#define MAGIC_BYTE_END 0xfafc

#define RETURN_FAILED -1

#define MAX_CONFIG_NBR 10

#define MODE_AUDIO_FIRE_MODE_1 1
#define MODE_AUDIO_FIRE_MODE_2 2
#define MODE_FIRE_LIGHT 3


#define MAIN_CONFIG  0
#define AUDIO_FIRE_1 1
#define AUDIO_FIRE_2 2
#define FIRE_LIGHT   3

union parameter {
  int pInt;
  double pDouble;
};

typedef struct configuration {
  int magicByteStart;
  int mode;
  parameter p[10];
  int magicByteEnd;
};


#define DATA_PIN     8 //Anpassen
#define CLK_PIN     7 //brauchst du nicht
#define VERBOSE

#define COLOR_ORDER BGR //Anpassen
//#define COLOR_ORDER RGB //Anpassen

#define CHIPSET     WS2801 //CHIPSET anpassen
#define NUM_LEDS    97 //Anpassen


Analyzer Audio = Analyzer(6, 5, A0)   ; //Strobe pin ->4  RST pin ->3 Analog Pin ->5 //Anpassen

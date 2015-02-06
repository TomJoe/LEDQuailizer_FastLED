#define UP 1
#define DOWN -1
#define LEFT -2
#define RIGHT 2
#define NUM_SEGMENTS 3
#define FIRE_LEDS_MAX 50

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define DEF_COOLING  50
#define PAR_COOLING thisConfiguration->p[0].pInt

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define DEF_SPARKING 80
#define PAR_SPARKING thisConfiguration->p[1].pInt

static byte heat[NUM_SEGMENTS][FIRE_LEDS_MAX];

configuration *thisConfiguration;

void initFireLight(configuration* thisConfig){
  
  thisConfiguration = thisConfig;
  PAR_COOLING = DEF_COOLING;
  PAR_SPARKING = DEF_SPARKING;
  
}

void Fire(int mode)
{

  FireVert(  0, 30, UP,   0);
  FireHorz( 45, 135, LEFT, 1);
  FireVert( 145, 159, DOWN, 2);

}

void FireVert(int startIdx, int endIdx, int orientation, int segmentNbr)
{
  // Array of temperature readings at each simulation cell
  int fire_leds = abs(endIdx - startIdx);

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < fire_leds; i++) {
    heat[segmentNbr][i] = qsub8( heat[segmentNbr][i],  random8(0, ((PAR_COOLING * 10) / fire_leds) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= fire_leds - 1; k >= 2; k--) {
    heat[segmentNbr][k] = (heat[segmentNbr][k - 1] + heat[segmentNbr][k - 2] + heat[segmentNbr][k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < PAR_SPARKING ) {
    int y = random8(7);
    heat[segmentNbr][y] = qadd8( heat[segmentNbr][y], random8(80,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < fire_leds; j++) {
    if((orientation == UP) && ((startIdx + j) <= endIdx)) leds[startIdx+j] = ColorFromPalette( gPal, heat[segmentNbr][j],heat[segmentNbr][j],BLEND);
    if((orientation == DOWN) && ((endIdx - j) >= startIdx)) leds[endIdx-j] = ColorFromPalette( gPal, heat[segmentNbr][j],heat[segmentNbr][j],BLEND);
  }
}

void FireHorz(int startIdx, int endIdx, int orientation, int segmentNbr)
{
  // Array of temperature readings at each simulation cell
  int fire_leds = abs(endIdx - startIdx);

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < fire_leds; i++) {
    heat[segmentNbr][i] = qsub8( heat[segmentNbr][i],  random8(0, ((PAR_COOLING * 10) / fire_leds) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= 2 ; k < (fire_leds-2); k++) {
    int old_left_heat = heat[segmentNbr][k-1];
    int old_very_left_heat = heat[segmentNbr][k-2];
    heat[segmentNbr][k] = (old_very_left_heat + old_left_heat + heat[segmentNbr][k] + heat[segmentNbr][k +1] + heat[segmentNbr][k + 2])/5;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  for(int y = 2; y < (fire_leds-2); y += 3){
    if( random8() < PAR_SPARKING / 10) {
      heat[segmentNbr][y] = qadd8( heat[segmentNbr][y], random8( 80, 120) );
      heat[segmentNbr][y-1] = heat[segmentNbr][y];
      heat[segmentNbr][y+1] = heat[segmentNbr][y];
    }
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < fire_leds; j++) {
    if((orientation == LEFT) && ((startIdx + j) <= endIdx))  {
      leds[startIdx+j] = ColorFromPalette( gPal, heat[segmentNbr][j],heat[segmentNbr][j],BLEND);
    }
    //if((orientation == RIGHT) && ((endIdx - j) >= startIdx)) leds[endIdx-j] = HeatColor( heat[segmentNbr][j]);
  }
}



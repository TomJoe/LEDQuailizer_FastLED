// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define DEF_MIDTEMP 200
#define PAR_MIDTEMP thisConfiguration->p[0].pInt

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define DEF_NOISE 1.0
#define PAR_NOISE thisConfiguration->p[1].pDouble

configuration *thisConfiguration;

void initFireLight(configuration* thisConfig){
  
  thisConfiguration = thisConfig;
  PAR_MIDTEMP = DEF_MIDTEMP;
  PAR_NOISE = DEF_MIDTEMP;
  
}

void Fire(int mode)
{

}






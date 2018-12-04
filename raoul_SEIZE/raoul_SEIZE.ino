

/* ----------------- Plein d'includes -------------------------*/
#include <HT1632.h>


#include <font_8x4.h>
#include <EEPROM.h>

#include <SD.h>
#include <SPI.h>
#include <arduino.h>
#include <MusicPlayer.h>

#include <lib_anims.h>


/* ----------------- Ecran LED -------------------------*/
#define pinCS1 3
#define pinWR 4
#define pinDATA 5


/* ----------------- Variables Diverses -------------------------*/

//Etat actuel du Raoul
typedef enum {AFFICHECOMPTEUR, AFFICHECAPS, AFFICHEPHRASE, AFFICHEANIM} stateRaoul;
stateRaoul myStateRaoul = AFFICHEANIM ;

int cpt = 0 ; // Compteur de capsules
#define CPT_ADDR 0 // Adresse de la variable cpt dans l'eeprom

byte current_anim = 0 ; // L'animation en train d'être jouée


bool bCapsule = false;
int i = 0 ;

const int PDCapsule = 2 ; //pin du bidule à capsule

// Pour l'affichage de texte qui défile
uint8_t wd;
uint8_t it;
char text[128] = "Roger vous couche tous !"; // Le texte à afficher sur l'écran
bool scrolling = false;
unsigned long lastScroll= 0 ;
#define TEXT_SCROLL_UPDATE_TIME 75 


//auto  timer = timer_create_default();


//Différentes valeurs de temps pour s'y retrouver dans tout ce merdier
unsigned long lastCapsSeen = 0 ;

/*const unsigned long DELAY_SCROLLING_TEXT = 40;
const unsigned long DELAY_CENTERED_TEXT = 2000;
const unsigned long DELAY_CAPSULE_TEXT = 200;
const unsigned long DELAY_RANDOM_TEXT = 780000;*/
const unsigned long DELAY_BETWEEN_CAPSULES = 500; //Allez tous niquer vos mères

/* ----------------- Fonctions Get Text -------------------------*/

void getCapsuleText(char* text)
{
  long rand = random(4);
  free(text);
  if(rand == (long)0) strcpy(text, "Voila un tir digne d'un légionnaire !");
  if(rand == (long)1) strcpy(text, "Capsulea jacta est !");
  if(rand == (long)2) strcpy(text, "Ah, quelle belle orgie !");
  if(rand == (long)3) strcpy(text, "Ca sent bon la decadence, tout ça !");
  if(rand == (long)4) strcpy(text, "Bacchus saura te recompenser!");
}
void getRandomText(char* text)
{
  long rand = random(5);
  free(text);
  if(rand == (long)0)   strcpy(text,"Du vin, par Junon !");
  if(rand == (long)1)   strcpy(text,"La promo 8*2 elle t'encule*2 !");
  if(rand == (long)2)   strcpy(text,"Cesar a la Gaule, et la 16 aussi !");
  if(rand == (long)3)   strcpy(text,"Je suis venu, j'ai bu, le reste j'm'en rappelle pu...");
  if(rand == (long)4)   strcpy(text,"Qui veut la chouille prépare l'apero");
  if(rand == (long)5)   strcpy(text,"Aut Cesar, aut Nahil");
}
    
/* ----------------- Fonctions Display Text -------------------------*/

void drawScrollingText()
{
  HT1632.clear();
  HT1632.drawText(text, OUT_SIZE - it, 0, FONT_8X4, FONT_8X4_END, FONT_8X4_HEIGHT);
  HT1632.render();
}

void drawCenteredText()
{
  HT1632.clear();
  HT1632.drawText(text, (OUT_SIZE - wd) / 2, 0, FONT_8X4, FONT_8X4_END, FONT_8X4_HEIGHT);
  HT1632.render();
}

/*
   Afficher un texte
   S'il dépasse de l'écran (en largeur), on le fait défiler
*/
void drawAdaptiveText()
{
  if (scrolling)
    drawScrollingText();
  else
    drawCenteredText();
}


/*
 *setScrolling
 *remet à 0 la valeur du curseur et vérifie si y'a besoin de scroll pour le texte 
 */
void setScrolling()
{
  it = 0;
  wd = HT1632.getTextWidth(text, FONT_8X4_END, FONT_8X4_HEIGHT);
  scrolling = (wd > OUT_SIZE);
}


/*
 *updateScroll
 *indente la position du curseur et affiche le texte avec la cadence désirée
 *retourne true s'il reste encore du texte à afficher, false sinon
 */
bool updateScroll()
{
  if (it <= wd + OUT_SIZE)
  {
    if(millis() - lastScroll > TEXT_SCROLL_UPDATE_TIME )
    {
      lastScroll = millis();
      it++;
      drawAdaptiveText();
    }
    return true ;
  }
  else return false ;
}



/* ----------------- Fonctions Accès mémoire  -------------------------*/

void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}
unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}



/* ----------------- Fonctions Raoul  -------------------------*/

void compteurInc()
{
  cpt++;
  EEPROMWriteInt(0, cpt);
}

void compteurInterrupt()
{
  Serial.println("Capsssss") ;
  bCapsule = true;
}

void checkCapsule() 
{  //Test du compte de capsule
  if(bCapsule )
  {
    //On récupère le passage de capsule via une interruption hardware, ici on se contente juste de changer le state si besoin
    if( (millis() - lastCapsSeen ) >= DELAY_BETWEEN_CAPSULES)
    {
      Serial.println("Okay, caps") ;
      //compteurInc();
      setState(AFFICHECAPS);
    }
    bCapsule = false;
  }
}


/* ------------------------ DEBUG
 *  
 *  
 */



/* ----------------- Fonctions ChangeState -------------------------*/


void setState(stateRaoul newState)
{
  myStateRaoul = newState;  
  switch(newState)
  {
    case AFFICHECOMPTEUR:
      break;
    case AFFICHECAPS: 
      //getCapsuleText(text);
    Serial.println("test");
    strcpy(text, "Test");
      setScrolling();
      drawAdaptiveText();
    Serial.println("test");
      break;
    case AFFICHEPHRASE:
      break;
    case AFFICHEANIM: 
    //random current_anim
      animations[current_anim]->startAnim();
      break;
    default :
      break;

  }
}




/* ----------------- Fonction Setup -------------------------*/
void setup () {  
  // Debug
  Serial.begin(9600);

  //Variables propres au Raoul
  cpt = EEPROMReadInt(CPT_ADDR);
 // text = (char*) malloc(512*sizeof(char));  


  // Ecran LED
  HT1632.begin(pinCS1, pinWR, pinDATA); //hasardeux, ça marche pas si je le mets avant, ni après, par contre avant ET après oui...
  
  delay(1000);
  // Lecteur audio
  player.begin(); //will initialize the hardware and set default mode to be normal.
  player.keyDisable();

  delay(1000);
  // Ecran LED
  HT1632.begin(pinCS1, pinWR, pinDATA);
  

  pinMode(PDCapsule, INPUT_PULLUP);

  // Attach interrupt pour la détection de la capsule
  //attachInterrupt(digitalPinToInterrupt(PDCapsule), compteurInterrupt, RISING);


  wd = HT1632.getTextWidth(text, FONT_8X4_END, FONT_8X4_HEIGHT);

  //animations[current_anim].startAnim();

  //Serial.println("Starting") ;
  //setState(AFFICHEANIM);
  
  setScrolling();
  //drawCenteredText();
}

/* ----------------- Fonction Loop -------------------------*/
void loop () {
  

  if(!updateScroll())
  {
    getRandomText(text);
    setScrolling();
  }
  //Update des trucs qui ont besoin de l'être
  player.play();
  //checkCapsule();


  // Logique affichage/animation
  switch (myStateRaoul)
  {
    case AFFICHECOMPTEUR :
      break ;
    case AFFICHECAPS :
      //drawAdaptiveText();
      break ;
    case AFFICHEPHRASE :
      break ;
    case AFFICHEANIM:
      //animations[current_anim].updateAnim();
      break ;
    default :
      break;
  }
  
  // Logique de comptage des capsules
/*
  unsigned long loopTimeLastCapsule = millis() - startTimeLastCapsule;
  if (bCapsule > 0 && !bCapsuleLoop && loopTimeLastCapsule >= DELAY_BETWEEN_CAPSULES)
  {
    compteurInc();
    bCapsule = 0;
    sprintf(text, "%d", cpt);
    //memcpy(text, cptC, sizeof(text));
    setScrolling();
    startTimeAfficheur = millis();
    startTimeRandom = millis();
    startTimeLastCapsule = millis();
    myTextState = AFFICHECOMPTEUR;
    myLedState = LEDCHENILLE;
    tLed = 0;
    ledLoop = 0;
  } 
   else
      bCapsuleLoop = false;

*/

}

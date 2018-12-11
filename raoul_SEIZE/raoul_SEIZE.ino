

#include <Arduino.h>
/* ----------------- Plein d'includes -------------------------*/
#include <HT1632.h>
#include <font_8x4.h>
#include <EEPROM.h>

#include <MusicPlayer.h>

#include <lib_anims.h>
#include "expressions.h"
#include "sounds.h"


/* ----------------- Pin Layout -------------------------*/

#define pinCS1 3
#define pinWR 4
#define pinDATA 5

#define CAPTEUR1 A4
#define CAPTEUR2 A7

/* ----------------- Variables Diverses -------------------------*/




//Etat actuel du Raoul
typedef enum {AFFICHECOMPTEUR, AFFICHECAPS, AFFICHEPHRASE, AFFICHEANIM, BLINKCOUNTER, JAMMED} stateRaoul;
stateRaoul myStateRaoul = AFFICHEPHRASE ;

#define SEUIL_CAPTEUR1 200
#define SEUIL_CAPTEUR2 200

unsigned int cpt = 0 ; // Compteur de capsules
#define CPT_ADDR 0 // Adresse de la variable cpt dans l'eeprom

uint8_t current_anim = 0 ; // L'animation en train d'être jouée


//bool bCapsule = false;

//const int PDCapsule = 2 ; //pin du bidule à capsule

// Pour l'affichage de texte qui défile
uint8_t wd;
uint8_t it;
char text[60] = "Roger vous couche tous" ; // Le texte à afficher sur l'écran
char soundPath[50]= "rom.mp3" ;
bool scrolling = false;
unsigned long lastScroll= 0 ;
unsigned long startCapsDisplay= 0 ;
#define TEXT_SCROLL_UPDATE_TIME 75 
#define TEXT_CENTERED_DISPLAY_TIME 5*1000 
#define CAPS_DISPLAY_TIME 16*1000 //16sec 

//pour le blink counter
bool blinked = true ;
uint8_t blinkNumber = 0 ;
unsigned long lastBlink= 0 ;
#define BLINK_TIME 500 // 
#define BLINK_OCCURENCES 10 // 


//Différentes valeurs de temps pour s'y retrouver dans tout ce merdier
unsigned long lastCapsSeen = 0 ;

unsigned long lastCapsSeenRow = 0 ;
uint8_t capsInARow = 0 ;
#define DELAY_CAPS_ROW 7*1000 //
#define CAPS_ROW 5 // 

unsigned long lastPlayedRandomSong = 0 ;

#define RANDOM_SONG_DELAY 16*60*1000 // toute les seize minutes
#define DELAY_BETWEEN_CAPSULES 500 //Allez tous niquer vos mères

/* ----------------- Fonctions Get Text -------------------------*/


void getCapsuleText(char* text)
{
  uint8_t rand = random(CAPS_TEXT_NUMBER);


  strcpy_P(text, (PGM_P)pgm_read_word(&( capsuleText[rand])));
}

void playVictorySound()
{
  uint8_t rand = random(VICTORY_SOUND_NUMBER);
  player.opStop();
  player.deleteSong(soundPath);

  strcpy_P(soundPath, (PGM_P)pgm_read_word(&( victorySounds[rand])));
  
  player.playOne(soundPath);
  player.play();
}

void playRandomSound()
{
  uint8_t rand = random(RANDOM_SOUND_NUMBER);
  player.opStop();
  player.deleteSong(soundPath);

  strcpy_P(soundPath, (PGM_P)pgm_read_word(&( randomSounds[rand])));

  player.playOne(soundPath);
  player.play();
}


void getRandomText(char* text)
{
  uint8_t rand = random(RANDOM_TEXT_NUMBER);
  strcpy_P(text, (PGM_P)pgm_read_word(&( randomText[rand])) );
}

void dec2romanstr( char * res,int num){
    if(num == 0 ) strcat(res, "nulla");
    int del[] = {1000,900,500,400,100,90,50,40,10,9,5,4,1}; // Key value in Roman counting
    char * sym[] = { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" }; //Symbols for key values
    //char res[64] = "\0";         //result string
    int i = 0;                   //
    while (num){                 //while input number is not zero
        while (num/del[i]){      //while a number contains the largest key value possible
            strcat(res, sym[i]); //append the symbol for this key value to res string
            num -= del[i];       //subtract the key value from number
        }
        i++;                     //proceed to the next key value
    }
}

/* ----------------- Fonctions Display Text -------------------------*/

bool updateBlinkText() // pour des centered text svp
{
  if(blinkNumber >= BLINK_OCCURENCES ) 
  {
    return false ;
  }


    if((millis() - lastBlink) > BLINK_TIME )
    {
      lastBlink = millis();
      blinkNumber++;
      blinked = !blinked ;
      if(blinked) 
      {
        HT1632.clear();
        HT1632.render();
      }
      else updateScroll() ;
    }
    
    return true ;
}


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
   setScrolling
   remet à 0 la valeur du curseur et vérifie si y'a besoin de scroll pour le texte 
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
  if(!scrolling) 
  {
     unsigned long elapsed = millis() - lastScroll ;
     if(elapsed > TEXT_CENTERED_DISPLAY_TIME )
     {
      return false ;
     }
     
    drawCenteredText();
    return true ;
  }
  if (it <= wd + OUT_SIZE)
  {
    unsigned long elapsed = millis() - lastScroll  ;
    
    if(elapsed > TEXT_SCROLL_UPDATE_TIME)
    {
      lastScroll = millis();
      it++;
      drawScrollingText();
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



/* ----------------- Fonctions Raoul  
-------------------------*/

void compteurInc()
{
  cpt++;
  EEPROMWriteInt(CPT_ADDR, cpt);  //à décommenter le jour J
}

void checkCapsule() 
{  //Test du compte de capsule
  
  bool bCapsule = (analogRead(CAPTEUR1) < SEUIL_CAPTEUR1 || analogRead(CAPTEUR2) < SEUIL_CAPTEUR2);
  
  if(bCapsule )
  {
    //On récupère le passage de capsule via une interruption hardware, ici on se contente juste de changer le state si besoin
    if( (millis() - lastCapsSeen ) > DELAY_BETWEEN_CAPSULES)
    {
      if(checkJam())
      {
        
        setState(JAMMED);
        return ;
      }
      compteurInc();
      setState(BLINKCOUNTER);
      lastCapsSeen = millis() ;
      
    }
    bCapsule = false;
  }
}

bool checkJam()
{
  if(millis()-lastCapsSeenRow < DELAY_CAPS_ROW)
  {
  Serial.println("CheckJam");
    if(capsInARow >= CAPS_ROW) return true ;
    capsInARow ++ ;
  }else
  {
    lastCapsSeenRow = millis();
    capsInARow = 0;
    
  }
    return false ;
}

/* ------------------------ DEBUG
 *  
 *  
 */

//int lastMem =0 ;

/* ----------------- Fonctions ChangeState -------------------------*/


void setState(stateRaoul newState)
{
  myStateRaoul = newState;  
  switch(myStateRaoul)
  {
    case JAMMED :
      strcpy(text,"debouchez l'acces capteur svp") ;
      setScrolling();
      drawAdaptiveText();
      break ;
    case BLINKCOUNTER :
      //////Serial.println("BLINKCOUNTER");
      blinkNumber = 0 ;
      playVictorySound() ;
      lastBlink = millis();
      sprintf(text,"%d",cpt);
      setScrolling();
      drawAdaptiveText();
      break ;
    case AFFICHECOMPTEUR:
      ////Serial.println("AFFICHECOMPTEUR");
      startCapsDisplay = millis();

      if(random(2)== 1)
      {
        sprintf(text,"Capsvles : ");
        dec2romanstr(text, cpt);
      }
      else sprintf(text,"%d",cpt);
      setScrolling();
      drawAdaptiveText();
      break;
    case AFFICHECAPS: 
      //Serial.println("AFFICHECAPS");
      getCapsuleText(text);
      setScrolling();
      drawAdaptiveText();
      break;
    case AFFICHEPHRASE:
      //Serial.println("AFFICHEPHRASE");
      getRandomText(text);
      setScrolling();
      drawAdaptiveText();
      break;
    case AFFICHEANIM: 
      //Serial.println("AFFICHEANIM");
    //random 
      current_anim = random(ANIMATIONS_SIZE);
      animTab[current_anim].startAnim() ;

      
      //free(text);
      //current_anim = getRandomAnim();
      //current_anim.startAnim();
      break;
    default :
      break;

  }
}


void updateRandomShit()
{
  if(millis()-lastPlayedRandomSong >=RANDOM_SONG_DELAY)
  {
    if(myStateRaoul != BLINKCOUNTER && myStateRaoul != AFFICHECAPS)
    {
      playRandomSound();
    }
    lastPlayedRandomSong = millis() ;
  }
}

/* ----------------- Fonction Setup -------------------------*/
void setup () { 
   
  // Debug
  Serial.begin(9600);

  //Variables propres au Raoul
  cpt = EEPROMReadInt(CPT_ADDR);


  // Ecran LED
  HT1632.begin(pinCS1, pinWR, pinDATA); //hasardeux, ça marche pas si je le mets avant, ni après, par contre avant ET après oui...

  delay(1000);
  
  // Lecteur audio
  player.begin(); //will initialize the hardware and set default mode to be normal.
  player.keyDisable();

  delay(1000);
  // Ecran LED
  HT1632.begin(pinCS1, pinWR, pinDATA);
  

  //pinMode(PDCapsule, INPUT_PULLUP);
  pinMode(CAPTEUR1, INPUT);
  pinMode(CAPTEUR2, INPUT);

  randomSeed(analogRead(CAPTEUR1));
  // Attach interrupt pour la détection de la capsule
  //attachInterrupt(digitalPinToInterrupt(PDCapsule), compteurInterrupt, FALLING);


  wd = HT1632.getTextWidth(text, FONT_8X4_END, FONT_8X4_HEIGHT);
  setScrolling();
  //setState(AFFICHEANIM);
  player.play();
}

/* ----------------- Fonction Loop -------------------------*/
void loop () {
  
  //debug
  /*
  if(lastMem != freeMemory() )
  {
    lastMem = freeMemory() ;
    Serial.println(lastMem);
  }
*/
/*
  Serial.print("capteur 1 A4 : ");
  Serial.print(analogRead(A4));
  Serial.print("  capteur 2 A6: ");
  Serial.print(analogRead(A6));
  Serial.print("  capteur 2 A7: ");
  Serial.println(analogRead(A7));*/
  //Update des trucs qui ont besoin de l'être
  checkCapsule();
  updateRandomShit();



  // Logique affichage/animation
  switch (myStateRaoul)
  {
    case JAMMED :
      while((analogRead(CAPTEUR1) < SEUIL_CAPTEUR1 || analogRead(CAPTEUR2) < SEUIL_CAPTEUR2))
      {
        if(!updateScroll())
        {
          setScrolling();
        }
      }
      strcpy(text, "merci !");
      myStateRaoul = AFFICHEPHRASE ;
      setScrolling();
      drawAdaptiveText();   
      break ;
    case BLINKCOUNTER :
      if(!updateBlinkText())
      {
        setState(AFFICHECAPS);
      }
      break ;
    case AFFICHECOMPTEUR :
      if(millis() - startCapsDisplay <= CAPS_DISPLAY_TIME )
      {
         if(!updateScroll())
         {
            setState(AFFICHEPHRASE);
         }
      }
      else setState(AFFICHEPHRASE);
      break ;
    case AFFICHECAPS :
      if(!updateScroll())
      {
        setState(AFFICHEPHRASE);
      }
      break ;
    case AFFICHEPHRASE :

      if(!updateScroll())
      {
        setState(AFFICHEANIM);
      }
      break ;
      
    case AFFICHEANIM:

        if(!animTab[current_anim].updateAnim())
        {
          setState(AFFICHECOMPTEUR);
        }

      break ;
    default :
      break;
  }
  delay(1);
}

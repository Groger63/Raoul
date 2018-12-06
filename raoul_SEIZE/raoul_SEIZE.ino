

#include <Arduino.h>
/* ----------------- Plein d'includes -------------------------*/
#include <HT1632.h>
#include <font_8x4.h>
#include <EEPROM.h>

#include <MusicPlayer.h>

#include <lib_anims.h>
#include "expressions.h"
#include "sounds.h"

#include<MemoryFree.h>
/* ----------------- Ecran LED -------------------------*/
#define pinCS1 3
#define pinWR 4
#define pinDATA 5

/* ----------------- Variables Diverses -------------------------*/

//Etat actuel du Raoul
typedef enum {AFFICHECOMPTEUR, AFFICHECAPS, AFFICHEPHRASE, AFFICHEANIM, BLINKCOUNTER} stateRaoul;
stateRaoul myStateRaoul = AFFICHEANIM ;



uint8_t cpt = 0 ; // Compteur de capsules
#define CPT_ADDR 0 // Adresse de la variable cpt dans l'eeprom

Anim current_anim = eyesAnim ; // L'animation en train d'être jouée


bool bCapsule = false;

const int PDCapsule = 2 ; //pin du bidule à capsule

// Pour l'affichage de texte qui défile
uint8_t wd;
uint8_t it;
char text[64] = "Roger vous couche" ; // Le texte à afficher sur l'écran
char soundPath[16]= "rom.mp3" ;
bool scrolling = false;
unsigned long lastScroll= 0 ;
unsigned long startCapsDisplay= 0 ;
#define TEXT_SCROLL_UPDATE_TIME 75 
#define CAPS_DISPLAY_TIME 10*1000 //10sec 

//pour le blink counter
bool blinked = true ;
byte blinkNumber = 0 ;
unsigned long lastBlink= 0 ;
#define BLINK_TIME 500 // 
#define BLINK_OCCURENCES 10 // 


//Différentes valeurs de temps pour s'y retrouver dans tout ce merdier
unsigned long lastCapsSeen = 0 ;

#define DELAY_BETWEEN_CAPSULES 500 //Allez tous niquer vos mères

/* ----------------- Fonctions Get Text -------------------------*/






void getCapsuleText(char* text)
{
  byte rand = random(CAPS_TEXT_NUMBER);


  strcpy_P(text, (PGM_P)pgm_read_word(&( capsuleText[rand])));
}

void playVictorySound()
{
  byte rand = random(VICTORY_SOUND_NUMBER);
  player.deleteSong(soundPath);

  strcpy_P(soundPath, (PGM_P)pgm_read_word(&( victorySounds[rand])));
  Serial.println(soundPath);
  player.playOne(soundPath);
}

void playRandomSound()
{
  byte rand = random(RANDOM_SOUND_NUMBER);
  //player.deleteSong(soundPath);

  strcpy_P(soundPath, (PGM_P)pgm_read_word(&( randomSounds[rand])));
  player.playOne(soundPath);
}


void getRandomText(char* text)
{
  byte rand = random(RANDOM_TEXT_NUMBER);
  //free(text);
  strcpy_P(text, (PGM_P)pgm_read_word(&( randomText[rand])) );
}

void dec2romanstr( char * res,int num){
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
    blinkNumber = 0;
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
  if (it <= wd + OUT_SIZE)
  {
    unsigned long elapsed = millis() - lastScroll  ;
    
    if(elapsed > TEXT_SCROLL_UPDATE_TIME)
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



/* ----------------- Fonctions Raoul  
-------------------------*/

void compteurInc()
{
  cpt++;
  //EEPROMWriteInt(CPT_ADDR, cpt); 
}

void compteurInterrupt()
{
  bCapsule = true;
  //Serial.println("Caps !");
}

void checkCapsule() 
{  //Test du compte de capsule
  
  bCapsule = (analogRead(A4) < 50 || analogRead(A5) < 50);
  
  if(bCapsule )
  {
    //On récupère le passage de capsule via une interruption hardware, ici on se contente juste de changer le state si besoin
    if( (millis() - lastCapsSeen ) > DELAY_BETWEEN_CAPSULES)
    {
      compteurInc();
      setState(BLINKCOUNTER);
      lastCapsSeen = millis() ;
      
    }
    bCapsule = false;
  }
}


/* ------------------------ DEBUG
 *  
 *  
 */

int lastMem =0 ;

/* ----------------- Fonctions ChangeState -------------------------*/


void setState(stateRaoul newState)
{
  myStateRaoul = newState;  
  Serial.print("change state ");
  switch(myStateRaoul)
  {
    case BLINKCOUNTER :
    //jouer du son
      playVictorySound() ;
      Serial.println("BLINKCOUNTER");
      lastBlink = millis();
      sprintf(text,"%d",cpt);
      setScrolling();
      drawAdaptiveText();
      break ;
    case AFFICHECOMPTEUR:
      Serial.println("AFFICHECOMPTEUR");
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
      Serial.println("AFFICHECAPS");
      getCapsuleText(text);
      setScrolling();
      drawAdaptiveText();
      break;
    case AFFICHEPHRASE:
      Serial.println("AFFICHEPHRASE");
      getRandomText(text);
      setScrolling();
      drawAdaptiveText();
      break;
    case AFFICHEANIM: 
      Serial.println("AFFICHEANIM");
    //random 
      //free(text);
      //current_anim = eyesAnim;
      current_anim.startAnim();
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


  // Ecran LED
  HT1632.begin(pinCS1, pinWR, pinDATA); //hasardeux, ça marche pas si je le mets avant, ni après, par contre avant ET après oui...
  cpt = 16 ;
  delay(1000);
  
  // Lecteur audio
  player.begin(); //will initialize the hardware and set default mode to be normal.
  player.keyDisable();

  delay(1000);
  // Ecran LED
  HT1632.begin(pinCS1, pinWR, pinDATA);
  

  pinMode(PDCapsule, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  randomSeed(analogRead(A5));
  // Attach interrupt pour la détection de la capsule
  //attachInterrupt(digitalPinToInterrupt(PDCapsule), compteurInterrupt, FALLING);


  wd = HT1632.getTextWidth(text, FONT_8X4_END, FONT_8X4_HEIGHT);

  setState(AFFICHEANIM);
  
}

/* ----------------- Fonction Loop -------------------------*/
void loop () {
  
  //debug
  if(lastMem != freeMemory() )
  {
    lastMem = freeMemory() ;
    Serial.println(lastMem);
  }

  
  //Update des trucs qui ont besoin de l'être
  player.play();
  checkCapsule();



  // Logique affichage/animation
  switch (myStateRaoul)
  {
    case BLINKCOUNTER :
      if(!updateBlinkText())
      {
        setState(AFFICHECAPS);
      }
      break ;
    case AFFICHECOMPTEUR :
      if(millis() - startCapsDisplay <= CAPS_DISPLAY_TIME )
      {
         updateScroll();
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

        if(!current_anim.updateAnim())
        {
          setState(AFFICHECOMPTEUR);
        }

      break ;
    default :
      break;
  }
  
}

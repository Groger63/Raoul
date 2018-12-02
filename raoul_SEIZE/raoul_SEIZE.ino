#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

#include <SD.h>
#include <SPI.h>
#include <arduino.h>
#include <MusicPlayer.h>


/*
   Configuration de l'ecran LED
*/
#define pinCS1 2
#define pinWR 3
#define pinDATA 4

int i = 0;

void setup () {  
  
  Serial.begin(9600);
  player.begin(); //will initialize the hardware and set default mode to be normal.
  player.keyDisable();
  player.addToPlaylist("A.mp3");
  player.addToPlaylist("B.mp3");
  HT1632.begin(pinCS1, pinWR, pinDATA);
}

void loop () {
  // Clear the previous image contents:
  player.play();
  HT1632.clear();

  // Draw a different image based on the frame number:
  if(i == 1) {
    HT1632.drawImage(YEUX_16, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }
  if(i == 2) {
    HT1632.drawImage(YEUX_16_GAUCHE, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }
  if(i == 3) {
    HT1632.drawImage(YEUX_16, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }
  if(i == 4) 
  {
    HT1632.drawImage(YEUX_16_DROITE, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }
  if(i == 5) 
  {
    HT1632.drawImage(YEUX_16_FERME, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }
  if(i == 6) 
  {
    HT1632.drawImage(YEUX_16_KAWAII, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }

  if(i == 7) 
  {
    i=0;
    HT1632.drawImage(YEUX_16_MORT, IMG_PROMO_WIDTH,  IMG_PROMO_HEIGHT, 0, 0);
  }

  
  i++ ;
  // Draw a different image based on the frame number:

  // Perform the drawing:
  HT1632.render();
  
  delay(1000);
}

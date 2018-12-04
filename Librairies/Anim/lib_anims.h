#include <Image.h>
#include <Anim.h>
#include <images.h>

#define ANIMATIONS_SIZE 2


/* --------- Animation yeux droite gauche ---------- */

Image img1(YEUX_16,32,8,2000);
Image img2(YEUX_16_DROITE_01,32,8, 25);
Image img3(YEUX_16_DROITE_02,32,8, 1000);
Image img4(YEUX_16_DROITE_01,32,8, 25);
Image img5(YEUX_16,32,8, 25);
Image img6(YEUX_16_GAUCHE_01,32,8, 25);
Image img7(YEUX_16_GAUCHE_02,32,8, 1000);
Image img8(YEUX_16_GAUCHE_01,32,8, 25);
Image img9(YEUX_16,32,8, 1000);
Image img10(YEUX_16_KAWAII,32,8, 1000);
Image img11(RUN1,32,8, 1000);
Image img12(RUN2,32,8, 1000);
Image img13(RUN3,32,8, 1000);


Image  anim_tab1 [] = {img1, img2, img3, img4, img5, img6, img7, img8, img9,img10} ;
Image  anim_tab2 [] = {img11, img12, img13};


Anim anim1(anim_tab1,10);
Anim anim1(anim_tab2,1);

// TODO : blinder sa race les animations

Anim animations[] = {anim1} ;
#include <Image.h>
#include <Anim.h>
#include <lib_images.h>

#define ANIMATIONS_SIZE 2


const Image img1(YEUX_16,32,8,2000);
const Image img2(YEUX_16_DROITE_01,32,8, 25);
const Image img3(YEUX_16_DROITE_02,32,8, 1000);
  //Image img4(YEUX_16_DROITE_01,32,8, 25);
const Image img5(YEUX_16,32,8, 25);
const Image img6(YEUX_16_GAUCHE_01,32,8, 25);
const Image img7(YEUX_16_GAUCHE_02,32,8, 1000);
  //Image img8(YEUX_16_GAUCHE_01,32,8, 25);
const Image img9(YEUX_16,32,8, 1000);
const Image img10(YEUX_16_KAWAII,32,8, 2*1000);

const Image anim_tab1 [] = {img1, img2, img3, img2, img5, img6, img7, img6, img9} ;
const Image anim_tab2 [] = {img10};

const Anim eyesAnim(anim_tab1,9,5);
const Anim kawaiAnim(anim_tab2,1,1);


const Anim  animTab [ANIMATIONS_SIZE] = {eyesAnim,kawaiAnim} ;
/* --------- Animation yeux droite gauche ---------- */





// TODO : blinder sa race les animations

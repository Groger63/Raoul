
#ifndef Anim_h
#define Anim_h


#include "Arduino.h"
#include <Image.h>

class Anim{
	private :
		Image * images ;
		int currentImg;
		int nbrImages ;
		unsigned long startTime ;
	public :


		Anim(Image imgs[], int nbimg );
		Anim();
		~Anim();
		void startAnim();
		void updateAnim();


};

#endif

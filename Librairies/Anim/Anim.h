
#ifndef Anim_h
#define Anim_h


#include "Arduino.h"
#include <Image.h>

class Anim{
	private :
		Image * images ;
		uint8_t currentImg;
		uint8_t nbrImages ;
		uint8_t repeat ;
		uint8_t repetitions ;
		unsigned long startTime ;
	public :


		Anim(Image * imgs, uint8_t nbimg, uint8_t rep );
		Anim();
		~Anim();
		void startAnim();
		bool updateAnim();


};

#endif

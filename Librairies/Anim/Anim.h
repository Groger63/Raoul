
#ifndef Anim_h
#define Anim_h


#include "Arduino.h"
#include <Image.h>

class Anim{
	private :
		Image * images ;
		byte currentImg;
		byte nbrImages ;
		unsigned long startTime ;
	public :


		Anim(Image * imgs, int nbimg );
		Anim();
		~Anim();
		void startAnim();
		bool updateAnim();


};

#endif

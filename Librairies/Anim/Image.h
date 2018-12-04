
#ifndef Image_h
#define Image_h


#include "Arduino.h"
#include <HT1632.h>

class Image{
	private :
		byte * matrix;
		byte matrixWidth, matrixHeight ;
		int displayTime ;
	public :

		Image(byte * mat, byte Width,byte Height, int _displayTime );
		~Image();

		void display();

		int getDisplayTime();

};

#endif

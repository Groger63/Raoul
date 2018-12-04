#include <HT1632.h>
#include <Image.h>


void Image::display()
{
	HT1632.clear();
	HT1632.drawImage(matrix,matrixWidth,matrixHeight,0,0);
  	HT1632.render();
}

Image::Image(byte * mat, byte Width,byte Height,int _displayTime) 
: matrixWidth(Width),matrixHeight(Height),displayTime(_displayTime)
{
	matrix = mat ;
}

int Image::getDisplayTime()
{
	return displayTime; 
}

Image::~Image()
{
	free(matrix);
}
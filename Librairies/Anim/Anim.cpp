#include <lib_images.h>
#include <Anim.h>



Anim::Anim(Image imgs[], int nbimg )
 :  nbrImages(nbimg) , currentImg(0) ,startTime(-1)
{
	images = imgs ;
}
Anim::Anim()
 : nbrImages(0) , currentImg(0) ,startTime(-1)
{
}

Anim::~Anim()
{
	//for(int i = 0 ; i < nbrImages ; i++) delete images[i] ;
	free(images);
}


void Anim::startAnim()
{
	currentImg = 0 ;
	startTime = millis();
	images[currentImg].display();
}

bool Anim::updateAnim()
{
	if((millis() - startTime) > images[currentImg].getDisplayTime() )  // next img
	{

		currentImg ++ ;
		if(currentImg>=nbrImages ) return false ;
		currentImg = currentImg%(nbrImages);

		startTime = millis();

		images[currentImg].display();

	}
	return true ;

}
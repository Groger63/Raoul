#include <lib_images.h>
#include <Anim.h>



Anim::Anim(Image imgs[], uint8_t nbimg, uint8_t rep )
 :  nbrImages(nbimg) , currentImg(0) ,startTime(-1), repeat(rep) ,repetitions(0)
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
	repetitions = 0 ;
	currentImg = 0 ;
	startTime = millis();
	images[currentImg].display();
}

bool Anim::updateAnim()
{
	if((millis() - startTime) > images[currentImg].getDisplayTime() )  // next img
	{

		currentImg ++ ;

		if(currentImg>=nbrImages ) 
			{
				repetitions ++ ;
				currentImg = 0 ;
				if(repetitions >= repeat)
				{
					return false ;
				}
			}



		//currentImg = currentImg%(nbrImages);

		startTime = millis();

		images[currentImg].display();

	}
	return true ;

}
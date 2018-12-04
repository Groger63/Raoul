#include <image.h>
#include <arduino.h>
#include <Timer.h>
#include <Anim.h>



Anim::Anim(Image * imgs, int nbimg )
 : images(imgs) , nbrImages(nbimg) , currentImg(0) ,startTime(-1)
{
}
Anim::Anim()
 : nbrImages(0) , currentImg(0) ,startTime(-1)
{
}

Anim::~Anim()
{
	free(images);
}


void Anim::startAnim()
{
	startTime = millis();
	images[currentImg].display();
}

void Anim::updateAnim()
{
	int duration = images[currentImg].getDisplayTime();
	if((millis() - startTime) > duration )  // next img
	{
		


		currentImg ++ ;
		currentImg = currentImg%(nbrImages);

		startTime = millis();

		images[currentImg].display();


	}

}
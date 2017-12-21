#include "graphic_controller.h"

static GraphicStatus graphicStatus;

static bool threadFlag = false;
static pthread_mutex_t graphicMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t gfxThread;

static timer_t volumeTimer;
static timer_t channelNumberTimer;
static timer_t infoBannerTimer;

static void* graphicControllerTask();
static int32_t initTimer(timer_t timer);
static void hideVolume(union sigval signalArg);
static void hideChannelNumber(union sigval signalArg);
static void hideInfoBanner(union sigval signalArg);

GraphicControllerError graphicConntrollerInit()
{
	
	if (pthread_create(&gfxThread, NULL, &graphicControllerTask, NULL))
	{
		printf("Error creating graphic rendering task!\n");
		return GRAPHIC_ERROR;
	}

	return GRAPHIC_NO_ERROR;
}

GraphicControllerError graphicConntrollerDeinit()
{
    threadFlag = true;
    if (pthread_join(gfxThread, NULL))
    {
        printf("\n%s : ERROR pthread_join fail!\n", __FUNCTION__);
        return GRAPHIC_THREAD_ERROR;
    }

	return GRAPHIC_NO_ERROR;
}

GraphicControllerError showVolume(const uint8_t value)
{
	if(value <= 10 && value >= 0){
		graphicStatus.showVolume = true;
		graphicStatus.volumeValue = value;
	}
}

GraphicControllerError showChannelNumber(const uint8_t value)
{
	
}

GraphicControllerError showInfoBanner(const char* text)
{
	
}

void* graphicControllerTask()
{
	while(!threadFlag){
		/* Show volume? */
		if(){

		}
		/* Show channel number? */
		if(){

		}
		/* Show info banner? */
		if(){

		}
		sleep(16);
	}
}

int32_t initTimer(timer_t timer){

}

void hideVolume(union sigval signalArg){

}

void hideChannelNumber(union sigval signalArg){

}

void hideInfoBanner(union sigval signalArg){

}

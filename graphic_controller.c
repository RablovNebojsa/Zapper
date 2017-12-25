#include "graphic_controller.h"

/* helper macro for error checking */
#define DFBCHECK(x...)                                      \
{                                                           \
DFBResult err = x;                                          \
                                                            \
if (err != DFB_OK)                                          \
  {                                                         \
    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
    DirectFBErrorFatal( #x, err );                          \
  }                                                         \
}


static GraphicStatus graphicStatus;
static IDirectFBSurface *primary = NULL;
IDirectFB *dfbInterface = NULL;
static IDirectFBFont *fontInterface = NULL;
static int32_t screenWidth = 0;
static int32_t screenHeight = 0;

static bool threadFlag = false;
static pthread_mutex_t graphicMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t gfxThread;
static pthread_cond_t deinitCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t deinitMutex = PTHREAD_MUTEX_INITIALIZER;

static int32_t timerFlags = 0;
static timer_t volumeTimer;
static timer_t channelNumberTimer;
static timer_t infoBannerTimer;
static struct itimerspec timerSpec;
static struct itimerspec timerSpecOld;

static void* graphicControllerTask();
static int32_t drawVolume();
static int32_t drawChannelNumber();
static int32_t drawInfoBanner();
static void hideVolume(union sigval signalArg);
static void hideChannelNumber(union sigval signalArg);
static void hideInfoBanner(union sigval signalArg);
static int32_t timerInit();
static int32_t timerDeinit();

GraphicControllerError graphicConntrollerInit()
{
	graphicStatus.showVolume = false;
	graphicStatus.showChannelNumber = false;
	graphicStatus.showInfoBanner = false;

	if(dfbInit()){
		printf("\n%s: DFB initialization error!\n",__FUNCTION__);
		return GRAPHIC_ERROR;
	}
	if(timerInit()){
		printf("\n%s: Timer initialization error!\n",__FUNCTION__);
		primary->Release(primary);
		dfbInterface->Release(dfbInterface);
		return GRAPHIC_ERROR;
	}

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
	pthread_mutex_lock(&deinitMutex);
	if (ETIMEDOUT == pthread_cond_wait(&deinitCond, &deinitMutex))
	{
		printf("\n%s : ERROR Lock timeout exceeded!\n", __FUNCTION__);
	}
	pthread_mutex_unlock(&deinitMutex);

    if (pthread_join(gfxThread, NULL))
    {
        printf("\n%s : ERROR pthread_join fail!\n", __FUNCTION__);
        return GRAPHIC_THREAD_ERROR;
    }
	dfbDeinit();
    
	timerDeinit();

	return GRAPHIC_NO_ERROR;
}

GraphicControllerError showVolume(const uint32_t value)
{
	int32_t timerFlags = 0;
	/* stop volumeTimer timer */
	memset(&timerSpec,0,sizeof(timerSpec));
	if(timer_settime(volumeTimer,0,&timerSpec,&timerSpecOld) == -1){
		printf("Error setting timer in %s!\n", __FUNCTION__);
	}
	
	graphicStatus.showVolume = true;
	graphicStatus.volumeValue = value;
	
	/* specify the timer timeout time */
	timerSpec.it_value.tv_sec = 3;
	timerSpec.it_value.tv_nsec = 0;

	if(timer_settime(volumeTimer,timerFlags,&timerSpec,&timerSpecOld) == -1){
		printf("Error setting timer in %s!\n", __FUNCTION__);
		return 1;
	}

	return GRAPHIC_NO_ERROR;
}

GraphicControllerError showChannelNumber(const uint32_t value)
{

	/* stop channelNumberTimer timer */
    memset(&timerSpec,0,sizeof(timerSpec));
    if(timer_settime(channelNumberTimer,0,&timerSpec,&timerSpecOld) == -1){
        printf("Error setting timer in %s!\n", __FUNCTION__);
    }
	
	graphicStatus.showChannelNumber = true;
	graphicStatus.channelNumberValue = value;
	
	/* specify the timer timeout time */
	timerSpec.it_value.tv_sec = 3;
	timerSpec.it_value.tv_nsec = 0;
	
	if(timer_settime(channelNumberTimer,timerFlags,&timerSpec,&timerSpecOld) == -1){
		printf("Error setting timer in %s!\n", __FUNCTION__);
		return 1;
	}
	return GRAPHIC_NO_ERROR;
}

GraphicControllerError showInfoBanner(const char* name, const char* start, const char* pid, const char* channel)
{
	/* stop infoBannerTimer timer */
    memset(&timerSpec,0,sizeof(timerSpec));
    if(timer_settime(infoBannerTimer,0,&timerSpec,&timerSpecOld) == -1){
        printf("Error setting timer in %s!\n", __FUNCTION__);
    }	
	graphicStatus.showInfoBanner = true;
	strcpy(graphicStatus.infoBannerName, name);
	strcpy(graphicStatus.infoBannerStart, start);
	strcpy(graphicStatus.infoBannerPid, pid);	
	strcpy(graphicStatus.infoBannerChannel, channel);	

	/* specify the timer timeout time */
	timerSpec.it_value.tv_sec = 5;
	timerSpec.it_value.tv_nsec = 0;	
	if(timer_settime(infoBannerTimer,timerFlags,&timerSpec,&timerSpecOld) == -1){
		printf("Error setting timer in %s!\n", __FUNCTION__);
		return 1;
	}
	return GRAPHIC_NO_ERROR;
}

void* graphicControllerTask()
{
	while(!threadFlag){
		DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x00));
    	DFBCHECK(primary->FillRectangle(primary, 0, 0, screenWidth, screenHeight));
		/* Show volume? */
		if(graphicStatus.showVolume){
			drawVolume();		
		}
		/* Show channel number? */
		if(graphicStatus.showChannelNumber){
			drawChannelNumber();
		}
		/* Show info banner? */
		if(graphicStatus.showInfoBanner){
			drawInfoBanner();
		}
	    /* update screen */
		DFBCHECK(primary->Flip(primary, NULL, 0));
		usleep(16000);
	}
	pthread_mutex_lock(&deinitMutex);
	pthread_cond_signal(&deinitCond);
	pthread_mutex_unlock(&deinitMutex); 
}

int32_t timerInit()
{
	struct sigevent signalEvent;
	int ret;
	signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = hideVolume;
	signalEvent.sigev_value.sival_ptr = NULL;
	signalEvent.sigev_notify_attributes = NULL;

	ret = timer_create(/*clock for time measuring*/CLOCK_REALTIME,
                       /*timer settings*/&signalEvent,
                       /*where to store the ID of the newly created timer*/&volumeTimer);
	if(ret != 0){
		printf("\nTimer cannot be created.\n");
		return 1;
	}

	signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = hideChannelNumber;
	signalEvent.sigev_value.sival_ptr = NULL;
	signalEvent.sigev_notify_attributes = NULL;

	ret = timer_create(/*clock for time measuring*/CLOCK_REALTIME,
                       /*timer settings*/&signalEvent,
                       /*where to store the ID of the newly created timer*/&channelNumberTimer);
	if(ret != 0){
		printf("\nTimer cannot be created.\n");
		return 1;
	}

	signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = hideInfoBanner;
	signalEvent.sigev_value.sival_ptr = NULL;
	signalEvent.sigev_notify_attributes = NULL;

	ret = timer_create(/*clock for time measuring*/CLOCK_REALTIME,
                       /*timer settings*/&signalEvent,
                       /*where to store the ID of the newly created timer*/&infoBannerTimer);
	if(ret != 0){
		printf("\nTimer cannot be created.\n");
		return 1;
	}

	return 0;
}

int32_t timerDeinit(){
	timer_delete(volumeTimer);
	timer_delete(channelNumberTimer);
	timer_delete(infoBannerTimer);
	return 0;
}

int32_t dfbInit()
{
	/* initialize DirectFB */
	DFBSurfaceDescription surfaceDesc;
	DFBCHECK(DirectFBInit(NULL, NULL));
	DFBCHECK(DirectFBCreate(&dfbInterface));
	DFBCHECK(dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN));
	
    /* create primary surface with double buffering enabled */  
	surfaceDesc.flags = DSDESC_CAPS;
	surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	DFBCHECK (dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary));

	/* fetch the screen size */
    DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));
	
	return 0;
}

int32_t dfbDeinit()
{
	primary->Release(primary);
	dfbInterface->Release(dfbInterface);
	
	return 0;
}

int32_t drawVolume()
{
	IDirectFBImageProvider *provider;
	DFBSurfaceDescription surfaceDesc;
	IDirectFBSurface *logoSurface = NULL;
	int32_t logoHeight, logoWidth;
	
    /* create the image provider for the specified file */
	switch(graphicStatus.volumeValue){
		case 0: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_0.png", &provider));
			break;
		case 1: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_1.png", &provider));
			break;	
		case 2: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_2.png", &provider));
			break;
		case 3: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_3.png", &provider));
			break;
		case 4: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_4.png", &provider));
			break;
		case 5: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_5.png", &provider));
			break;	
		case 6: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_6.png", &provider));
			break;	
		case 7: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_7.png", &provider));
			break;
		case 8: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_8.png", &provider));
			break;	
		case 9: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_9.png", &provider));
			break;
		case 10: 
			DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "volume_10.png", &provider));
			break;
		default:
			printf("\nVolume: %d\n",graphicStatus.volumeValue);
	}
    /* get surface descriptor for the surface where the image will be rendered */
	DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDesc));
    /* create the surface for the image */
	DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &logoSurface));
    /* render the image to the surface */
	DFBCHECK(provider->RenderTo(provider, logoSurface, NULL));
	
    /* cleanup the provider after rendering the image to the surface */
	provider->Release(provider);
	
    /* fetch the logo size and add (blit) it to the screen */
	DFBCHECK(logoSurface->GetSize(logoSurface, &logoWidth, &logoHeight));
	DFBCHECK(primary->Blit(primary, logoSurface, NULL, screenWidth/2 - logoWidth/2, screenHeight/2 - logoHeight/2));
 	
	return 0;
}

int32_t drawChannelNumber()
{
	char channelNumberString[4];
	
    /* draw the frame */
    DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x9f));
    DFBCHECK(primary->FillRectangle(primary, screenWidth/10, screenHeight/10, screenWidth/10 + FRAME_WIDTH, screenHeight/10 + FRAME_HEIGHT));

    /* generate channel number string */
    sprintf(channelNumberString,"%d",graphicStatus.channelNumberValue);	

	DFBFontDescription fontDesc;
	
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = CHANNEL_NUMBER_HEIGHT;
	
	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(primary->SetFont(primary, fontInterface));
	
	/* draw the string */
    DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0xff));
	DFBCHECK(primary->DrawString(primary, channelNumberString, -1, screenWidth/10 + 50 , screenHeight/10 + 50, DSTF_CENTER));

	return 0;
}

int32_t drawInfoBanner()
{
	DFBFontDescription fontDesc;
	
    /* draw the frame */
    DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0x9f));
    DFBCHECK(primary->FillRectangle(primary, 0, screenHeight - INFO_BANNER_HEIGHT, screenWidth, screenHeight));
	
    /* specify the height of the font by raising the appropriate flag and setting the height value */
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 40;
	
    /* create the font and set the created font for primary surface text drawing */
	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(primary->SetFont(primary, fontInterface));
    
    /* draw the text */
    DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0xff));
	DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ graphicStatus.infoBannerChannel,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ 3,
                                 /*y coordinate of the lower left corner of the resulting text*/ screenHeight - 159,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
	DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ graphicStatus.infoBannerName,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ 3,
                                 /*y coordinate of the lower left corner of the resulting text*/ screenHeight - 106,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
	DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ graphicStatus.infoBannerStart,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ 3,
                                 /*y coordinate of the lower left corner of the resulting text*/ screenHeight - 53,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));

	DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ graphicStatus.infoBannerPid,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ 3,
                                 /*y coordinate of the lower left corner of the resulting text*/ screenHeight - 3,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
	return 0;	
}

void hideVolume(union sigval signalArg)
{
	graphicStatus.showVolume = false;
}

void hideChannelNumber(union sigval signalArg)
{
	graphicStatus.showChannelNumber = false;
}

void hideInfoBanner(union sigval signalArg)
{
	graphicStatus.showInfoBanner = false;
}

#include "remote_controller.h"
#include "stream_controller.h"
#include "config_parser.h"
#include "graphic_controller.h"

static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
   char command[13];

   /* command is the control command to the terminal */
   sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
   printf("%s", command);
}

/* macro function for error checking */
#define ERRORCHECK(x)                                                       \
{                                                                           \
if (x != 0)                                                                 \
 {                                                                          \
    textColor(1,1,0);                                                       \
    printf(" Error!\n File: %s \t Line: <%d>\n", __FILE__, __LINE__);       \
    textColor(0,7,0);                                                       \
    return -1;                                                              \
 }                                                                          \
}

#define MUTE 0
#define MIN_VOLUME 0
#define MAX_VOLUME 10
#define DEFAULT_VOLUME 3
#define VOLUME_INCEMENT 200000000

static char infoBannerStart[30];
static char infoBannerName[50];
static char infoBannerPid[50];
static char infoBannerChannel[30];
static uint32_t playerVolume = 0;
static bool isMuted = false;
static int32_t volume = DEFAULT_VOLUME;
static uint32_t programNumberBuffer;
static int16_t counter = 1;
static timer_t keyPressTimer;
static Config configData;
	
static pthread_cond_t deinitCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t deinitMutex = PTHREAD_MUTEX_INITIALIZER;
static ChannelInfo channelInfo;

static void remoteControllerCallback(uint16_t code, uint16_t type, uint32_t value);
static int32_t initTimer();
static int32_t numKeyPressed(uint32_t keyPressed);
static void timeOut(union sigval signalArg);

int main(int argc, char *argv[])
{
	/* Reading initial data from config file */
	ERRORCHECK(parseConfigFile((char*)argv[1], &configData));	

	/* initialize graphic controller module */
	ERRORCHECK(graphicConntrollerInit());

	/* Initialize keyPressTimer */
	ERRORCHECK(initTimer());

	/* initialize remote controller module */
	ERRORCHECK(remoteControllerInit(configData));

	/* register remote controller callback */
	ERRORCHECK(registerRemoteControllerCallback(remoteControllerCallback));

	/* initialize stream controller module */
	ERRORCHECK(streamControllerInit(configData));

	/* Set default volume on start */
	setPlayerVolume(DEFAULT_VOLUME * VOLUME_INCEMENT);

	/* wait for a EXIT remote controller key press event */
	pthread_mutex_lock(&deinitMutex);
	if (ETIMEDOUT == pthread_cond_wait(&deinitCond, &deinitMutex))
	{
		printf("\n%s : ERROR Lock timeout exceeded!\n", __FUNCTION__);
	}
	pthread_mutex_unlock(&deinitMutex);

	//ERRORCHECK(graphicConntrollerDeinit);

	/* unregister remote controller callback */
	ERRORCHECK(unregisterRemoteControllerCallback(remoteControllerCallback));

	/* deinitialize remote controller module */
	ERRORCHECK(remoteControllerDeinit());

	/* deinitialize stream controller module */
	ERRORCHECK(streamControllerDeinit());

	/* deinitialize graphic controller module */
	ERRORCHECK(graphicConntrollerDeinit);

	timer_delete(keyPressTimer);
	  
	return 0;
}

void remoteControllerCallback(uint16_t code, uint16_t type, uint32_t value)
{
    switch(code)
	{
		case KEYCODE_1:
			printf("\nKey 1 pressed\n");
			numKeyPressed(1);
			break;
		case KEYCODE_2:
			printf("\nKey 2 pressed\n");
			numKeyPressed(2);
			break;
		case KEYCODE_3:
			printf("\nKey 3 pressed\n");
			numKeyPressed(3);
			break;
		case KEYCODE_4:
			printf("\nKey 4 pressed\n");
			numKeyPressed(4);
			break;
		case KEYCODE_5:
			printf("\nKey 5 pressed\n");
			numKeyPressed(5);
			break;
		case KEYCODE_6:
			printf("\nKey 6 pressed\n");
			numKeyPressed(6);
			break;
		case KEYCODE_7:
			printf("\nKey 7 pressed\n");
			numKeyPressed(7);
			break;
		case KEYCODE_8:
			printf("\nKey 8 pressed\n");
			numKeyPressed(8);
			break;
		case KEYCODE_9:
			printf("\nKey 9 pressed\n");
			numKeyPressed(9);
			break;
		case KEYCODE_0:
			printf("\nKey 0 pressed\n");
			numKeyPressed(0);
			break;
		case KEYCODE_INFO:
		    printf("\nInfo pressed\n");          
		    if (getChannelInfo(&channelInfo) == SC_NO_ERROR)
		    {	
				sprintf(infoBannerChannel, "Channel %d",channelInfo.programNumber);
				sprintf(infoBannerPid, "Video PID: %d, Audio PID: %d",channelInfo.videoPid, channelInfo.audioPid);
				sprintf(infoBannerName,"Event name: %s", channelInfo.eventName + 1);
				sprintf(infoBannerStart,"Start time: %x:%x:%x", channelInfo.startTime[2], channelInfo.startTime[3], channelInfo.startTime[4]);
				showInfoBanner(infoBannerName, infoBannerStart, infoBannerPid, infoBannerChannel);

		        printf("\n********************* Channel info *********************\n");
		        printf("Program number: %d\n", channelInfo.programNumber);
				printf("Service ID: %d\n", channelInfo.serviceId);
				printf("Event Name: %s\n", channelInfo.eventName);
				printf("Start time: %x:%x:%x\n", channelInfo.startTime[2], channelInfo.startTime[3], channelInfo.startTime[4]);
		        printf("**********************************************************\n");
		    }
			break;
		case KEYCODE_P_PLUS:
			printf("\nCH+ pressed\n");
            channelUp();
			break;
		case KEYCODE_P_MINUS:
		    printf("\nCH- pressed\n");
            channelDown();
			break;
		case KEYCODE_V_PLUS:
			if(volume < MAX_VOLUME){			
				volume++;
			}
			setPlayerVolume(volume * VOLUME_INCEMENT);
			isMuted = false;
			showVolume(volume);
			break;
		case KEYCODE_V_MINUS:
			if(volume > MIN_VOLUME){
				volume--;
			}
			setPlayerVolume(volume * VOLUME_INCEMENT);
			isMuted = false;
			showVolume(volume);
			break;
		case KEYCODE_MUTE:
			if(isMuted){
				isMuted = false;
				volume = DEFAULT_VOLUME;
				setPlayerVolume(volume * VOLUME_INCEMENT);
				showVolume(volume);
			}else{
				isMuted = true;
				showVolume(MUTE);
				setPlayerVolume(MUTE * VOLUME_INCEMENT);
				volume = DEFAULT_VOLUME;
			}
			break;
		case KEYCODE_EXIT:
			printf("\nExit pressed\n");
			pthread_mutex_lock(&deinitMutex);
			pthread_cond_signal(&deinitCond);
			pthread_mutex_unlock(&deinitMutex);
			break;
		default:
			printf("\nPress P+, P-, info or exit! \n\n");
	}
}

int32_t numKeyPressed(uint32_t keyPressed)
{
	struct itimerspec timerSpec;
	struct itimerspec timerSpecOld;
	int32_t timerFlags = 0;	
	memset(&timerSpec,0,sizeof(timerSpec));
	timer_settime(keyPressTimer,timerFlags,&timerSpec,&timerSpecOld);
	if(counter == 3){
		programNumberBuffer = (programNumberBuffer * 10) + keyPressed;
printf("*** %d",programNumberBuffer);
		if(channelChange(programNumberBuffer)){
			printf("\n Channel %d unavailable!\n", programNumberBuffer);
		}
		showChannelNumber(programNumberBuffer);
		counter = 1;
		return 0;
	}else if(counter == 2) {
		programNumberBuffer = (programNumberBuffer * 10) + keyPressed;
		showChannelNumber(programNumberBuffer);
		counter = 3;
	}else{
		programNumberBuffer = keyPressed;
		showChannelNumber(programNumberBuffer);
		counter = 2;
	}
	memset(&timerSpec,0,sizeof(timerSpec));
	/* specify the timer timeout time */
	timerSpec.it_value.tv_sec = 1;
	timerSpec.it_value.tv_nsec = 0;

	if(timer_settime(keyPressTimer,timerFlags,&timerSpec,&timerSpecOld) == -1){
		printf("\nError setting timer!\n");
		return 1;
	}
	return 0;
}

/* Initialize linux timer to count time 
 * between two key presses
 */
int32_t initTimer(){

	struct sigevent signalEvent;
	int ret;
	signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = timeOut;
	signalEvent.sigev_value.sival_ptr = NULL;
	signalEvent.sigev_notify_attributes = NULL;
	ret = timer_create(/*clock for time measuring*/CLOCK_REALTIME,
                       /*timer settings*/&signalEvent,
                       /*where to store the ID of the newly created timer*/&keyPressTimer);
	if(ret != 0){
		printf("\nTimer can not be created.\n");
		return 1;
	}
	printf("\n*** Timer initalized!\n");
	return 0;
}

void timeOut(union sigval signalArg){
	counter = 1;
	if(channelChange(programNumberBuffer)){
		printf("\n Channel %d unavailable!\n", programNumberBuffer);
	}
}

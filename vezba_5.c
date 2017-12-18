#include "remote_controller.h"
#include "stream_controller.h"
#include "config_parser.h"

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

static int32_t programNumberBuffer;
static int16_t counter = 1;
static timer_t keyPressTimer;
static Config configData;
	
static pthread_cond_t deinitCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t deinitMutex = PTHREAD_MUTEX_INITIALIZER;
static ChannelInfo channelInfo;

static void remoteControllerCallback(uint16_t code, uint16_t type, uint32_t value);
static int32_t initTimer();
static int32_t numKeyPressed(uint16_t keyPressed);
static void timeOut(union sigval signalArg);

int main(int argc, char *argv[])
{
	ERRORCHECK(parseConfigFile((char*)argv[1], &configData));	

	ERRORCHECK(initTimer());
    /* initialize remote controller module */
    ERRORCHECK(remoteControllerInit(configData));
    
    /* register remote controller callback */
    ERRORCHECK(registerRemoteControllerCallback(remoteControllerCallback));
    
    /* initialize stream controller module */
    ERRORCHECK(streamControllerInit(configData));

    /* wait for a EXIT remote controller key press event */
    pthread_mutex_lock(&deinitMutex);
	if (ETIMEDOUT == pthread_cond_wait(&deinitCond, &deinitMutex))
	{
		printf("\n%s : ERROR Lock timeout exceeded!\n", __FUNCTION__);
	}
	pthread_mutex_unlock(&deinitMutex);
    
    /* unregister remote controller callback */
    ERRORCHECK(unregisterRemoteControllerCallback(remoteControllerCallback));

    /* deinitialize remote controller module */
    ERRORCHECK(remoteControllerDeinit());

    /* deinitialize stream controller module */
    ERRORCHECK(streamControllerDeinit());
  
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
		        printf("\n********************* Channel info *********************\n");
		        printf("Program number: %d\n", channelInfo.programNumber);
		        printf("Audio pid: %d\n", channelInfo.audioPid);
		        printf("Video pid: %d\n", channelInfo.videoPid);
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

int32_t numKeyPressed(uint16_t keyPressed)
{
	struct itimerspec timerSpec;
	struct itimerspec timerSpecOld;
	int32_t timerFlags = 0;	
	memset(&timerSpec,0,sizeof(timerSpec));
	timer_settime(keyPressTimer,timerFlags,&timerSpec,&timerSpecOld);
	if(counter == 3){
		programNumberBuffer = (programNumberBuffer * 10) + keyPressed;
		if(channelChange(programNumberBuffer)){
			printf("\n Channel %d unavailable!\n", programNumberBuffer);
		}
		counter = 1;
		return 0;
	}else if(counter == 2) {
		programNumberBuffer = (programNumberBuffer * 10) + keyPressed;
		counter = 3;
	}else{
		programNumberBuffer = keyPressed;
		counter = 2;
	}
	memset(&timerSpec,0,sizeof(timerSpec));
	/* specify the timer timeout time */
	timerSpec.it_value.tv_sec = 3;
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

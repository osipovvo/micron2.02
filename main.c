/*
	application code v.1
	for uspd micron 2.0x

	2012 - January
	NPO Frunze
	RUSSIA NIGNY NOVGOROD

	OSIPOV V, SHASHUNKIN D, OSKOLKOVA O, UHOV E
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "pio.h"
#include "cli.h"
#include "storage.h"
#include "serial.h"
#include "plc.h"
#include "pool.h"
#include "connection.h"
#include "micron_v1.h"
#include "exio.h"
#include "debug.h"


extern BOOL alreadyInSyncRoutine;

//#define NO_CONNECTION

int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused)))
{
	int res = ERROR_GENERAL;

	//init COMMON part
	COMMON_Initialize();

	//init TRACE part
	DEBUG_InitTrace();

	//initialize pio
	res = PIO_Initialize();
	COMMON_STR_DEBUG(DEBUG_MAIN "PIO_Initialize() %s", getErrorCode(res));
	if (res != OK){
		EXIT_PATTERN;
	}

	//initialize cli
	res = CLI_Initialize();
	COMMON_STR_DEBUG(DEBUG_MAIN "CLI_Initialize() %s", getErrorCode(res));
	if (res != OK){
		EXIT_PATTERN;
	}

	//initialise storage engine
	res = STORAGE_Initialize();
	COMMON_STR_DEBUG(DEBUG_MAIN "STORAGE_Initialize() %s", getErrorCode(res));
	if ((res != OK) && (res != ERROR_FILE_OPEN_ERROR)){
		goto waitBeforeExit;
	}

	//initialise serial ports
	res = SERIAL_Initialize();
	COMMON_STR_DEBUG(DEBUG_MAIN "SERIAL_Initialize() %s", getErrorCode(res));
	if (res != OK){
		goto waitBeforeExit;
	}

	#if (REV_COMPILE_EXIO == 1) && (REV_COMPILE_PLC == 1)
		#error Flags ExIO and PLC are both ON
	#elif (REV_COMPILE_EXIO == 0) && (REV_COMPILE_PLC == 0)
		#warning Flags ExIO and PLC are both OFF
	#else //(REV_COMPILE_EXIO == 1) && (REV_COMPILE_PLC == 1)

		#if (REV_COMPILE_PLC == 1)
			//initialize rf part
			res = PLC_Initialize();
			COMMON_STR_DEBUG(DEBUG_MAIN "PLC_Initialize() %s", getErrorCode(res));
			if (res != OK){
				goto waitBeforeExit;
			}
		#elif (REV_COMPILE_EXIO == 1)
			//initialize rf part
			res = EXIO_Initialize();
			COMMON_STR_DEBUG(DEBUG_MAIN "EXIO_Initialize() %s", getErrorCode(res));
			if (res != OK){
				goto waitBeforeExit;
			}
		#endif // (REV_COMPILE_EXIO == 1)


	#endif //(REV_COMPILE_EXIO == 1) && (REV_COMPILE_PLC == 1)


	//initialize connection stack
	#ifndef NO_CONNECTION
	res = CONNECTION_Initialize();
	COMMON_STR_DEBUG(DEBUG_MAIN "CONNECTION_Initialize() %s", getErrorCode(res));
	if (res != OK){
		goto waitBeforeExit;
	}
	#endif

	//initialise pooler engine
	res = POOL_Initialize();
	COMMON_STR_DEBUG(DEBUG_MAIN "POOL_Initialize() %s", getErrorCode(res));
	if (res != OK){
		goto waitBeforeExit;
	}	
	//do main protocol job
	do{
		SVISOR_THREAD_SEND(THREAD_MAIN);
		if ( STORAGE_GetSeasonAllowFlag() == STORAGE_SEASON_CHANGE_ENABLE )
		{
			unsigned char savedSeason = STORAGE_GetCurrentSeason();

			dateTimeStamp springDts , autumnDts , currentDts;
			memset( &springDts , 0 , sizeof(dateTimeStamp) );
			memset( &autumnDts , 0 , sizeof(dateTimeStamp) );
			memset( &currentDts , 0 , sizeof(dateTimeStamp) );
			COMMON_GetCurrentDts( &currentDts );

			COMMON_GetSeasonChangingDts( currentDts.d.y , savedSeason , &springDts , &autumnDts );

//			COMMON_STR_DEBUG( DEBUG_MAIN "current dts " DTS_PATTERN , DTS_GET_BY_VAL(currentDts) );
//			COMMON_STR_DEBUG( DEBUG_MAIN "autumn dts " DTS_PATTERN , DTS_GET_BY_VAL(autumnDts) );
//			COMMON_STR_DEBUG( DEBUG_MAIN "spring dts " DTS_PATTERN , DTS_GET_BY_VAL(springDts) );

			if ( COMMON_DtsInRange( &currentDts , &springDts , &autumnDts) == TRUE )
			{
				//summer
				if( savedSeason != STORAGE_SEASON_SUMMER )
				{
					COMMON_STR_DEBUG( DEBUG_MAIN "current dts " DTS_PATTERN , DTS_GET_BY_VAL(currentDts) );
					COMMON_STR_DEBUG( DEBUG_MAIN "autumn dts " DTS_PATTERN , DTS_GET_BY_VAL(autumnDts) );
					COMMON_STR_DEBUG( DEBUG_MAIN "spring dts " DTS_PATTERN , DTS_GET_BY_VAL(springDts) );
					
					COMMON_STR_DEBUG( DEBUG_MAIN "need to change season to summer\n");

					//protect changes from other places
					alreadyInSyncRoutine = TRUE;

					//pause pooler
					POOL_Pause();

					COMMON_GetCurrentDts( &currentDts ); //getting current time again because pausing pooler takes some time
					COMMON_ChangeDts(&currentDts, INC, BY_HOUR);

					//set new time
					COMMON_SetTime(&currentDts, NULL , DESC_EVENT_TIME_SYNCED_BY_SEASON);

					//resume poller
					POOL_Continue();

					//unprotect changes from other places
					alreadyInSyncRoutine = FALSE;

					STORAGE_UpdateCurrentSeason( STORAGE_SEASON_SUMMER );
				}
			}
			else
			{
				//winter
				autumnDts.t.h = 3 ; //need change season at 3 o`clock, not at 2

				if( (savedSeason != STORAGE_SEASON_WINTER) && (COMMON_DtsIsLower( &autumnDts , &currentDts ) == TRUE))
				{
					COMMON_STR_DEBUG( DEBUG_MAIN "current dts " DTS_PATTERN , DTS_GET_BY_VAL(currentDts) );
                                        COMMON_STR_DEBUG( DEBUG_MAIN "autumn dts " DTS_PATTERN , DTS_GET_BY_VAL(autumnDts) );
                                        COMMON_STR_DEBUG( DEBUG_MAIN "spring dts " DTS_PATTERN , DTS_GET_BY_VAL(springDts) );\

					COMMON_STR_DEBUG( DEBUG_MAIN "need to change season to winter\n");

					//protect changes from other places
					alreadyInSyncRoutine = TRUE;

					//pause pooler
					POOL_Pause();

					COMMON_GetCurrentDts( &currentDts ); //getting current time again because pausing pooler takes some time
					COMMON_ChangeDts(&currentDts, DEC, BY_HOUR);

					//set new time
					COMMON_SetTime(&currentDts, NULL , DESC_EVENT_TIME_SYNCED_BY_SEASON);

					//resume poller
					POOL_Continue();

					//unprotect changes from other places
					alreadyInSyncRoutine = FALSE;

					STORAGE_UpdateCurrentSeason( STORAGE_SEASON_WINTER );
				}
			}
		}

		// do uspd task
		#ifdef NO_CONNECTION
		usleep(100);
		#else
		MICRON_V1();
		usleep(10);
		#endif

	} while(1);


waitBeforeExit:
	//sleep on 15 seconds before exit
	sleep(15);

	return -1;
}

//EOF

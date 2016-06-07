/*
	application code v.1
	for uspd micron 2.0x

	2012 - January
	NPO Frunze
	RUSSIA NIGNY NOVGOROD

	OSIPOV V, SHASHUNKIN D, OSKOLKOVA O, UHOV E
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "common.h"
#include "storage.h"
#include "debug.h"
#include "gubanov.h"
#include "acounts.h"


#define COUNTER_TASK_SYNC_TIME			1
#define COUNTER_TASK_WRITE_TARIFF_MAP	1
#define COUNTER_TASK_WRITE_PSM			1
#define COUNTER_TASK_READ_DTS			1
#define COUNTER_TASK_READ_PROFILES		1
#define COUNTER_TASK_READ_CUR_METERAGES	1
#define COUNTER_TASK_READ_DAY_METERAGES	1
#define COUNTER_TASK_READ_MON_METERAGES	1
#define COUNTER_TASK_READ_JOURNALS		1
#define COUNTER_TASK_READ_PQP			1

//local variables
const unsigned char currentTarifs[] = {'-', 'E','W','V','U'}; //start from 1, 0 - is unused
const unsigned char monthTarifs[] = {'-', 'B','F','L','P'}; //start from 1, 0 - is unused
//
//
//
void GUBANOV_GetCRC(unsigned char * cmd, unsigned int size, unsigned char * resultCrc)
{
	DEB_TRACE(DEB_IDX_GUBANOV);
	const unsigned char hexValues[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	unsigned int crc = 0;
	unsigned int byteIndex = 0;

	for(; byteIndex < size; byteIndex++)
		crc = crc + cmd[byteIndex];

	crc = crc % 256;

	resultCrc[0] = hexValues[(crc >> 4) & 0x0f];
	resultCrc[1] = hexValues[crc & 0x0f];
}

//
//
//

int GUBANOV_GetProfileIndex( dateTimeStamp * dtsToRequest )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	int profileIndex = 1 ;
	if ( !(dtsToRequest->d.m % 2) )
	{
		profileIndex = 1489 ;
	}
	profileIndex += ((dtsToRequest->d.d - 1) * 48) + (dtsToRequest->t.h * 2) ;
	if( dtsToRequest->t.m >= 30 )
	{
		profileIndex += 1 ;
	}
	return profileIndex ;
}

//
//
//
void GUBANOV_GetCounterAddress(counter_t * counter, unsigned char * counterAddress)
{
	DEB_TRACE(DEB_IDX_GUBANOV)

	unsigned int shortAddress = counter->serial % 1000;

	sprintf((char *)counterAddress, "%03d", shortAddress);
}

//
//
//

unsigned char GUBANOV_GetStartCommandSymbol( counter_t * counter __attribute__((unused)), BOOL broadcastCommand )
{
	if ( broadcastCommand == TRUE )
	{
		return GUBANOV_GROUP_MARKER_ADDRESS;
	}
	else
	{
		return GUBANOV_START_MARKER_ADDRESS;

		//TODO - some types have start symbol not a '#'

//		switch( counter->type )
//		{
//			default:
//			case TYPE_GUBANOV_PSH_3TA07_DOT_1:
//			case TYPE_GUBANOV_PSH_3TA07_DOT_2:
//			case TYPE_GUBANOV_PSH_3TA07A:
//			case TYPE_GUBANOV_SEB_2A05:
//			case TYPE_GUBANOV_SEB_2A07:
//			case TYPE_GUBANOV_SEB_2A08:

//			case TYPE_GUBANOV_PSH_3ART:
//			case TYPE_GUBANOV_PSH_3ART_1:
//			case TYPE_GUBANOV_PSH_3ART_D:
//			{
//				return '#';
//			}
//			break;
//		}
	}
}

//
//
//

int GUBANOV_Command_GetCurrentDateAndTime(counter_t * counter, counterTransaction_t ** transaction)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetCurrentDateAndTime start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'D' ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetCurrentDateAndTime: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_DATE_TIME_STAMP;
	(*transaction)->waitSize = GUBANOV_ANSWER_LENGTH_DATE_TIME_STAMP;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (DateTimeStamp) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_CurrentMeterage(counter_t * counter, counterTransaction_t ** transaction, unsigned char tarifIndex)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_CurrentMeterage start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	//allocate transaction and set command data and sizes
	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D :
		{
			//
			// TODO
			//

		}
		break;

		case TYPE_GUBANOV_PSH_3ART:
		{
			//
			// TODO
			//
		}
		break;

		case TYPE_GUBANOV_MAYK_301:
		{
			//
			// TODO
			//
		}
		break;

		default:
		{
			tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

			//address
			GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
			commandSize += GUBANOV_SIZE_OF_ADDRESS ;

			//password
			memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
			commandSize += GUBANOV_SIZE_OF_PASSWORD ;

			//set command
			tempCommand[commandSize++] = currentTarifs[tarifIndex];

			//crc
			GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
			commandSize += GUBANOV_SIZE_OF_CRC ;

			tempCommand[ commandSize++ ] = '\r' ;

			COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_CurrentMeterage: " , tempCommand , commandSize );

			(*transaction)->shortAnswerIsPossible = FALSE ;
			(*transaction)->shortAnswerFlag = FALSE ;
			(*transaction)->waitSize = GUBANOV_ANSWER_LENGTH_CURRENT_SEB_2A;
			(*transaction)->transactionType = TRANSACTION_METERAGE_CURRENT + tarifIndex;
			(*transaction)->answerSize = 0;
			(*transaction)->commandSize = commandSize;
			(*transaction)->answer = NULL;
			(*transaction)->result = (unsigned char)0;
			(*transaction)->command = (unsigned char *)malloc(commandSize);
			if ((*transaction)->command != NULL)
			{
				memcpy((*transaction)->command, tempCommand, commandSize);
				res = OK;
			}
			else
				res = ERROR_MEM_ERROR;
		}
		break;
	}
	return res;
}

//
//
//

int GUBANOV_Command_DayMeterage(counter_t * counter, counterTransaction_t ** transaction, unsigned char tarifIndex , int dayDiff)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_DayMeterage start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;
	int waitSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	//address
	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	//password
	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	BOOL shortAnswerIsPossible = FALSE ;

	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D :
		{
			//
			// TODO
			//

		}
		break;

		case TYPE_GUBANOV_PSH_3ART:
		{
			//
			// TODO
			//
		}
		break;

		case TYPE_GUBANOV_MAYK_301:
		{
			//
			// TODO
			//
		}
		break;

		case TYPE_GUBANOV_MAYK_102:
		{
			//set command
			tempCommand[commandSize++] = '3';

			COMMON_DecToAscii( dayDiff , 2 , &tempCommand[ commandSize ] );
			commandSize += 2 ;

			//
			//osipov: need to use 0,1,2,3 for T1,T2,T3,T4 not 1,2,3,4 indexes
			//
			COMMON_DecToAscii( tarifIndex-1 , 1 , &tempCommand[ commandSize ] );
			commandSize += 1 ;

			waitSize = GUBANOV_ANSWER_LENGTH_DAY_MAYK_102;
			shortAnswerIsPossible = TRUE ;
		}
		break;

		default:
			return ERROR_GENERAL ;
			break;
	}

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_DayMeterage: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = shortAnswerIsPossible ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->waitSize = waitSize ;
	(*transaction)->transactionType = TRANSACTION_METERAGE_DAY + tarifIndex;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char)0;
	(*transaction)->command = (unsigned char *)malloc(commandSize);
	if ((*transaction)->command != NULL)
	{
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	}
	else
		res = ERROR_MEM_ERROR;

	return res;
}

//
//
//

int GUBANOV_Command_MonthMeterage(counter_t * counter, counterTransaction_t ** transaction , int monthIndex , unsigned char tarifIndex)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_MonthMeterage start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		case TYPE_GUBANOV_MAYK_301:

			//
			// TODO
			//

			break;

		case TYPE_GUBANOV_PSH_3TA07_DOT_1:
		case TYPE_GUBANOV_PSH_3TA07_DOT_2:
			tempCommand[ commandSize++ ] = '[' ;
			tempCommand[ commandSize++ ] = monthTarifs[ tarifIndex ] ;

			COMMON_DecToAscii( monthIndex , 2 , &tempCommand[ commandSize ] );
			commandSize += 2 ;

			(*transaction)->waitSize = GUBANOV_ANSWER_LONG_LENGTH_MONTH;
			break;

		default:
			tempCommand[ commandSize++ ] = '[' ;
			tempCommand[ commandSize++ ] = monthTarifs[ tarifIndex ] ;

			COMMON_DecToAscii( monthIndex , 2 , &tempCommand[ commandSize ] );
			commandSize += 2 ;

			(*transaction)->waitSize = GUBANOV_ANSWER_SHORT_LENGTH_MONTH;
			break;
	}

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_MonthMeterage: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_METERAGE_MONTH + tarifIndex;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetMonth) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_GetProfile(counter_t * counter, counterTransaction_t ** transaction , dateTimeStamp * dtsToRequest)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetProfile start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		case TYPE_GUBANOV_MAYK_301:
			tempCommand[ commandSize++ ] = '1' ;
			tempCommand[ commandSize++ ] = 'E' ;
			(*transaction)->waitSize = GUBANOV_ANSWER_ART_LENGTH_PROFILE;
			break;

		default:
			tempCommand[ commandSize++ ] = '?' ;
			(*transaction)->waitSize = GUBANOV_ANSWER_LENGTH_PROFILE;
			break;
	}


	int profileIndex = GUBANOV_GetProfileIndex(dtsToRequest) ;
	memcpy( &(*counter).profileLastRequestDts , dtsToRequest , sizeof(dateTimeStamp) );
	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Searching for profile index [%d]" , profileIndex);

	COMMON_DecToAscii( profileIndex , 4 , &tempCommand[ commandSize ] );
	commandSize += 4 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetProfile: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_PROFILE_GET_VALUE;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetProfile) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_GetPqp_ActivePower(counter_t * counter, counterTransaction_t ** transaction )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetPqp_ActivePower start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '=' ;
	tempCommand[ commandSize++ ] = 'M' ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetPqp_ActivePower: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GET_PQP_POWER_P_PHASE1;
	(*transaction)->waitSize = GUBANOV_ANSWER_ACTIVE_POWER;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetActivePower) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_GetSimpleJournal_BoxOpen(counter_t * counter, counterTransaction_t ** transaction )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournalBoxOpen start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '\\' ;
	tempCommand[ commandSize++ ] = 'T' ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournalBoxOpen: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GET_EVENT;
	(*transaction)->waitSize = GUBANOV_ANSWER_SIMPLE_JOURNAL_BOX_OPENED;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetEvent: Box open) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_GetSimpleJournal_PowerSwitchOn(counter_t * counter, counterTransaction_t ** transaction )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournal_PowerSwitchOn start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '\\' ;
	tempCommand[ commandSize++ ] = 'P' ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournal_PowerSwitchOn: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GET_EVENT;
	(*transaction)->waitSize = GUBANOV_ANSWER_SIMPLE_JOURNAL_POWER_SWITCH_ON;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetEvent: power On) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_GetSimpleJournal_PowerSwitchOff(counter_t * counter, counterTransaction_t ** transaction )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournal_PowerSwitchOff start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '\\' ;
	tempCommand[ commandSize++ ] = 'F' ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournal_PowerSwitchOff: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GET_EVENT;
	(*transaction)->waitSize = GUBANOV_ANSWER_SIMPLE_JOURNAL_POWER_SWITCH_OFF;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetEvent: power Off) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_GetSimpleJournal_TimeSynced(counter_t * counter, counterTransaction_t ** transaction )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournal_TimeSynced start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '\\' ;
	tempCommand[ commandSize++ ] = 'C' ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_GetSimpleJournal_TimeSynced: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GET_EVENT;
	(*transaction)->waitSize = GUBANOV_ANSWER_SIMPLE_JOURNAL_TIME_SINCED;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (GetEvent: time synced) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetTime(counter_t * counter, counterTransaction_t ** transaction , timeStamp * ts )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTime start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , TRUE );

	memcpy( &tempCommand[ commandSize ] , counter->password2 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'C' ;

	//timestamp
	COMMON_DecToAscii( ts->h , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	COMMON_DecToAscii( ts->m , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	COMMON_DecToAscii( ts->s , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTime: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_TIME;
	(*transaction)->waitSize = GUBANOV_ANSWER_GROUP_CMD;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set time) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetDate(counter_t * counter, counterTransaction_t ** transaction , dateStamp * ds )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetDate start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , TRUE );

	memcpy( &tempCommand[ commandSize ] , counter->password2 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'D' ;

	//get week day by dateStamp
	dateTimeStamp dts;
	memset(&dts , 0 , sizeof(dateTimeStamp) );
	memcpy(&dts.d , ds , sizeof(dateStamp) );
	int weekDay = COMMON_GetWeekDayByDts( &dts );
	if ( weekDay == 7 ){
		weekDay = 0 ;
	}
	weekDay += 0x30 ;
	tempCommand[ commandSize++ ] = (unsigned char)weekDay ;

	//datestamp
	COMMON_DecToAscii( ds->d , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	COMMON_DecToAscii( ds->m , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	COMMON_DecToAscii( ds->y , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetDate: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_DATE;
	(*transaction)->waitSize = GUBANOV_ANSWER_GROUP_CMD;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set date) %s" , getErrorCode(res) );
	return res ;
}

//
//
//
#if 0
int GUBANOV_Command_SoftSyncTime(counter_t * counter, counterTransaction_t ** transaction , int secondsToCorrect )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SoftSyncTime start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '0' ;
	tempCommand[ commandSize++ ] = '0' ;

	if ( secondsToCorrect >= 0 ){
		tempCommand[ commandSize++ ] = '+' ;
	}
	else{
		tempCommand[ commandSize++ ] = '-' ;
	}

	COMMON_DecToAscii( abs( secondsToCorrect ) , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SoftSyncTime: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_SYNC_SOFT;
	(*transaction)->waitSize = GUBANOV_ANSWER_SOFT_SYNC_TIME;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (soft sync) %s" , getErrorCode(res) );
	return res ;
}
#endif
//
//
//

int GUBANOV_Command_PowerControl(counter_t * counter, counterTransaction_t ** transaction , BOOL releState )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_PowerControl start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'G' ;

	if ( releState == TRUE ){
		tempCommand[ commandSize++ ] = 'C' ;
	}
	else{
		tempCommand[ commandSize++ ] = 'F' ;
	}

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_PowerControl: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_RELE_CONTROL;
	(*transaction)->waitSize = GUBANOV_ANSWER_RELE_CONTROL;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (power control) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetPowerLimit(counter_t * counter, counterTransaction_t ** transaction , int limit )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetPowerLimit start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , TRUE );

	memcpy( &tempCommand[ commandSize ] , counter->password2 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'L' ;

	//consumer category
	COMMON_DecToAscii( GUBANOV_CONSUMER_CATERORY , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		case TYPE_GUBANOV_MAYK_301:
			COMMON_DecToAscii( limit , 4 , &tempCommand[ commandSize ] );
			commandSize += 4 ;
			break;

		default:
			COMMON_DecToAscii( limit , 3 , &tempCommand[ commandSize ] );
			commandSize += 3 ;
			break;
	}

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetPowerLimit: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_POWER_LIMIT;
	(*transaction)->waitSize = GUBANOV_ANSWER_GROUP_CMD;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set power limit) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetConsumerCategory(counter_t * counter, counterTransaction_t ** transaction )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetConsumerCategory start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'K' ;

	//consumer category
	COMMON_DecToAscii( GUBANOV_CONSUMER_CATERORY , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetConsumerCategory: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_CONSUMER_CATEGORY;
	(*transaction)->waitSize = GUBANOV_ANSWER_SET_CONSUMER_CATEGORY;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set consumer category) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetMonthLimit(counter_t * counter, counterTransaction_t ** transaction , int limit)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetMonthLimit start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = 'J' ;

	//consumer category
	COMMON_DecToAscii( GUBANOV_CONSUMER_CATERORY , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//month limit
	COMMON_DecToAscii( limit , 4 , &tempCommand[ commandSize ] );
	commandSize += 4 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetMonthLimit: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_MONTH_LIMIT;
	(*transaction)->waitSize = GUBANOV_ANSWER_SET_MONTH_LIMIT;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set month limit) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetTariffDayTemplate(counter_t * counter, counterTransaction_t ** transaction , int dayTemplate )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariffDayTemplate start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '4' ;
	tempCommand[ commandSize++ ] = '2' ;

	//day template
	COMMON_DecToAscii( dayTemplate , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariffDayTemplate: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_DAY_TEMPLATE;
	(*transaction)->waitSize = GUBANOV_ANSWER_SET_DAY_TEMPLATE;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set day template) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetTariff_1_StartTime(counter_t * counter, counterTransaction_t ** transaction , gubanovTariff_t * gubanovTariff )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariff_1_StartTime start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '0' ;
	tempCommand[ commandSize++ ] = '1' ;

	//consumer category
	COMMON_DecToAscii( GUBANOV_CONSUMER_CATERORY , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//day of week
	COMMON_DecToAscii( gubanovTariff->dd , 1 , &tempCommand[ commandSize ] );
	commandSize += 1 ;

	//hour
	COMMON_DecToAscii( gubanovTariff->hh , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//minute
	COMMON_DecToAscii( gubanovTariff->mm , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//month
	COMMON_DecToAscii( 1 , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariff_1_StartTime: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_1_TARIFF_START_TIME;
	(*transaction)->waitSize = GUBANOV_ANSWER_SET_1_TARIFF_START_TIME;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set 1 tariff start time) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetTariff_2_StartTime(counter_t * counter, counterTransaction_t ** transaction , gubanovTariff_t * gubanovTariff )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariff_2_StartTime start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '0' ;
	tempCommand[ commandSize++ ] = '2' ;

	//consumer category
	COMMON_DecToAscii( GUBANOV_CONSUMER_CATERORY , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//day of week
	COMMON_DecToAscii( gubanovTariff->dd , 1 , &tempCommand[ commandSize ] );
	commandSize += 1 ;

	//hour
	COMMON_DecToAscii( gubanovTariff->hh , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//minute
	COMMON_DecToAscii( gubanovTariff->mm , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//month
	COMMON_DecToAscii( 1 , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariff_2_StartTime: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_2_TARIFF_START_TIME;
	(*transaction)->waitSize = GUBANOV_ANSWER_SET_2_TARIFF_START_TIME;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set 2 tariff start time) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetTariff_3_StartTime(counter_t * counter, counterTransaction_t ** transaction , gubanovTariff_t * gubanovTariff )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariff_3_StartTime start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '0' ;
	tempCommand[ commandSize++ ] = '3' ;

	//consumer category
	COMMON_DecToAscii( GUBANOV_CONSUMER_CATERORY , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//day of week
	COMMON_DecToAscii( gubanovTariff->dd , 1 , &tempCommand[ commandSize ] );
	commandSize += 1 ;

	//time zone
	COMMON_DecToAscii( gubanovTariff->timeZone , 1 , &tempCommand[ commandSize ] );
	commandSize += 1 ;

	//hour
	COMMON_DecToAscii( gubanovTariff->hh , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//minute
	COMMON_DecToAscii( gubanovTariff->mm , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//duration
	COMMON_DecToAscii( gubanovTariff->duration , 3 , &tempCommand[ commandSize ] );
	commandSize += 3 ;

	//real tariff		
	switch( gubanovTariff->realTariffIdx )
	{
		default:
		case 1:
			tempCommand[ commandSize++ ] = 'B' ;
			break;

		case 2:
			tempCommand[ commandSize++ ] = 'F' ;
			break;

		case 3:
			tempCommand[ commandSize++ ] = 'L' ;
			break;

		case 4:
			tempCommand[ commandSize++ ] = 'M' ;
			break;
	}

	//month
	COMMON_DecToAscii( 1 , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariff_3_StartTime: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_3_TARIFF_START_TIME;
	(*transaction)->waitSize = GUBANOV_ANSWER_SET_3_TARIFF_START_TIME;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set 3 tariff start time) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetTariffHoliday(counter_t * counter, counterTransaction_t ** transaction , gubanovHoliday_t * gubanovHoliday )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariffHoliday start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;


	//command
	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		case TYPE_GUBANOV_MAYK_301:
			tempCommand[ commandSize++ ] = '1' ;
			tempCommand[ commandSize++ ] = '4' ;
			(*transaction)->waitSize = GUBANOV_ANSWER_LONG_SET_HOLIDAY;
			break;

		default:
			tempCommand[ commandSize++ ] = ']' ;
			tempCommand[ commandSize++ ] = 'W' ;
			(*transaction)->waitSize = GUBANOV_ANSWER_SHORT_SET_HOLIDAY;
			break;
	}

	//holiday index
	COMMON_DecToAscii( gubanovHoliday->index , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//TODO add switch-case depends

	switch ( counter->type )
	{
		default:
		case TYPE_GUBANOV_SEB_2A05:
		case TYPE_GUBANOV_SEB_2A07:
		case TYPE_GUBANOV_MAYK_101:
		case TYPE_GUBANOV_MAYK_102:
		{
			//dec
			//day
			snprintf((char *)&tempCommand[ commandSize ] , 3 , "%02d" , gubanovHoliday->dd);
			commandSize += 2 ;

			//month
			snprintf((char *)&tempCommand[ commandSize ] , 3 , "%02d" , gubanovHoliday->MM);
			commandSize += 2 ;
		}
		break;

		case TYPE_GUBANOV_SEB_2A08:
		case TYPE_GUBANOV_PSH_3TA07_DOT_1 :
		case TYPE_GUBANOV_PSH_3TA07_DOT_2:
		case TYPE_GUBANOV_PSH_3TA07A:
		case TYPE_GUBANOV_MAYK_301:
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		{
			//hex
			//day
			snprintf((char *)&tempCommand[ commandSize ] , 3 , "%02X" , gubanovHoliday->dd);
			commandSize += 2 ;

			//month
			snprintf((char *)&tempCommand[ commandSize ] , 3 , "%02X" , gubanovHoliday->MM);
			commandSize += 2 ;
		}
		break;
	}


	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		case TYPE_GUBANOV_MAYK_301:
			//type
			COMMON_DecToAscii( gubanovHoliday->type , 2 , &tempCommand[ commandSize ] );
			commandSize += 2 ;
		break;

		default:
			break;
	}

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetTariffHoliday: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_GUBANOV_SET_HOLIDAY;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set holiday) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetIndicationSimple(counter_t * counter, counterTransaction_t ** transaction , gubanovIndication_t * gubanovIndication )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetIndicationSimple start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '^' ;
	tempCommand[ commandSize++ ] = 'W' ;

	//T1
	if ( gubanovIndication->T2 == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//odd parameter
	tempCommand[ commandSize++ ] = 'N' ;

	//T2
	if ( gubanovIndication->T3 == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//odd parameter
	tempCommand[ commandSize++ ] = 'N' ;

	//T3
	if ( gubanovIndication->T1 == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//odd parameter
	tempCommand[ commandSize++ ] = 'N' ;

	//T4
	if ( gubanovIndication->T4 == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//odd parameter
	tempCommand[ commandSize++ ] = 'N' ;

	//TS
	if ( gubanovIndication->TS == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//date
	if ( gubanovIndication->date == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//time
	if ( gubanovIndication->time == TRUE )
	{
		tempCommand[ commandSize++ ] = 'Y' ;
	}
	else
	{
		tempCommand[ commandSize++ ] = 'N' ;
	}

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetIndicationSimple: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_WRITING_INDICATIONS;
	(*transaction)->waitSize = GUBANOV_ANSWER_INDICATIONS_SIMPLE;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set simple indication) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetIndicationImproved(counter_t * counter, counterTransaction_t ** transaction , int loopIndex , unsigned int loopMask )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetIndicationSimple start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '1' ;
	tempCommand[ commandSize++ ] = '1' ;

	//loop index
	COMMON_DecToAscii( loopIndex , 1 , &tempCommand[ commandSize ] );
	commandSize += 1 ;

	//loop mask
	COMMON_DecToAscii( loopMask , 4 , &tempCommand[ commandSize ] );
	commandSize += 4 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetIndicationImproved: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_WRITING_INDICATIONS;
	(*transaction)->waitSize = GUBANOV_ANSWER_INDICATIONS_IMPROVED;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set improve indication) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

int GUBANOV_Command_SetIndicationDuration(counter_t * counter, counterTransaction_t ** transaction , int duration )
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetIndicationDuration start");

	int res = ERROR_GENERAL ;

	unsigned char tempCommand[64];
	memset( tempCommand , 0 , 64 );
	int commandSize = 0 ;

	tempCommand[ commandSize++ ] = GUBANOV_GetStartCommandSymbol( counter , FALSE );

	GUBANOV_GetCounterAddress( counter , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_ADDRESS ;

	memcpy( &tempCommand[ commandSize ] , counter->password1 , GUBANOV_SIZE_OF_PASSWORD ) ;
	commandSize += GUBANOV_SIZE_OF_PASSWORD ;

	//command
	tempCommand[ commandSize++ ] = '^' ;
	tempCommand[ commandSize++ ] = 'T' ;

	//duration
	COMMON_DecToAscii( duration , 2 , &tempCommand[ commandSize ] );
	commandSize += 2 ;

	//crc
	GUBANOV_GetCRC( tempCommand , commandSize , &tempCommand[ commandSize ] );
	commandSize += GUBANOV_SIZE_OF_CRC ;

	tempCommand[ commandSize++ ] = '\r' ;

	COMMON_ASCII_DEBUG( DEBUG_GUBANOV "GUBANOV_Command_SetIndicationDuration: " , tempCommand , commandSize );

	//allocate transaction and set command data and sizes
	(*transaction)->shortAnswerIsPossible = FALSE ;
	(*transaction)->shortAnswerFlag = FALSE ;
	(*transaction)->transactionType = TRANSACTION_WRITING_LOOP_DURATION;
	(*transaction)->waitSize = GUBANOV_ANSWER_LOOP_DURATION;
	(*transaction)->answerSize = 0;
	(*transaction)->commandSize = commandSize;
	(*transaction)->answer = NULL;
	(*transaction)->result = (unsigned char) 0;
	(*transaction)->command = (unsigned char *) malloc(commandSize);
	if ((*transaction)->command != NULL) {
		memcpy((*transaction)->command, tempCommand, commandSize);
		res = OK;
	} else
		res = ERROR_MEM_ERROR;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_CreateTask (set loop duration) %s" , getErrorCode(res) );
	return res ;
}

//
//
//

void GUBANOV_SetNextJournalNumber( counter_t * counter )
{
	if( !counter )
		return ;

	switch( counter->type )
	{
		case TYPE_GUBANOV_PSH_3ART:
		case TYPE_GUBANOV_PSH_3ART_1:
		case TYPE_GUBANOV_PSH_3ART_D:
		case TYPE_GUBANOV_MAYK_301:
		{
			//use improved journals

			//
			// TODO
			//
		}
		break;

		default:
		{
			switch( counter->journalNumber )
			{
				case GUBANOV_SIMPLE_JOURNAL_EVENT_OPEN:
					counter->journalNumber = GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_ON ;
					break;

				case GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_ON:
					counter->journalNumber = GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_OFF ;
					break;

				case GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_OFF:
					counter->journalNumber = GUBANOV_SIMPLE_JOURNAL_EVENT_TIME_SYNC ;
					break;

				default:
				case GUBANOV_SIMPLE_JOURNAL_EVENT_TIME_SYNC:
					counter->journalNumber = GUBANOV_SIMPLE_JOURNAL_EVENT_OPEN ;
					break;
			}
		}
		break;
	}

	return ;
}

//
//
//

int GUBANOV_GetSimpleJournal(counter_t * counter, counterTask_t ** counterTask)
{
	int res = ERROR_GENERAL ;

	if( !counter )
		return res ;

	switch( counter->journalNumber )
	{
		case GUBANOV_SIMPLE_JOURNAL_EVENT_OPEN:
		{
			(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
			if ((*counterTask)->transactions != NULL) {
				(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
				if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
					res = GUBANOV_Command_GetSimpleJournal_BoxOpen(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
				else
					return ERROR_MEM_ERROR;
			} else
				return ERROR_MEM_ERROR;
		}
		break;

		case GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_ON:
		{
			(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
			if ((*counterTask)->transactions != NULL) {
				(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
				if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
					res = GUBANOV_Command_GetSimpleJournal_PowerSwitchOn(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
				else
					return ERROR_MEM_ERROR;
			} else
				return ERROR_MEM_ERROR;
		}
		break;

		case GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_OFF:
		{
			(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
			if ((*counterTask)->transactions != NULL) {
				(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
				if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
					res = GUBANOV_Command_GetSimpleJournal_PowerSwitchOff(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
				else
					return ERROR_MEM_ERROR;
			} else
				return ERROR_MEM_ERROR;
		}
		break;

		case GUBANOV_SIMPLE_JOURNAL_EVENT_TIME_SYNC:
		{
			(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
			if ((*counterTask)->transactions != NULL) {
				(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
				if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
					res = GUBANOV_Command_GetSimpleJournal_TimeSynced(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
				else
					return ERROR_MEM_ERROR;
			} else
				return ERROR_MEM_ERROR;
		}
		break;

		default:
		{
			GUBANOV_SetNextJournalNumber( counter );
			res = OK ;
		}
		break;
	}

	return res ;
}

//
//
//

int GUBANOV_GetImprovedJournal(counter_t * counter __attribute__((unused)), counterTask_t ** counterTask __attribute__((unused)))
{
	int res = ERROR_GENERAL ;

	//TODO

	return res ;
}

//
//
//

int GUBANOV_SyncTime(counter_t * counter, counterTask_t ** counterTask)
{
	int res = OK ;

	dateTimeStamp dtsToSet;
	memset(&dtsToSet, 0 , sizeof(dateTimeStamp));
	int i = 0;
	for( ; i < ( counter->transactionTime / 2 ) ; ++i ){
		COMMON_ChangeDts( &dtsToSet , INC , BY_SEC );
	}
	COMMON_GetCurrentDts(&dtsToSet);

	unsigned int syncState = counter->syncFlag & 0xC0 ;

	switch(syncState)
	{
		case 0:
		{
			//check if we can do soft time correction

			if ( (abs(counter->syncWorth) < GUBANOV_SOFT_TIME_CORRECTION_DIFF) && (
					(counter->type == TYPE_GUBANOV_PSH_3ART) ||
					(counter->type == TYPE_GUBANOV_PSH_3ART_1) ||
					(counter->type == TYPE_GUBANOV_PSH_3ART_D) ||
					(counter->type == TYPE_GUBANOV_MAYK_102) ||
					(counter->type == TYPE_GUBANOV_MAYK_301) )
				)
			{
				//do not use soft time correction because of gubanov-shagin firmware error
				#if 0
				(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
				if ((*counterTask)->transactions != NULL) {
					(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
					if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
						res = GUBANOV_Command_SoftSyncTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , ((-1) * (counter->syncWorth)) );
					else
						return ERROR_MEM_ERROR;
				} else
					return ERROR_MEM_ERROR;
				#else

				STORAGE_SwitchSyncFlagTo_SyncDone(&counter);
				return ERROR_SYNC_IS_IMPOSSIBLE ;

				#endif

				break;
			}
			else
			{
				if ( (counter->currentDts.d.d != dtsToSet.d.d) ||
					 (counter->currentDts.d.m != dtsToSet.d.m) ||
					 (counter->currentDts.d.y != dtsToSet.d.y) )
				{
					//set date stamp

					(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
					if ((*counterTask)->transactions != NULL) {
						(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
						if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
						{
							STORAGE_SwitchSyncFlagTo_SyncProcess(&counter);
							res = GUBANOV_Command_SetDate(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &dtsToSet.d);
						}
						else
							return ERROR_MEM_ERROR;
					} else
						return ERROR_MEM_ERROR;

					break ;
				}
				else
				{
					STORAGE_SwitchSyncFlagTo_SyncProcess(&counter);
					//step to the next case to set time stamp only
					counter->syncFlag |= STORAGE_SYNC_DATE_SET ;
				}
			}
		}


		case STORAGE_SYNC_DATE_SET:
		{
			//date stamp is OK. Need to set time stamp

			(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
			if ((*counterTask)->transactions != NULL) {
				(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
				if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
					res = GUBANOV_Command_SetTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &dtsToSet.t);
				else
					return ERROR_MEM_ERROR;
			} else
				return ERROR_MEM_ERROR;
		}
		break;

		case ( STORAGE_SYNC_DATE_SET + STORAGE_SYNC_TIME_SET ):
		{
			//Need to check dateTime stamp
			(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
			if ((*counterTask)->transactions != NULL) {
				(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
				if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
					res = GUBANOV_Command_GetCurrentDateAndTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
				else
					return ERROR_MEM_ERROR;
			} else
				return ERROR_MEM_ERROR;
		}
		break;

		default:
			break;
	}

	return res ;
}

//
//
//

int GUBANOV_WritePSM(counter_t * counter, counterTask_t ** counterTask ,unsigned char * psmRules, int psmRulesLength)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	int res = ERROR_COUNTER_TYPE ;

	enum
	{
		PARSING_COUNTER_TYPE ,
		PARSING_INSTANTANTEOUS_CONTROL ,
		PARSING_MONTH_ENERGY_LIMIT ,
		PARSING_POWER_LIMIT ,
		PARSING_STATE_WRITE_LIMITS ,
		PARSING_STATE_DONE
	};

	BOOL switchingRele = FALSE;

	int monthLimit = 0 ;
	int powerLimit = 0 ;

	(*counterTask)->powerControlTransactionCounter = 0 ;

	int parsingState = PARSING_COUNTER_TYPE ;
	while( parsingState != PARSING_STATE_DONE )
	{
		switch( parsingState )
		{
			case PARSING_COUNTER_TYPE :
			{
				COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM parsingState PARSING_COUNTER_TYPE");

				parsingState = PARSING_STATE_DONE;

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)psmRules , psmRulesLength , "counter", &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						int propertyIndex = 0 ;
						for( ; propertyIndex < blockParsingResultLength ; ++propertyIndex )
						{
							if ( strstr( blockParsingResult[ propertyIndex ].variable , "model" ) )
							{
								if( atoi( blockParsingResult[ propertyIndex ].value ) == counter->type )
								{
									parsingState = PARSING_INSTANTANTEOUS_CONTROL ;
									res = OK ;
								}
								break;
							}
						}
						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}

				if ( res != OK )
				{
					COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM counter type is not equal");
					return res ;
				}
			}
			break;

			//=====================

			case PARSING_INSTANTANTEOUS_CONTROL :
			{
				COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM parsingState PARSING_INSTANTANTEOUS_CONTROL");

				parsingState = PARSING_MONTH_ENERGY_LIMIT;

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)psmRules , psmRulesLength , "instantaneous_control", &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						int propertyIndex = 0 ;
						for( ; propertyIndex < blockParsingResultLength ; ++propertyIndex )
						{
							if ( strstr( blockParsingResult[ propertyIndex ].variable , "controlLoadState" ) )
							{
								BOOL releState = FALSE ;
								if ( atoi(blockParsingResult[ propertyIndex ].value ) == 1){
									releState = TRUE ;
								}

								if ( res == OK )
								{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
											res = GUBANOV_Command_PowerControl(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , releState );
											(*counterTask)->powerControlTransactionCounter++ ;
											switchingRele = TRUE;
										}
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
								}
							}
						}

						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}
			}
			break;

			//=====================

			case PARSING_MONTH_ENERGY_LIMIT :
			{
				COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM parsingState PARSING_MONTH_ENERGY_LIMIT");

				parsingState = PARSING_POWER_LIMIT;

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)psmRules , psmRulesLength , "month_energy_limit", &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						int propertyIndex = 0 ;
						for( ; propertyIndex < blockParsingResultLength ; ++propertyIndex )
						{
							if ( strstr( blockParsingResult[ propertyIndex ].variable , "energyLimit" ) )
							{
								monthLimit = atoi( blockParsingResult[ propertyIndex ].value );
							}

						}

						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}
			}
			break;

			//=====================

			case PARSING_POWER_LIMIT :
			{
				parsingState = PARSING_STATE_WRITE_LIMITS;

				COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM parsingState PARSING_POWER_LIMIT");

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)psmRules , psmRulesLength , "power_limit", &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						int propertyIndex = 0 ;
						for( ; propertyIndex < blockParsingResultLength ; ++propertyIndex )
						{
							if ( strstr( blockParsingResult[ propertyIndex ].variable , "powerLimit" ) )
							{
								powerLimit = atoi( blockParsingResult[ propertyIndex ].value );
							}
						}

						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}
			}
			break;

			//=====================

			case PARSING_STATE_WRITE_LIMITS:
			{
				parsingState = PARSING_STATE_DONE;
				COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM parsingState PARSING_STATE_WRITE_LIMITS");

				if ( (powerLimit > 0) || (monthLimit > 0))
				{
					if ( (switchingRele == FALSE) && (res == OK) )
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
								res = GUBANOV_Command_PowerControl(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , TRUE );
								(*counterTask)->powerControlTransactionCounter++ ;
								switchingRele = TRUE;
							}
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}

					if ( res == OK )
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
								res = GUBANOV_Command_SetConsumerCategory(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
								(*counterTask)->powerControlTransactionCounter++ ;
							}
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}

					if ( powerLimit > 0 )
					{
						if ( res == OK )
						{
							(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
							if ((*counterTask)->transactions != NULL) {
								(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
								if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
									res = GUBANOV_Command_SetPowerLimit(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , powerLimit);
									(*counterTask)->powerControlTransactionCounter++ ;
								}
								else
									return ERROR_MEM_ERROR;
							} else
								return ERROR_MEM_ERROR;
						}
					}
					if ( monthLimit > 0 )
					{
						if ( res == OK )
						{
							(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
							if ((*counterTask)->transactions != NULL) {
								(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
								if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
									res = GUBANOV_Command_SetMonthLimit(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , monthLimit);
									(*counterTask)->powerControlTransactionCounter++ ;
								}
								else
									return ERROR_MEM_ERROR;
							} else
								return ERROR_MEM_ERROR;
						}
					}					
				}
			}
			break;

			//=====================

			default:
				COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV_WritePSM unknown parsingState [%d] , parsingState");
				parsingState = PARSING_STATE_DONE ;
				break;
		}
	}

	return res ;
}

//
//
//

int GUBANOV_WriteTariff(counter_t * counter, counterTask_t ** counterTask ,unsigned char * tariff, int tariffLength)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	int res = ERROR_FILE_FORMAT ;

	int tariffMapTemplate = 0 ;
	int blockNumber = 0 ;
	int blockIndex = 1;

	enum
	{
		PARSING_VERSION ,

		PARSING_STATE_VERSION_1 ,

		PARSING_STATE_VERSION_2 ,
		PARSING_BLOCK_BY_INDEX , //template 2.0
		PARSING_HOLIDAYS ,		 //template 2.0

		PARSING_INDICATIONS ,

		PARSING_STATE_DONE
	};

	int parsingState = PARSING_VERSION ;
	while( parsingState != PARSING_STATE_DONE )
	{
		switch( parsingState )
		{
			case PARSING_VERSION :
			{
				parsingState = PARSING_STATE_DONE ;

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)tariff , tariffLength , "CounterType", &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						int propertyIndex = 0 ;
						for( ; propertyIndex < blockParsingResultLength ; ++propertyIndex )
						{
							if ( strstr( blockParsingResult[ propertyIndex ].variable , "Version" ) )
							{
								if ( strstr( blockParsingResult[ propertyIndex ].value , "0.1" ) )
								{
									parsingState = PARSING_STATE_VERSION_1 ;
								}
								else
								{
									parsingState = PARSING_STATE_VERSION_2 ;
								}
							}
							else if ( strstr( blockParsingResult[ propertyIndex ].variable , "Template" ) )
							{
								tariffMapTemplate = atoi( blockParsingResult[ propertyIndex ].value );
							}
							else if ( strstr( blockParsingResult[ propertyIndex ].variable , "BlockCount" ) )
							{
								blockNumber = atoi( blockParsingResult[ propertyIndex ].value );
							}
						}

						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}
			}
			break;

			//========================

			case PARSING_STATE_VERSION_1 :
			{
				res = ERROR_FILE_FORMAT ;
				//
				// TODO
				//
				parsingState = PARSING_STATE_DONE ;
			}
			break;

			//========================

			case PARSING_STATE_VERSION_2 :
			{
				if ( ( (tariffMapTemplate == 1 ) || (tariffMapTemplate==2) ) && (blockNumber > 0) )
				{
					parsingState = PARSING_BLOCK_BY_INDEX ;
					res = OK ;

					(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
					if ((*counterTask)->transactions != NULL) {
						(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
						if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
							res = GUBANOV_Command_SetTariffDayTemplate(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , tariffMapTemplate );
							(*counterTask)->tariffMapTransactionCounter++;
						}
						else
							return ERROR_MEM_ERROR;
					} else
						return ERROR_MEM_ERROR;

					if ( res == OK )
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
								res = GUBANOV_Command_SetConsumerCategory(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
								(*counterTask)->tariffMapTransactionCounter++;
							}
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}
				}
				else
				{
					parsingState = PARSING_STATE_DONE ;
				}
			}
			break;

			//========================

			case PARSING_BLOCK_BY_INDEX :
			{
				if ( (blockIndex <= blockNumber) && (blockIndex >= 0) )
				{
					char blockHead[16];
					memset( blockHead , 0 , 16);
					snprintf( blockHead , 16 , "Block%d" , blockIndex);
					blockIndex++ ;

					blockParsingResult_t * blockParsingResult = NULL ;
					int blockParsingResultLength = 0 ;

					if ( COMMON_SearchBlock( (char *)tariff , tariffLength , blockHead , &blockParsingResult , &blockParsingResultLength ) == OK )
					{
						if ( blockParsingResult )
						{
							int tariffIdx = -1 ;
							int data0Idx = -1 ;
							int data1Idx = -1 ;
							int data2Idx = -1 ;

							int i = 0 ;
							for ( ; i < blockParsingResultLength ; ++i )
							{
								if ( strstr( blockParsingResult[ i ].variable , "Tarif" ) )	{tariffIdx = i ;}
								else if ( strstr( blockParsingResult[ i ].variable , "Data0" ) ){data0Idx = i ;}
								else if ( strstr( blockParsingResult[ i ].variable , "Data1" ) ){data1Idx = i ;}
								else if ( strstr( blockParsingResult[ i ].variable , "Data2" ) ){data2Idx = i ;}
							}

							if ( tariffIdx >= 0 )
							{
								gubanovTariff_t gubanovTariff ;
								memset(&gubanovTariff , 0 , sizeof(gubanovTariff_t) );

								switch( atoi( blockParsingResult[ tariffIdx ].value ) )
								{
									case 1:
									{
										if ( data0Idx >= 0 )
										{
											memset(&gubanovTariff , 0 , sizeof(gubanovTariff_t) );
											sscanf( blockParsingResult[ data0Idx ].value , "%u %u %u" , &gubanovTariff.dd , &gubanovTariff.hh , &gubanovTariff.mm );

											if ( res == OK )
											{
												(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
												if ((*counterTask)->transactions != NULL) {
													(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
													if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
														res = GUBANOV_Command_SetTariff_1_StartTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &gubanovTariff );
														(*counterTask)->tariffMapTransactionCounter++;
													}
													else
														return ERROR_MEM_ERROR;
												} else
													return ERROR_MEM_ERROR;
											}
										}
									}
									break;

									case 2:
									{
										if ( data0Idx >= 0 )
										{
											memset(&gubanovTariff , 0 , sizeof(gubanovTariff_t) );
											sscanf( blockParsingResult[ data0Idx ].value , "%u %u %u" , &gubanovTariff.dd , &gubanovTariff.hh , &gubanovTariff.mm );

											if ( res == OK )
											{
												(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
												if ((*counterTask)->transactions != NULL) {
													(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
													if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
														res = GUBANOV_Command_SetTariff_2_StartTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &gubanovTariff );
														(*counterTask)->tariffMapTransactionCounter++;
													}
													else
														return ERROR_MEM_ERROR;
												} else
													return ERROR_MEM_ERROR;
											}
										}
									}
									break;

									case 3:
									case 4:
									{
										int dataIndex = 0 ;
										for ( ; dataIndex < 3 ; ++dataIndex )
										{
											int needIdx = 0 ;
											switch( dataIndex )
											{
												case 0: needIdx = data0Idx ; break;
												case 1: needIdx = data1Idx ; break;
												case 2: needIdx = data2Idx ; break;
											}

											if ( needIdx >= 0 )
											{
												memset(&gubanovTariff , 0 , sizeof(gubanovTariff_t) );
												sscanf( blockParsingResult[ needIdx ].value , "%u %u %u %u %u %u" , &gubanovTariff.timeZone , &gubanovTariff.dd , &gubanovTariff.hh , &gubanovTariff.mm , &gubanovTariff.duration , &gubanovTariff.realTariffIdx);
												if ( res == OK )
												{
													(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
													if ((*counterTask)->transactions != NULL) {
														(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
														if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
															res = GUBANOV_Command_SetTariff_3_StartTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &gubanovTariff );
															(*counterTask)->tariffMapTransactionCounter++;
														}
														else
															return ERROR_MEM_ERROR;
													} else
														return ERROR_MEM_ERROR;
												}
											}
										}
									}
									break;

									default:
										break;
								}
							}

							free( blockParsingResult );
							blockParsingResult = NULL ;
							blockParsingResultLength = 0 ;
						}
					}
					else
					{
						blockIndex = -1 ;
					}
				}
				else
				{
					parsingState = PARSING_HOLIDAYS ;
				}
			}
			break;

			//========================

			case PARSING_HOLIDAYS :
			{
				parsingState = PARSING_INDICATIONS ;

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)tariff , tariffLength , "Holiday" , &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						char partHead[16];
						memset( partHead , 0 , 16 );

						int holidayBlockIndex = 0 ;
						while(1)
						{
							snprintf( partHead , 16 , "H%d" , holidayBlockIndex);
							++holidayBlockIndex ;

							int dataIndex = -1;
							int i = 0 ;
							for( ; i < blockParsingResultLength ; ++i )
							{
								if ( strstr( blockParsingResult[ i ].variable , partHead ) )
								{
									dataIndex = i ;
									break;
								}
							}

							if ( dataIndex >= 0 )
							{
								gubanovHoliday_t gubanovHoliday;
								memset( &gubanovHoliday , 0 , sizeof(gubanovHoliday_t) );
								gubanovHoliday.index = holidayBlockIndex;

								sscanf(  blockParsingResult[ dataIndex ].value , "%u %u %u" , &gubanovHoliday.dd , &gubanovHoliday.MM , &gubanovHoliday.type) ;

								if ( res == OK )
								{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
											res = GUBANOV_Command_SetTariffHoliday(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &gubanovHoliday );
											(*counterTask)->tariffHolidaysTransactionCounter++;
										}
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
								}
								else
								{
									parsingState = PARSING_STATE_DONE ;
								}
							}
							else
							{
								break;
							}

							if ( holidayBlockIndex > 32 )
							{
								break;
							}
						}

						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}
			}
			break;

			//========================

			case PARSING_INDICATIONS:
			{
				parsingState = PARSING_STATE_DONE ;

				blockParsingResult_t * blockParsingResult = NULL ;
				int blockParsingResultLength = 0 ;

				if ( COMMON_SearchBlock( (char *)tariff , tariffLength , "indication" , &blockParsingResult , &blockParsingResultLength ) == OK )
				{
					if ( blockParsingResult )
					{
						unsigned char counterType = 0 ;

						int blockParsingResultIndex = 0 ;
						for ( ; blockParsingResultIndex < blockParsingResultLength ; ++blockParsingResultIndex )
						{
							if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "CounterType" ) )
							{
								sscanf( blockParsingResult[ blockParsingResultIndex ].value , "%hhu" , &counterType );
								break; //exit from 'for' loop
							}
						}

						if ( counterType == counter->type )
						{
							gubanovIndication_t gubanovIndication ;
							memset( &gubanovIndication , 0 , sizeof(gubanovIndication_t) );

							switch( counterType )
							{
								case TYPE_GUBANOV_PSH_3TA07_DOT_1:
								case TYPE_GUBANOV_PSH_3TA07_DOT_2:
								case TYPE_GUBANOV_PSH_3TA07A:
								case TYPE_GUBANOV_SEB_2A05:
								case TYPE_GUBANOV_SEB_2A07:
								case TYPE_GUBANOV_SEB_2A08:
								case TYPE_GUBANOV_MAYK_101:
								case TYPE_GUBANOV_MAYK_102:
								{
									for ( blockParsingResultIndex = 0 ; blockParsingResultIndex < blockParsingResultLength ; ++blockParsingResultIndex )
									{
										if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "T1" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.T1 = FALSE ;
											}
											else
											{
												gubanovIndication.T1 = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "T2" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.T2 = FALSE ;
											}
											else
											{
												gubanovIndication.T2 = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "T3" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.T3 = FALSE ;
											}
											else
											{
												gubanovIndication.T3 = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "T4" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.T4 = FALSE ;
											}
											else
											{
												gubanovIndication.T4 = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "TS" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.TS = FALSE ;
											}
											else
											{
												gubanovIndication.TS = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Date" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.date = FALSE ;
											}
											else
											{
												gubanovIndication.date = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Time" ) )
										{
											if ( atoi(blockParsingResult[ blockParsingResultIndex ].value) == 0 )
											{
												gubanovIndication.time = FALSE ;
											}
											else
											{
												gubanovIndication.time = TRUE ;
											}
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Duration" ) )
										{
											gubanovIndication.duration = atoi( blockParsingResult[ blockParsingResultIndex ].value ) ;
											if ( gubanovIndication.duration < 6 )
											{
												gubanovIndication.duration = 6 ;
											}
											else if ( gubanovIndication.duration > 60 )
											{
												gubanovIndication.duration = 60 ;
											}
											else
											{
												if ( gubanovIndication.duration % 2 )
												{
													gubanovIndication.duration += 1;
												}
											}
										}
									} //for()

									if ( res == OK )
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationSimple(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &gubanovIndication );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}

									if ( ( res == OK ) && ( gubanovIndication.duration > 0 ))
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationDuration(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , gubanovIndication.duration );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}
								}
								break;

								case TYPE_GUBANOV_PSH_3ART:
								case TYPE_GUBANOV_PSH_3ART_1:
								case TYPE_GUBANOV_PSH_3ART_D:
								case TYPE_GUBANOV_MAYK_301:
								{
									for ( blockParsingResultIndex = 0 ; blockParsingResultIndex < blockParsingResultLength ; ++blockParsingResultIndex )
									{
										if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Loop_0" ) )
										{
											gubanovIndication.loop0 = (unsigned int)atoi( blockParsingResult[ blockParsingResultIndex ].value ) ;
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Loop_1" ) )
										{
											gubanovIndication.loop1 = (unsigned int)atoi( blockParsingResult[ blockParsingResultIndex ].value ) ;
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Loop_3" ) )
										{
											gubanovIndication.loop3 = (unsigned int)atoi( blockParsingResult[ blockParsingResultIndex ].value ) ;
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Loop_4" ) )
										{
											gubanovIndication.loop4 = (unsigned int)atoi( blockParsingResult[ blockParsingResultIndex ].value ) ;
										}
										else if ( !strcmp( blockParsingResult[ blockParsingResultIndex ].variable , "Duration" ) )
										{
											gubanovIndication.duration = atoi( blockParsingResult[ blockParsingResultIndex ].value ) ;
											if ( gubanovIndication.duration < 6 )
											{
												gubanovIndication.duration = 6 ;
											}
											else if ( gubanovIndication.duration > 60 )
											{
												gubanovIndication.duration = 60 ;
											}
											else
											{
												if ( gubanovIndication.duration % 2 )
												{
													gubanovIndication.duration += 1;
												}
											}
										}
									} //for()

									if ( res == OK )
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationImproved(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , 0 , gubanovIndication.loop0 );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}

									if ( res == OK )
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationImproved(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , 1 , gubanovIndication.loop1 );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}

									if ( res == OK )
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationImproved(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , 3 , gubanovIndication.loop3 );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}

									if ( res == OK )
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationImproved(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , 4 , gubanovIndication.loop4 );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}

									if ( ( res == OK ) && ( gubanovIndication.duration > 0 ))
									{
										(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
										if ((*counterTask)->transactions != NULL) {
											(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
											if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL){
												res = GUBANOV_Command_SetIndicationDuration(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , gubanovIndication.duration );
												(*counterTask)->indicationWritingTransactionCounter++;
											}
											else
												return ERROR_MEM_ERROR;
										} else
											return ERROR_MEM_ERROR;
									}
								}
								break;

								default:
								{
									res = ERROR_COUNTER_TYPE ;
								}
								break;
							} // switch()



						}
						else
						{
							if ( counterType == 0 )
							{
								res = ERROR_FILE_FORMAT ;
							}
							else
							{
								res = ERROR_COUNTER_TYPE ;
							}
						}

						free( blockParsingResult );
						blockParsingResult = NULL ;
						blockParsingResultLength = 0 ;
					}
				}
			}
			break;

			default:
			{
				COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_WriteTariff unknown parsing state [%d]" , parsingState  );
				parsingState = PARSING_STATE_DONE ;
			}
			break;
		}
	}

	return res ;
}

//
//
//
int GUBANOV_CreateCounterTask(counter_t * counter, counterTask_t ** counterTask)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	int res = OK;

	//allocate task
	(*counterTask) = (counterTask_t *)malloc (sizeof(counterTask_t));
	if ((*counterTask) == NULL)
		return ERROR_MEM_ERROR;

	//setup transactions pointers
	(*counterTask)->transactionsCount = 0;
	(*counterTask)->transactions = NULL;

	//setup associated serial
	memset((*counterTask)->associatedSerial, 0, SIZE_OF_SERIAL + 1);
	memcpy((*counterTask)->associatedSerial, counter->serialRemote, SIZE_OF_SERIAL + 1);

	//setup dbid and handle and others
	(*counterTask)->counterDbId = counter->dbId;
	(*counterTask)->counterType = counter->type;
	(*counterTask)->associatedId = -1;
	(*counterTask)->currentTransaction = 0;
	(*counterTask)->transactionsCount = 0;

	//free statuses of result
	(*counterTask)->resultCurrentReady = ZERO;
	(*counterTask)->resultDayReady = ZERO;
	(*counterTask)->resultMonthReady = ZERO;
	(*counterTask)->resultProfileReady = ZERO;

	//setup power control transaction counters
	(*counterTask)->powerControlTransactionCounter = 0 ;

	//setup tariff transaction counters
	(*counterTask)->tariffHolidaysTransactionCounter = 0 ;
	(*counterTask)->tariffMapTransactionCounter = 0 ;
	(*counterTask)->tariffMovedDaysTransactionsCounter = 0 ;
	(*counterTask)->indicationWritingTransactionCounter = 0 ;

	//free results
	memset(&(*counterTask)->resultCurrent, 0, sizeof (meterage_t));
	memset(&(*counterTask)->resultDay, 0, sizeof (meterage_t));
	memset(&(*counterTask)->resultMonth, 0, sizeof (meterage_t));
	memset(&(*counterTask)->resultProfile, 0, sizeof (profile_t));

	enum{
		TASK_CREATION_STATE_START,

		TASK_CREATION_STATE_SYNC_TIME,
		TASK_CREATION_STATE_WRITE_TARIFF_MAP,
		TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES,
		TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME,
		TASK_CREATION_STATE_READ_CUR_METERAGES,
		TASK_CREATION_STATE_READ_MON_METERAGES,
		TASK_CREATION_STATE_READ_DAY_METERAGES,
		TASK_CREATION_STATE_READ_PROFILES,
		TASK_CREATION_STATE_READ_JOURNALS,
		TASK_CREATION_STATE_READ_PQP,

		TASK_CREATION_STATE_DONE
	};

	int taskCreationState = TASK_CREATION_STATE_START ;
	while ( taskCreationState != TASK_CREATION_STATE_DONE )
	{
		switch(taskCreationState)
		{
			case TASK_CREATION_STATE_START:
			{
				COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TASK_CREATION_STATE_START FOR DbId = [%ld]" , counter->dbId );
				taskCreationState = TASK_CREATION_STATE_SYNC_TIME;
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_SYNC_TIME:
			{
				#if COUNTER_TASK_SYNC_TIME
					COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TASK_CREATION_STATE_SYNC_TIME syncFlag [%02X]" , counter->syncFlag );
					if ( STORAGE_IsCounterNeedToSync( &counter ) == TRUE )
					{
						taskCreationState = TASK_CREATION_STATE_DONE ;
						res = GUBANOV_SyncTime( counter , counterTask );
						if ( res == ERROR_SYNC_IS_IMPOSSIBLE )
						{
							STORAGE_SwitchSyncFlagTo_SyncFail( &counter ) ;
							taskCreationState = TASK_CREATION_STATE_WRITE_TARIFF_MAP ;
						}
					}
					else
					{
						COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Do not need to sync");
						taskCreationState = TASK_CREATION_STATE_WRITE_TARIFF_MAP ;
					}
				#else
					taskCreationState = TASK_CREATION_STATE_WRITE_TARIFF_MAP ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_WRITE_TARIFF_MAP:
			{
				#if COUNTER_TASK_WRITE_TARIFF_MAP
					COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TASK_CREATION_STATE_WRITE_TARIFF_MAP");

					taskCreationState = TASK_CREATION_STATE_DONE ;

					unsigned char * tariff = NULL ;
					int tariffLength = 0 ;
					if ( ACOUNT_Get( counter->dbId , &tariff , &tariffLength , ACOUNTS_PART_TARIFF) == OK )
					{
						if ( tariff )
						{
							res = GUBANOV_WriteTariff(counter, counterTask , tariff, tariffLength) ;

							if ( res == OK )
							{
								ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_IN_PROCESS , ACOUNTS_PART_TARIFF );

								unsigned char evDesc[EVENT_DESC_SIZE];
								unsigned char dbidDesc[EVENT_DESC_SIZE];
								memset(dbidDesc , 0, EVENT_DESC_SIZE);
								COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
								COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_TARIFF_WRITING_START, evDesc);
								STORAGE_JournalUspdEvent( EVENT_TARIFF_MAP_WRITING , evDesc );

							}
							else
							{
								if ( res == ERROR_FILE_FORMAT )
								{
									res = OK ;
									taskCreationState = TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES ;

									unsigned char evDesc[EVENT_DESC_SIZE];
									unsigned char dbidDesc[EVENT_DESC_SIZE];
									memset(dbidDesc , 0, EVENT_DESC_SIZE);
									COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
									COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_TARIFF_WRITING_ERR_FORM, evDesc);
									STORAGE_JournalUspdEvent( EVENT_TARIFF_MAP_WRITING , evDesc );

									ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_FILE_FORMAT , ACOUNTS_PART_TARIFF );
								}
								else if ( res == ERROR_COUNTER_TYPE )
								{
									res = OK ;
									taskCreationState = TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES ;

									unsigned char evDesc[EVENT_DESC_SIZE];
									unsigned char dbidDesc[EVENT_DESC_SIZE];
									memset(dbidDesc , 0, EVENT_DESC_SIZE);
									COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
									COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_TARIFF_WRITING_ERR_TYPE, evDesc);
									STORAGE_JournalUspdEvent( EVENT_TARIFF_MAP_WRITING , evDesc );

									ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_COUNTER_TYPE , ACOUNTS_PART_TARIFF );
								}

								//free transactions in current task if need
								if ( (*counterTask)->transactions != NULL )
								{
									unsigned int transactionIndex = 0 ;
									for ( ; transactionIndex < (*counterTask)->transactionsCount ; ++transactionIndex )
									{
										if ( (*counterTask)->transactions[ transactionIndex ] )
										{
											if ( (*counterTask)->transactions[ transactionIndex ]->command )
											{
												free( (*counterTask)->transactions[ transactionIndex ]->command );
												(*counterTask)->transactions[ transactionIndex ]->command = NULL ;
											}
											free((*counterTask)->transactions[ transactionIndex ]);
											(*counterTask)->transactions[ transactionIndex ] = NULL ;
										}
									}
									free( (*counterTask)->transactions );
									(*counterTask)->transactions = NULL ;
									(*counterTask)->transactionsCount = 0 ;
								}
							}

							free( tariff );
							tariff = NULL ;
							tariffLength = 0 ;
						}
						else
						{
							unsigned char evDesc[EVENT_DESC_SIZE];
							unsigned char dbidDesc[EVENT_DESC_SIZE];
							memset(dbidDesc , 0, EVENT_DESC_SIZE);
							COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
							COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_TARIFF_WRITING_ERR_FORM, evDesc);
							STORAGE_JournalUspdEvent( EVENT_TARIFF_MAP_WRITING , evDesc );

							ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_FILE_FORMAT , ACOUNTS_PART_TARIFF );
							taskCreationState = TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES ;
						}
					}
					else
					{
						COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Do not need to write tariff");
						taskCreationState = TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES ;
					}
				#else
					taskCreationState = TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES:
			{
				#if COUNTER_TASK_WRITE_PSM
					COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TASK_CREATION_STATE_WRITE_POWER_SWITCHER_MODULE_RULES");

					taskCreationState = TASK_CREATION_STATE_DONE ;

					unsigned char * psmRules = NULL ;
					int psmRulesLength = 0 ;
					if ( ACOUNT_Get( counter->dbId , &psmRules , &psmRulesLength , ACOUNTS_PART_POWR_CTRL) == OK )
					{
						if ( psmRules )
						{
							res = GUBANOV_WritePSM(counter, counterTask , psmRules, psmRulesLength) ;
							if ( res == OK )
							{
								ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_IN_PROCESS , ACOUNTS_PART_POWR_CTRL );

								unsigned char evDesc[EVENT_DESC_SIZE];
								unsigned char dbidDesc[EVENT_DESC_SIZE];
								memset(dbidDesc , 0, EVENT_DESC_SIZE);
								COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
								COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_PSM_RULES_WRITING_START, evDesc);
								STORAGE_JournalUspdEvent( EVENT_PSM_RULES_WRITING , evDesc );
							}
							else
							{
								if ( res == ERROR_FILE_FORMAT )
								{
									res = OK ;
									taskCreationState = TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME ;

									ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_FILE_FORMAT , ACOUNTS_PART_POWR_CTRL );

									unsigned char evDesc[EVENT_DESC_SIZE];
									unsigned char dbidDesc[EVENT_DESC_SIZE];
									memset(dbidDesc , 0, EVENT_DESC_SIZE);
									COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
									COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_PSM_RULES_WRITING_ERR_FORM, evDesc);
									STORAGE_JournalUspdEvent( EVENT_PSM_RULES_WRITING , evDesc );
								}
								else if ( res == ERROR_COUNTER_TYPE )
								{
									res = OK ;
									taskCreationState = TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME ;

									ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_COUNTER_TYPE , ACOUNTS_PART_POWR_CTRL );

									unsigned char evDesc[EVENT_DESC_SIZE];
									unsigned char dbidDesc[EVENT_DESC_SIZE];
									memset(dbidDesc , 0, EVENT_DESC_SIZE);
									COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
									COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_PSM_RULES_WRITING_ERR_TYPE, evDesc);
									STORAGE_JournalUspdEvent( EVENT_PSM_RULES_WRITING , evDesc );
								}

								//free transactions in current task if need
								if ( (*counterTask)->transactions != NULL )
								{
									unsigned int transactionIndex = 0 ;
									for ( ; transactionIndex < (*counterTask)->transactionsCount ; ++transactionIndex )
									{
										if ( (*counterTask)->transactions[ transactionIndex ] )
										{
											if ( (*counterTask)->transactions[ transactionIndex ]->command )
											{
												free( (*counterTask)->transactions[ transactionIndex ]->command );
												(*counterTask)->transactions[ transactionIndex ]->command = NULL ;
											}
											free((*counterTask)->transactions[ transactionIndex ]);
											(*counterTask)->transactions[ transactionIndex ] = NULL ;
										}
									}
									free( (*counterTask)->transactions );
									(*counterTask)->transactions = NULL ;
									(*counterTask)->transactionsCount = 0 ;
								}
							}

							free( psmRules );
							psmRules = NULL ;
							psmRulesLength = 0 ;
						}
						else
						{
							unsigned char evDesc[EVENT_DESC_SIZE];
							unsigned char dbidDesc[EVENT_DESC_SIZE];
							memset(dbidDesc , 0, EVENT_DESC_SIZE);
							COMMON_TranslateLongToChar(counter->dbId, dbidDesc);
							COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_PSM_RULES_WRITING_ERR_FORM, evDesc);
							STORAGE_JournalUspdEvent( EVENT_PSM_RULES_WRITING , evDesc );

							ACOUNT_StatusSet( counter->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_FILE_FORMAT , ACOUNTS_PART_POWR_CTRL );
							taskCreationState = TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME ;
						}
					}
					else
					{
						COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Do not need to write power switcher rules");
						taskCreationState = TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME ;
					}
					//TODO
				#else
					taskCreationState = TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_RATIO_DATE_AND_TIME:
			{
				#if COUNTER_TASK_READ_DTS
					//get current time and date of counter
					if ( res == OK )
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
								res = GUBANOV_Command_GetCurrentDateAndTime(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1]);
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}
					taskCreationState = TASK_CREATION_STATE_READ_CUR_METERAGES ;
				#else
					taskCreationState = TASK_CREATION_STATE_DONE ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_CUR_METERAGES:
			{
				#if COUNTER_TASK_READ_CUR_METERAGES

					if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T1))
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
								res = GUBANOV_Command_CurrentMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , TARIFF_1 );
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}

					if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T2))
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
								res = GUBANOV_Command_CurrentMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , TARIFF_2 );
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}

					if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T3))
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
								res = GUBANOV_Command_CurrentMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , TARIFF_3 );
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}

					if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T4))
					{
						(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
						if ((*counterTask)->transactions != NULL) {
							(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
							if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
								res = GUBANOV_Command_CurrentMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , TARIFF_4 );
							else
								return ERROR_MEM_ERROR;
						} else
							return ERROR_MEM_ERROR;
					}

					taskCreationState = TASK_CREATION_STATE_READ_MON_METERAGES ;
				#else
					taskCreationState = TASK_CREATION_STATE_READ_MON_METERAGES ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_MON_METERAGES:
			{
				#if COUNTER_TASK_READ_MON_METERAGES

					taskCreationState = TASK_CREATION_STATE_READ_DAY_METERAGES ;

					if ( res == OK )
					{
						dateTimeStamp dtsToRequest;
						memset(&dtsToRequest , 0 , sizeof(dateTimeStamp));

						if ( STORAGE_CheckTimeToRequestMeterage( counter , STORAGE_MONTH , &dtsToRequest, NULL ) == TRUE )
						{
							const int monthMeterageStorageDepth = COMMON_GetStorageDepth( counter , STORAGE_MONTH ) - 1 ;
							int dateDifference = COMMON_GetDtsDiff__Alt( &dtsToRequest , &counter->currentDts , BY_MONTH ) ;
							if( dateDifference > monthMeterageStorageDepth )
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV difference between last meterage dts and current counter dts is [%d] month", dateDifference);

								meterage_t fillMeterage ;
								memset(&fillMeterage, 0, sizeof (meterage_t));
								fillMeterage.status = METERAGE_OUT_OF_ARCHIVE_BOUNDS;

								//fill the storage
								do
								{
									COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV fill empty month for: " DTS_PATTERN, DTS_GET_BY_VAL(dtsToRequest));
									memcpy(&fillMeterage.dts, &dtsToRequest, sizeof (dateTimeStamp));
									res = STORAGE_SaveMeterages(STORAGE_MONTH, counter->dbId, &fillMeterage);
									if ( res != OK )
									{
										break;
									}
									COMMON_ChangeDts( &dtsToRequest , INC , BY_MONTH );
									dateDifference = COMMON_GetDtsDiff__Alt( &dtsToRequest , &counter->currentDts , BY_MONTH ) ;
								}
								while( dateDifference > monthMeterageStorageDepth );
							}

							if ( (dateDifference <= monthMeterageStorageDepth) && (dateDifference >= 0) )
							{
								memcpy( &counter->monthMetaragesLastRequestDts , &dtsToRequest.d , sizeof(dateStamp) );

								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV It is time to request month meterage for: " DTS_PATTERN, DTS_GET_BY_VAL(dtsToRequest));

								if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T1))
								{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
											res = GUBANOV_Command_MonthMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , dtsToRequest.d.m , TARIFF_1 );
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
								}
								if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T2))
								{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
											res = GUBANOV_Command_MonthMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , dtsToRequest.d.m , TARIFF_2 );
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
								}
								if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T3))
								{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
											res = GUBANOV_Command_MonthMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , dtsToRequest.d.m , TARIFF_3 );
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
								}
								if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T4))
								{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
											res = GUBANOV_Command_MonthMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , dtsToRequest.d.m , TARIFF_4 );
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
								}
							}
						}
					}

				#else
					taskCreationState = TASK_CREATION_STATE_READ_DAY_METERAGES ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_DAY_METERAGES:
			{
				#if COUNTER_TASK_READ_DAY_METERAGES
					taskCreationState = TASK_CREATION_STATE_READ_PROFILES ;

					if ( res == OK )
					{
						switch( counter->type )
						{
							case TYPE_GUBANOV_PSH_3ART:
							case TYPE_GUBANOV_PSH_3ART_1:
							case TYPE_GUBANOV_PSH_3ART_D:
							case TYPE_GUBANOV_MAYK_301:
							case TYPE_GUBANOV_MAYK_102:
							{

								dateTimeStamp dtsToRequest;
								memset(&dtsToRequest , 0 , sizeof(dateTimeStamp));
								if ( STORAGE_CheckTimeToRequestMeterage( counter , STORAGE_DAY , &dtsToRequest, NULL ) == TRUE )
								{
									const int dayMeterageStorageDepth = COMMON_GetStorageDepth( counter , STORAGE_DAY );

									COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV storage depth for this counter type [%hhu] is [%d]" , counter->type , dayMeterageStorageDepth );

									int dateDifference = COMMON_GetDtsDiff__Alt( &dtsToRequest , &counter->currentDts , BY_DAY ) ;

									if ( dateDifference >= dayMeterageStorageDepth )
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV difference between last meterage dts and current counter dts is [%d] day", dateDifference);
										//fill the storage
										meterage_t fillMeterage ;
										memset(&fillMeterage, 0, sizeof (meterage_t));
										fillMeterage.status = METERAGE_OUT_OF_ARCHIVE_BOUNDS;

										do
										{
											COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV fill empty day for: " DTS_PATTERN, DTS_GET_BY_VAL(dtsToRequest));
											memcpy(&fillMeterage.dts, &dtsToRequest, sizeof (dateTimeStamp));

											res = STORAGE_SaveMeterages(STORAGE_DAY, counter->dbId, &fillMeterage);
											if ( res != OK )
											{
												break;
											}
											COMMON_ChangeDts( &dtsToRequest , INC , BY_DAY );
											dateDifference = COMMON_GetDtsDiff__Alt( &dtsToRequest , &counter->currentDts , BY_DAY ) ;
										}
										while( dateDifference >= dayMeterageStorageDepth );
									}

									if ( (dateDifference < dayMeterageStorageDepth) && (dateDifference >= 0) )
									{
										memcpy( &counter->dayMetaragesLastRequestDts , &dtsToRequest.d , sizeof(dateStamp) );

										if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T1))
										{
											(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
											if ((*counterTask)->transactions != NULL) {
												(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
												if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
													res = GUBANOV_Command_DayMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] ,TARIFF_1 , dateDifference );
												else
													return ERROR_MEM_ERROR;
											} else
												return ERROR_MEM_ERROR;
										}
										if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T2))
										{
											(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
											if ((*counterTask)->transactions != NULL) {
												(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
												if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
													res = GUBANOV_Command_DayMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] ,TARIFF_2 , dateDifference );
												else
													return ERROR_MEM_ERROR;
											} else
												return ERROR_MEM_ERROR;
										}
										if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T3))
										{
											(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
											if ((*counterTask)->transactions != NULL) {
												(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
												if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
													res = GUBANOV_Command_DayMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] ,TARIFF_3 , dateDifference );
												else
													return ERROR_MEM_ERROR;
											} else
												return ERROR_MEM_ERROR;
										}
										if ((res == OK) && (counter->maskTarifs & COUNTER_MASK_T4))
										{
											(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
											if ((*counterTask)->transactions != NULL) {
												(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
												if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
													res = GUBANOV_Command_DayMeterage(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] ,TARIFF_4 , dateDifference );
												else
													return ERROR_MEM_ERROR;
											} else
												return ERROR_MEM_ERROR;
										}
									}
								}
							}
							break;

							default:
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Counter type is not ART. Do not ask for day meterages" );
							}
							break;
						}
					}

				#else
					taskCreationState = TASK_CREATION_STATE_READ_PROFILES ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_PROFILES:
			{
				#if COUNTER_TASK_READ_PROFILES

					COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TASK_CREATION_STATE_READ_PROFILES");

					taskCreationState = TASK_CREATION_STATE_READ_JOURNALS ;

					if ( ( res == OK ) && ( counter->profile == POWER_PROFILE_READ ) )
					{
						if ( counter->profileInterval == STORAGE_UNKNOWN_PROFILE_INTERVAL ){
							counter->profileInterval = 30 ;
						}

						dateTimeStamp dtsToRequest;
						memset( &dtsToRequest , 0 , sizeof(dateTimeStamp) );

						if ( STORAGE_CheckTimeToRequestMeterage(counter , STORAGE_PROFILE, &dtsToRequest, NULL ) == TRUE )
						{
							COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV It is time to read profile");

							BOOL addProfileRequestToTaskFlag = TRUE ;

							//counter storage contain profiles for 2 month
							dateTimeStamp firstPossibleDts ;
							memset( &firstPossibleDts , 0 , sizeof( dateTimeStamp));
							memcpy( &firstPossibleDts , &counter->currentDts , sizeof( dateTimeStamp) );
							COMMON_ChangeDts( &firstPossibleDts , DEC , BY_MONTH );
							firstPossibleDts.d.d = 1 ;
							firstPossibleDts.t.h = 0 ;
							firstPossibleDts.t.m = 0 ;
							firstPossibleDts.t.s = 0 ;

							//check difference
							int dtsDifference = COMMON_GetDtsDiff__Alt( &dtsToRequest , &firstPossibleDts , BY_HALF_HOUR ) ;
							if ( dtsDifference > 0 )
							{
								//fill storage by empty profiles
								profile_t fillProfile;
								memset( &fillProfile , 0 , sizeof(profile_t) );
								fillProfile.ratio = GUBANOV_METERAGES_RATIO_VALUE;
								fillProfile.p.e[ENERGY_AP - 1] = 0;
								fillProfile.p.e[ENERGY_AM - 1] = 0;
								fillProfile.p.e[ENERGY_RP - 1] = 0;
								fillProfile.p.e[ENERGY_RM - 1] = 0;
								fillProfile.status = METERAGE_OUT_OF_ARCHIVE_BOUNDS ;

								do
								{
									memcpy( &fillProfile.dts , &dtsToRequest , sizeof(dateTimeStamp) );
									COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV fill the profile storage for the " DTS_PATTERN , DTS_GET_BY_VAL(dtsToRequest) ) ;
									res = STORAGE_SaveProfile( counter->dbId , &fillProfile) ;
									if ( res != OK )
									{
										addProfileRequestToTaskFlag = FALSE ;
										break;
									}
									COMMON_ChangeDts( &dtsToRequest , INC , BY_HALF_HOUR ) ;
									dtsDifference = COMMON_GetDtsDiff__Alt( &dtsToRequest , &firstPossibleDts , BY_HALF_HOUR ) ;
								}
								while( dtsDifference > 0 );

							}
							COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV request profile for the " DTS_PATTERN , DTS_GET_BY_VAL(dtsToRequest) ) ;

							if ( addProfileRequestToTaskFlag == TRUE )
							{
								(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
								if ((*counterTask)->transactions != NULL) {
									(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
									if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
										res = GUBANOV_Command_GetProfile(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] , &dtsToRequest );
									else
										return ERROR_MEM_ERROR;
								} else
									return ERROR_MEM_ERROR;
							}
						}
						else
						{
							COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV It is not time to read profile");
						}

					}

				#else
					taskCreationState = TASK_CREATION_STATE_READ_JOURNALS ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_JOURNALS:
			{
				#if COUNTER_TASK_READ_JOURNALS
					taskCreationState = TASK_CREATION_STATE_READ_PQP ;
					if ( res == OK)
					{
						switch( counter->type )
						{
							case TYPE_GUBANOV_PSH_3ART:
							case TYPE_GUBANOV_PSH_3ART_1:
							case TYPE_GUBANOV_PSH_3ART_D:
							case TYPE_GUBANOV_MAYK_301:
							{
								res = GUBANOV_GetImprovedJournal( counter , counterTask );
							}
							break;

							default:
							{
								res = GUBANOV_GetSimpleJournal( counter , counterTask );
							}
							break;
						}
					}

				#else
					taskCreationState = TASK_CREATION_STATE_READ_PQP ;
				#endif
			}
			break;

			//
			//
			//

			case TASK_CREATION_STATE_READ_PQP:
			{
				#if COUNTER_TASK_READ_PQP
					taskCreationState = TASK_CREATION_STATE_DONE ;

					if ( res == OK )
					{
						switch( counter->type )
						{
							case TYPE_GUBANOV_PSH_3ART:
							case TYPE_GUBANOV_PSH_3ART_1:
							case TYPE_GUBANOV_PSH_3ART_D:
							case TYPE_GUBANOV_MAYK_301:
							{
								//
								// TODO
								//
							}
							break;

							default:
							{
									(*counterTask)->transactions = (counterTransaction_t **) realloc((*counterTask)->transactions, (++(*counterTask)->transactionsCount) * sizeof (counterTransaction_t *));
									if ((*counterTask)->transactions != NULL) {
										(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] = (counterTransaction_t *) malloc(sizeof (counterTransaction_t));
										if (((*counterTask)->transactions[(*counterTask)->transactionsCount - 1]) != NULL)
											res = GUBANOV_Command_GetPqp_ActivePower(counter, &(*counterTask)->transactions[(*counterTask)->transactionsCount - 1] );
										else
											return ERROR_MEM_ERROR;
									} else
										return ERROR_MEM_ERROR;
							}
							break;
						}
					}
				#else
					taskCreationState = TASK_CREATION_STATE_DONE ;
				#endif
			}
			break;

			//
			//
			//

			default:
			{
				taskCreationState = TASK_CREATION_STATE_DONE ;
			}
			break;
		} //switch(taskCreationState)
	} //while ( taskCreationState != TASK_CREATION_STATE_DONE )


	COMMON_GetCurrentDts( &(*counterTask)->dtsStart );

	return res;
}

//
//
//
int GUBANOV_SaveTaskResults(counterTask_t * counterTask)
{
	DEB_TRACE(DEB_IDX_GUBANOV);

	int res = ERROR_GENERAL;

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_SaveTaskResults start");

	powerQualityParametres_t pqp;
	memset(&pqp , 0 , sizeof(powerQualityParametres_t));
	int pqpOkFlag = 0;

	counterTask->resultMonth.status = METERAGE_NOT_PRESET ;
	counterTask->resultDay.status = METERAGE_NOT_PRESET ;

	unsigned int transactionIndex = 0;
	unsigned char transactionType = 0;
	unsigned char transactionResult = TRANSACTION_DONE_OK;

	unsigned int resultMetrMonthMsk = 0 ;
	unsigned int resultMetrCurrMsk = 0 ;
	unsigned int resultMetrDayMsk = 0 ;

	int powerControlTransactionCounter = 0 ;
	int tariffMapTransactionCounter = 0 ;
	int tariffHolidaysTransactionCounter = 0;	
	int indicationWritingTransactionCounter = 0 ;

	counter_t * counterPtr = NULL;
	if ( STORAGE_FoundCounter( counterTask->counterDbId , &counterPtr ) == OK )
	{
		if( counterPtr )
		{
			counterPtr->lastTaskRepeateFlag = 0 ;

			for (; ((transactionIndex < counterTask->transactionsCount) && (transactionResult == TRANSACTION_DONE_OK)); transactionIndex++)
			{

				transactionType = counterTask->transactions[transactionIndex]->transactionType;
				transactionResult = counterTask->transactions[transactionIndex]->result;

				COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV SaveTaskResult: transaction [%02X], result [ %s ]" , transactionType, getTransactionResult(transactionResult));
				//COMMON_BUF_DEBUG( DEBUG_GUBANOV "ANSWER: ", counterTask->transactions[transactionIndex]->answer , counterTask->transactions[transactionIndex]->answerSize);

				COMMON_ASCII_DEBUG( DEBUG_GUBANOV "REQUEST: ", counterTask->transactions[transactionIndex]->command , counterTask->transactions[transactionIndex]->commandSize ) ;
				COMMON_ASCII_DEBUG( DEBUG_GUBANOV "ANSWER: ", counterTask->transactions[transactionIndex]->answer , counterTask->transactions[transactionIndex]->answerSize);


				if ( transactionResult == TRANSACTION_DONE_OK )
				{
					int tTime = counterTask->transactions[transactionIndex]->tStop - counterTask->transactions[transactionIndex]->tStart;
					if (tTime >= 0){
						counterPtr->transactionTime = tTime;
						COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV tStop [%ld] ; tStart [%ld]" , counterTask->transactions[transactionIndex]->tStop , counterTask->transactions[transactionIndex]->tStart);
						COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV transaction time : %d sec", tTime);
					}

					BOOL crcFlag = FALSE ;


					int answerSize = counterTask->transactions[transactionIndex]->answerSize ;
					unsigned char * answer = NULL;
					if( answerSize > 0 )
					{
						COMMON_AllocateMemory( &answer  , answerSize );
						memcpy( answer , counterTask->transactions[transactionIndex]->answer , answerSize * sizeof(unsigned char) );

						//check crc
						if ( answerSize > 3 )
						{
							unsigned char expectedCrc[GUBANOV_SIZE_OF_CRC ];
							memset( expectedCrc , 0 , GUBANOV_SIZE_OF_CRC );

							GUBANOV_GetCRC( answer , answerSize - 3 , expectedCrc ) ;
							if ( !memcmp( expectedCrc , &answer[ answerSize - 3 ] , GUBANOV_SIZE_OF_CRC ) )	{
								crcFlag = TRUE ;
							}
						}
					}

					if ( answerSize == 0 )
					{
						crcFlag = TRUE ;
					}

					if ( crcFlag == TRUE )
					{
						//fix last success poll time in  memory
						time(&counterPtr->lastPollTime);

						//parse data by transactions
						switch (transactionType)
						{
							case TRANSACTION_DATE_TIME_STAMP:
							{
								if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == 'D')
								{
									COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_DATE_TIME_STAMP catched" );
									dateTimeStamp currentCounterDts;
									memset( &currentCounterDts , 0 , sizeof(dateTimeStamp) );
									unsigned int aIndex = 6;

									currentCounterDts.t.h = COMMON_AsciiToInt( &answer[ aIndex ] , 2 , 10) ;
									currentCounterDts.t.m = COMMON_AsciiToInt( &answer[ aIndex + 2 ] , 2 , 10) ;
									currentCounterDts.t.s = COMMON_AsciiToInt( &answer[ aIndex + 4 ] , 2 , 10) ;
									currentCounterDts.d.d = COMMON_AsciiToInt( &answer[ aIndex + 6 ] , 2 , 10) ;
									currentCounterDts.d.m = COMMON_AsciiToInt( &answer[ aIndex + 8 ] , 2 , 10) ;
									currentCounterDts.d.y = COMMON_AsciiToInt( &answer[ aIndex + 10] , 2 , 10) ;

									//COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV current counter dts is " DTS_PATTERN, DTS_GET_BY_VAL(currentCounterDts));

									//debug
									dateTimeStamp currentDts ;
									memset(&currentDts , 0 ,sizeof(dateTimeStamp) );
									COMMON_GetCurrentDts(&currentDts);
									int taskTime = COMMON_GetDtsDiff__Alt( &counterTask->dtsStart , &currentDts , BY_SEC );

									if ( ( counterPtr->syncFlag & 0xC0 ) == (STORAGE_SYNC_DATE_SET + STORAGE_SYNC_TIME_SET) )
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "Checking dts after time syncing" );
										counterPtr->lastTaskRepeateFlag = 1 ;

										counterPtr->syncFlag -= STORAGE_SYNC_TIME_SET ;
										counterPtr->syncFlag -= STORAGE_SYNC_DATE_SET ;

										int worth = COMMON_GetDtsWorth( &currentCounterDts ) + taskTime;
										if ( abs(worth) <= 2 )
										{
											COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Time sync is OK" );
											STORAGE_SwitchSyncFlagTo_SyncDone(&counterPtr);
											STORAGE_UpdateCounterDts( counterTask->counterDbId , &currentCounterDts ,taskTime);

											STORAGE_ClearMeteragesAfterHardSync( counterPtr );

											unsigned char desc[ EVENT_DESC_SIZE ];
											memset(desc , 0 , EVENT_DESC_SIZE);

											desc[0] = (char)( ( counterPtr->dbId & 0xFF000000) >> 24 );
											desc[1] = (char)( ( counterPtr->dbId & 0xFF0000) >> 16 );
											desc[2] = (char)( ( counterPtr->dbId & 0xFF00) >> 8 );
											desc[3] = (char) ( counterPtr->dbId & 0xFF) ;
											STORAGE_JournalUspdEvent( EVENT_COUNTER_METERAGES_CLEAR , desc );
										}
										else
										{
											COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Time sync is not OK. Try again" );
											counterPtr->syncWorth = worth ;
											memcpy( &counterPtr->currentDts , &currentCounterDts , sizeof(dateTimeStamp) );
										}
									}
									else
									{
										if ( COMMON_CheckDtsForValid(&counterPtr->currentDts) != OK )
										{
											counterPtr->lastTaskRepeateFlag = 1 ;
										}
										STORAGE_UpdateCounterDts( counterTask->counterDbId , &currentCounterDts ,taskTime);

										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Current counter dts is " DTS_PATTERN, DTS_GET_BY_VAL(currentCounterDts));


										COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV Current USPD dts is " DTS_PATTERN, DTS_GET_BY_VAL(currentDts));
										COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV worth is [%d] , transactionTime = [%d]", counterPtr->syncWorth , counterPtr->transactionTime);
									}

								}
								else
								{
									COMMON_STR_DEBUG(DEBUG_GUBANOV "Answer code is invalid");
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break; //case TRANSACTION_DATE_TIME_STAMP

							case (TRANSACTION_METERAGE_CURRENT + TARIFF_1):
							case (TRANSACTION_METERAGE_CURRENT + TARIFF_2):
							case (TRANSACTION_METERAGE_CURRENT + TARIFF_3):
							case (TRANSACTION_METERAGE_CURRENT + TARIFF_4):
							{
								unsigned char tarifIndex = (transactionType - TRANSACTION_METERAGE_CURRENT);

								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_METERAGE_CURRENT for tariff [%d] catched" , tarifIndex );

								switch( counterPtr->type )
								{
									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV counter type is [%d]" , counterPtr->type );
										//
										// TODO
										//
									}
									break;

									case TYPE_GUBANOV_MAYK_102:
									{
										unsigned char waitCommandCode = currentTarifs[ tarifIndex ] ;
										if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == waitCommandCode)
										{
											unsigned int eAp = COMMON_AsciiToInt( &answer[ GUBANOV_COMMAND_ANSWER_POS + 1 ] , 8 , 10) ;
											unsigned int eAm = 0;
											unsigned int eRp = 0;
											unsigned int eRm = 0;

											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d A+ %d", tarifIndex, eAp);
											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d A- %d", tarifIndex, eAm);
											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d R+ %d", tarifIndex, eRp);
											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d R- %d", tarifIndex, eRm);

											memcpy(&counterTask->resultCurrent.dts, &counterPtr->currentDts, sizeof (dateTimeStamp));
											resultMetrCurrMsk |= (1 << (tarifIndex - 1) ) ;
											if ( resultMetrCurrMsk == counterPtr->maskTarifs )
											{
												counterTask->resultCurrentReady = READY;
											}
											counterTask->resultCurrent.status = METERAGE_VALID;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_AP - 1] = eAp;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_AM - 1] = eAm;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_RP - 1] = eRp;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_RM - 1] = eRm;

											counterTask->resultCurrent.ratio = GUBANOV_METERAGES_RATIO_VALUE;
											counterTask->resultCurrent.delimeter = 4;
										}
										else
										{
											COMMON_STR_DEBUG(DEBUG_GUBANOV "Answer code is invalid");
											transactionResult = TRANSACTION_DONE_ERROR_A ;
										}
									}
									break;

									default:
									{
										unsigned char waitCommandCode = currentTarifs[ tarifIndex ] ;
										if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == waitCommandCode)
										{
											unsigned int eAp = COMMON_AsciiToInt( &answer[ GUBANOV_COMMAND_ANSWER_POS + 1 ] , 8 , 10) ;
											unsigned int eAm = 0;
											unsigned int eRp = 0;
											unsigned int eRm = 0;

											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d A+ %d", tarifIndex, eAp);
											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d A- %d", tarifIndex, eAm);
											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d R+ %d", tarifIndex, eRp);
											COMMON_STR_DEBUG(DEBUG_GUBANOV "CURRENT T%d R- %d", tarifIndex, eRm);

											memcpy(&counterTask->resultCurrent.dts, &counterPtr->currentDts, sizeof (dateTimeStamp));
											resultMetrCurrMsk |= (1 << (tarifIndex - 1) ) ;
											if ( resultMetrCurrMsk == counterPtr->maskTarifs )
											{
												counterTask->resultCurrentReady = READY;
											}
											counterTask->resultCurrent.status = METERAGE_VALID;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_AP - 1] = eAp;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_AM - 1] = eAm;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_RP - 1] = eRp;
											counterTask->resultCurrent.t[tarifIndex - 1].e[ENERGY_RM - 1] = eRm;

											counterTask->resultCurrent.ratio = GUBANOV_METERAGES_RATIO_VALUE;
											counterTask->resultCurrent.delimeter = 4;

											//fill the day meterages storage
											if ( counterTask->resultCurrentReady == READY )
											{
												BOOL mountDFlag = FALSE ;
												COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV Meterge current is ready. Check if it is time to save it to day meterage storage" );
												dateTimeStamp lastMeterageDts;
												memset( &lastMeterageDts , 0 , sizeof(dateTimeStamp) );
												if ( STORAGE_GetLastMeterageDts(STORAGE_DAY, counterPtr->dbId, &lastMeterageDts) != OK)
												{
													memcpy( &lastMeterageDts.d , &counterPtr->mountD , sizeof(dateStamp) );
													mountDFlag = TRUE ;
													COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV there is no day meterages in storage. Use mount date " DTS_PATTERN , DTS_GET_BY_VAL(lastMeterageDts) ) ;
												}
												else
												{
													COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV last day meterage have dateStamp " DTS_PATTERN , DTS_GET_BY_VAL(lastMeterageDts) ) ;
												}

												dateTimeStamp currentCounterDts;
												memcpy( &currentCounterDts.d , &counterPtr->currentDts.d , sizeof(dateStamp) );
												memset( &currentCounterDts.t , 0 , sizeof(timeStamp) );

												int dtsDifference = COMMON_GetDtsDiff__Alt( &lastMeterageDts , &currentCounterDts ,BY_DAY ) ;
												COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV difference between last day meterge dateStamp and current date is [%d]" , dtsDifference );
												if ( dtsDifference > 1 )
												{
													//fill the storage
													meterage_t meterage;
													memset(&meterage, 0, sizeof (meterage_t));
													meterage.status = METERAGE_OUT_OF_ARCHIVE_BOUNDS;
													do
													{
														COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV fill the day storage for the " DTS_PATTERN , DTS_GET_BY_VAL(lastMeterageDts) ) ;
														memcpy(&meterage.dts, &lastMeterageDts, sizeof (dateTimeStamp));
														if ( STORAGE_SaveMeterages(STORAGE_DAY, counterPtr->dbId, &meterage) != OK )
														{
															mountDFlag = FALSE ;
															break ;
														}
														COMMON_ChangeDts( &lastMeterageDts , INC , BY_DAY );
														dtsDifference = COMMON_GetDtsDiff__Alt( &lastMeterageDts , &currentCounterDts , BY_DAY ) ;
													}
													while(dtsDifference >= 1);
													COMMON_STR_DEBUG(DEBUG_GUBANOV "GUBANOV stamp after filling is " DTS_PATTERN , DTS_GET_BY_VAL(lastMeterageDts) ) ;
												}

												if ( (dtsDifference == 1) || (mountDFlag == TRUE))
												{
													//use current meterage like day meterage
													memcpy( &counterTask->resultDay , &counterTask->resultCurrent , sizeof( meterage_t ) );
													memset( &counterTask->resultDay.dts.t , 0 , sizeof( timeStamp ));
													counterTask->resultDayReady = READY;
												}
											}
										}
										else
										{
											COMMON_STR_DEBUG(DEBUG_GUBANOV "Answer code is invalid");
											transactionResult = TRANSACTION_DONE_ERROR_A ;
										}
									}
									break;
								}


							}
							break; //case TRANSACTION_METERAGE_CURRENT

							case TRANSACTION_PROFILE_GET_VALUE:
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_PROFILE_GET_VALUE");

								memset( &counterTask->resultProfile , 0 , sizeof( profile_t ) );
								memcpy(&(counterTask->resultProfile.dts) , &counterPtr->profileLastRequestDts, sizeof(dateTimeStamp));

								int aIndex = GUBANOV_COMMAND_ANSWER_POS ;
								int monthIndex = 0;

								int aEnergyIndex = -1 ;
								int aEnergySize = 6 ;
								int aEnergyRatio = 1 ;

								int rEnergyIndex = -1 ;
								int rEnergySize = 6 ;
								int rEnergyRatio = 1 ;

								switch( counterPtr->type )
								{
									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										monthIndex = COMMON_AsciiToInt( &answer[ aIndex + 2 ] , 1 , 16) ;
										aEnergyIndex = aIndex + 3;
										//
										// TODO
										//
									}
									break;

									case TYPE_GUBANOV_PSH_3TA07_DOT_2:
									{
										monthIndex = COMMON_AsciiToInt( &answer[ aIndex + 1 ] , 1 , 16) ;
										aEnergyIndex = aIndex + 2;
										aEnergyRatio = 100 ;
										aEnergySize = 6 ;

										rEnergyIndex = -1 ;
									}
									break;

									default:
									{
										monthIndex = COMMON_AsciiToInt( &answer[ aIndex + 1 ] , 1 , 16) ;
										aEnergyIndex = aIndex + 2;
										aEnergyRatio = 10 ;
										aEnergySize = 6 ;

										rEnergyIndex = -1 ;
									}
									break;
								} //switch

								if ( monthIndex == counterPtr->profileLastRequestDts.d.m )
								{
									counterTask->resultProfile.ratio = GUBANOV_METERAGES_RATIO_VALUE;

									if ( aEnergyIndex > 0)
									{
										counterTask->resultProfile.p.e[ ENERGY_AP - 1 ] = COMMON_AsciiToInt( &answer[ aEnergyIndex ] , aEnergySize , 16 ) /  aEnergyRatio;

										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV: profile A+ = [%d]" ,  counterTask->resultProfile.p.e[ ENERGY_AP - 1 ]);

										counterPtr->lastTaskRepeateFlag = 1 ;

									}
									if ( rEnergyIndex > 0)
									{
										counterTask->resultProfile.p.e[ ENERGY_RP - 1 ] = COMMON_AsciiToInt( &answer[ rEnergyIndex ] , rEnergySize , 16 ) / rEnergyRatio;
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV: profile R+ = [%d]" ,  counterTask->resultProfile.p.e[ ENERGY_RP - 1 ]);
									}
									counterTask->resultProfile.status = METERAGE_VALID ;
									counterTask->resultProfileReady = READY;
								}
								else
								{
									counterTask->resultProfile.status = METERAGE_NOT_PRESET ;
									counterTask->resultProfileReady = READY;
									counterPtr->lastTaskRepeateFlag = 1 ;
								}
							}
							break;

							case (TRANSACTION_METERAGE_DAY + TARIFF_1):
							case (TRANSACTION_METERAGE_DAY + TARIFF_2):
							case (TRANSACTION_METERAGE_DAY + TARIFF_3):
							case (TRANSACTION_METERAGE_DAY + TARIFF_4):
							{
								memcpy(&counterTask->resultDay.dts.d , &counterPtr->dayMetaragesLastRequestDts, sizeof(dateStamp));
								counterTask->resultDay.ratio = GUBANOV_METERAGES_RATIO_VALUE ;

								int aEnergyIndex = -1 ;
								int aEnergySize = 0 ;

								int rEnergyIndex = -1 ;
								int rEnergySize = 0 ;

								unsigned char tarifIndex = (transactionType - TRANSACTION_METERAGE_DAY);
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_METERAGE_DAY for tariff [%d] was catched" , tarifIndex);

								switch( counterPtr->type )
								{
									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										//
										// TODO
										//
									}
									break;

									case TYPE_GUBANOV_PSH_3TA07_DOT_1:
									{
										//
										// TODO
										//
									}
									break;

									case TYPE_GUBANOV_MAYK_102:
									{
										if ( counterTask->transactions[transactionIndex]->shortAnswerFlag == FALSE )
										{
											if ( answer[ 8 ] == 'Y' )
											{
												aEnergyIndex = 9 ;
												aEnergySize = 8 ;
											}
										}
										else
										{
											COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV transaction answer is short. Save empty meterage" );
										}
									}
									break;

									default:
									{
										transactionResult = TRANSACTION_DONE_ERROR_A ;
									}
									break;
								}

								if ( aEnergyIndex > 0 )
								{
									counterTask->resultDay.t[tarifIndex - 1].e[ENERGY_AP - 1] = COMMON_AsciiToInt( &answer[ aEnergyIndex ] , aEnergySize , 10 );
									if ( counterTask->resultDay.t[tarifIndex - 1].e[ENERGY_AP - 1] != 0 )
									{
										counterTask->resultDay.status = METERAGE_VALID ;
									}
								}

								if ( rEnergyIndex > 0 )
								{
									counterTask->resultDay.t[tarifIndex - 1].e[ENERGY_RP - 1] = COMMON_AsciiToInt( &answer[ rEnergyIndex ] , rEnergySize , 10 );
									if ( counterTask->resultDay.t[tarifIndex - 1].e[ENERGY_AP - 1] != 0 )
									{
										counterTask->resultDay.status = METERAGE_VALID ;
									}
								}

								resultMetrDayMsk |= (1 << (tarifIndex - 1) ) ;
								if ( resultMetrDayMsk == counterPtr->maskTarifs )
								{
									counterPtr->lastTaskRepeateFlag = 1 ;
									counterTask->resultDayReady = READY;
								}
							}
							break;


							case (TRANSACTION_METERAGE_MONTH + TARIFF_1):
							case (TRANSACTION_METERAGE_MONTH + TARIFF_2):
							case (TRANSACTION_METERAGE_MONTH + TARIFF_3):
							case (TRANSACTION_METERAGE_MONTH + TARIFF_4):
							{
								memcpy(&counterTask->resultMonth.dts.d , &counterPtr->monthMetaragesLastRequestDts, sizeof(dateStamp));
								counterTask->resultMonth.ratio = GUBANOV_METERAGES_RATIO_VALUE ;

								unsigned char tarifIndex = (transactionType - TRANSACTION_METERAGE_MONTH);
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_METERAGE_MONTH for tariff [%d] was catched" , tarifIndex);

								int aEnergyIndex = -1 ;
								int aEnergySize = 0 ;

								int rEnergyIndex = -1 ;
								int rEnergySize = 0 ;

								switch( counterPtr->type )
								{
									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV counter type is [%d]. Answer handler for ART" , counterPtr->type );
										//
										// TODO
										//
									}
									break;

									case TYPE_GUBANOV_PSH_3TA07_DOT_1:
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV counter type is [%d]. Answer handler for 3TA_1" , counterPtr->type );

										aEnergyIndex = 5 ;
										aEnergySize = 9 ; //drop down 1 number

										rEnergyIndex = -1 ;
										rEnergySize = 0 ;
									}
									break;

									case TYPE_GUBANOV_PSH_3TA07_DOT_2:
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV counter type is [%d]. Answer handler for 3TA_2" , counterPtr->type );

										aEnergyIndex = 5 ;
										aEnergySize = 8 ; //drop down 2 numbers

										rEnergyIndex = -1 ;
										rEnergySize = 0 ;
									}
									break;

									default:
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV counter type is [%d]. Answer handler for MAYK-102AT" , counterPtr->type );

										aEnergyIndex = 5 ;
										aEnergySize = 8 ;

										rEnergyIndex = -1 ;
										rEnergySize = 0 ;
									}
									break;
								}

								if ( aEnergyIndex > 0 )
								{
									counterTask->resultMonth.t[tarifIndex - 1].e[ENERGY_AP - 1] = COMMON_AsciiToInt( &answer[ aEnergyIndex ] , aEnergySize , 10 );
									if ( counterTask->resultMonth.t[tarifIndex - 1].e[ENERGY_AP - 1] != 0 )
									{
										counterTask->resultMonth.status = METERAGE_VALID ;
									}
								}

								if ( rEnergyIndex > 0 )
								{
									counterTask->resultMonth.t[tarifIndex - 1].e[ENERGY_RP - 1] = COMMON_AsciiToInt( &answer[ rEnergyIndex ] , rEnergySize , 10 );
									if ( counterTask->resultMonth.t[tarifIndex - 1].e[ENERGY_RP - 1] != 0 )
									{
										counterTask->resultMonth.status = METERAGE_VALID ;
									}
								}

								resultMetrMonthMsk |= (1 << (tarifIndex - 1) ) ;
								if ( resultMetrMonthMsk == counterPtr->maskTarifs )
								{
									counterPtr->lastTaskRepeateFlag = 1 ;
									counterTask->resultMonthReady = READY;
								}
							}
							break;

							case TRANSACTION_GET_EVENT:
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_GET_EVENT was cathed");

								switch(counterPtr->type)
								{
									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										//
										// TODO
										//
									}
									break;

									default:
									{
										if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == '\\')
										{
											uspdEvent_t event;
											memset(&event , 0 , sizeof(uspdEvent_t));

											unsigned int aIndex = GUBANOV_COMMAND_ANSWER_POS + 2;

											event.dts.t.h = COMMON_AsciiToInt( &answer[ aIndex ] , 2 , 10) ;
											event.dts.t.m = COMMON_AsciiToInt( &answer[ aIndex + 2 ] , 2 , 10) ;
											event.dts.t.s = COMMON_AsciiToInt( &answer[ aIndex + 4 ] , 2 , 10) ;
											event.dts.d.d = COMMON_AsciiToInt( &answer[ aIndex + 6 ] , 2 , 10) ;
											event.dts.d.m = COMMON_AsciiToInt( &answer[ aIndex + 8 ] , 2 , 10) ;
											event.dts.d.y = COMMON_AsciiToInt( &answer[ aIndex + 10] , 2 , 10) ;

											memset( &event.evDesc[0] , 0xFF , 6);
											switch( counterPtr->journalNumber )
											{
												case GUBANOV_SIMPLE_JOURNAL_EVENT_OPEN:
													event.evType = EVENT_OPENING_CONTACT ;
													break;

												case GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_ON:
													event.evType = EVENT_COUNTER_SWITCH_ON ;
													break;

												case GUBANOV_SIMPLE_JOURNAL_EVENT_POWER_OFF:
													event.evType = EVENT_COUNTER_SWITCH_OFF ;
													break;

												case GUBANOV_SIMPLE_JOURNAL_EVENT_TIME_SYNC:
													event.evType = EVENT_SYNC_TIME_HARD ;
													break;
												default:
													break;
											}

											if( (event.evType != 0 ) && ( COMMON_CheckDtsForValid(&event.dts) == OK ) )
											{
												if (STORAGE_IsEventInJournal(&event , counterTask->counterDbId) != TRUE)
												{
													COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Save event [%02X]" , event.evType );
													event.crc = STORAGE_CalcCrcForJournal(&event);
													STORAGE_SaveEvent(&event , counterTask->counterDbId);
												}
											}

											GUBANOV_SetNextJournalNumber( counterPtr );
										}
										else
										{
											COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV There is no journal sign '\\'");
											transactionResult = TRANSACTION_DONE_ERROR_A ;
										}
									}
									break;
								}
							}
							break;

							case TRANSACTION_GET_PQP_POWER_P_PHASE1:
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_GET_PQP_POWER_P was cathed");

								if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == '=')
								{
									int power = COMMON_AsciiToInt( &answer[ GUBANOV_COMMAND_ANSWER_POS + 1 ] , 4 , 10 );
									if ( (counterPtr->type != TYPE_GUBANOV_PSH_3TA07_DOT_2) ||
										 (counterPtr->type != TYPE_GUBANOV_PSH_3ART_D) )
									{
										power *= 10 ;
									}

									COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Active power is [%d]" , power);

									pqp.P.a = power ;
									pqp.P.sigma = power ;
									pqpOkFlag = 1 ;

								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_SET_DATE:
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_GUBANOV_SET_DATE was cathed");
								counterPtr->syncFlag += STORAGE_SYNC_DATE_SET ;
								counterPtr->lastTaskRepeateFlag = 1 ;
							}
							break;

							case TRANSACTION_GUBANOV_SET_TIME:
							{
								COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_GUBANOV_SET_TIME was cathed");
								counterPtr->syncFlag += STORAGE_SYNC_TIME_SET ;
								counterPtr->lastTaskRepeateFlag = 1 ;
							}
							break;

							case TRANSACTION_SYNC_SOFT:
							{
								if ( (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '0') &&
									 (answer[ GUBANOV_COMMAND_ANSWER_POS + 1 ] == '0'))
								{
									COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV TRANSACTION_SYNC_SOFT was cathed");

									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2 ] == 'Y' )
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV SYNC SOFT OK");
										STORAGE_SwitchSyncFlagTo_SyncDone(&counterPtr);
									}
									else
									{
										COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV CAN NOT SYNC SOFT");
										if( answer[ GUBANOV_COMMAND_ANSWER_POS + 3 ] == '1' ){
											COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV soft sync is impossible becouse 20 seconds" );
											if ( counterPtr->syncWorth >= 0 )
											{
												counterPtr->syncWorth = GUBANOV_SOFT_TIME_CORRECTION_DIFF + 1 ;
											}
											else
											{
												counterPtr->syncWorth = -1 * (GUBANOV_SOFT_TIME_CORRECTION_DIFF + 1) ;
											}
										}
										else
										{
											STORAGE_SwitchSyncFlagTo_SyncFail(&counterPtr);
										}
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_SET_POWER_LIMIT:
							{
								powerControlTransactionCounter++ ;
							}
							break;

							case TRANSACTION_GUBANOV_SET_MONTH_LIMIT:
							{
								if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == 'J')
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == 'Y' )
									{
										powerControlTransactionCounter++ ;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_SET_CONSUMER_CATEGORY:
							{
								if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == 'K')
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == 'Y' )
									{
										powerControlTransactionCounter++ ;
										tariffMapTransactionCounter++;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_RELE_CONTROL:
							{
								if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == 'G')
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == 'Y' )
									{
										powerControlTransactionCounter++ ;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;


							case TRANSACTION_GUBANOV_SET_DAY_TEMPLATE:
							{
								if ( (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '4') && (answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == '2') )
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2] == 'Y' )
									{
										tariffMapTransactionCounter++ ;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_SET_1_TARIFF_START_TIME:
							{
								if ( (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '0') && (answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == '1') )
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2] == 'Y' )
									{
										tariffMapTransactionCounter++ ;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_SET_2_TARIFF_START_TIME:
							{
								if ( (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '0') && (answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == '2') )
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2] == 'Y' )
									{
										tariffMapTransactionCounter++ ;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;


							case TRANSACTION_GUBANOV_SET_3_TARIFF_START_TIME:
							{
								if ( (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '0') && (answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == '3') )
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2] == 'Y' )
									{
										tariffMapTransactionCounter++ ;
									}
								}
								else
								{
									transactionResult = TRANSACTION_DONE_ERROR_A ;
								}
							}
							break;

							case TRANSACTION_GUBANOV_SET_HOLIDAY:
							{
								switch( counterTask->counterType )
								{
									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										if ( (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '1') && (answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == '4') )
										{
											if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2] == 'Y' )
											{
												tariffHolidaysTransactionCounter++ ;
											}
										}
										else
										{
											transactionResult = TRANSACTION_DONE_ERROR_A ;
										}
									}
									break;

									default:
									{
										if ( answer[ GUBANOV_COMMAND_ANSWER_POS ] == ']' )
										{
											if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == 'Y' )
											{
												tariffHolidaysTransactionCounter++ ;
											}
										}
										else
										{
											transactionResult = TRANSACTION_DONE_ERROR_A ;
										}
									}
									break;
								}
							}
							break;

							case TRANSACTION_WRITING_INDICATIONS:
							{
								switch( counterTask->counterType )
								{
									case TYPE_GUBANOV_PSH_3TA07_DOT_1:
									case TYPE_GUBANOV_PSH_3TA07_DOT_2:
									case TYPE_GUBANOV_PSH_3TA07A:
									case TYPE_GUBANOV_SEB_2A05:
									case TYPE_GUBANOV_SEB_2A07:
									case TYPE_GUBANOV_SEB_2A08:
									case TYPE_GUBANOV_MAYK_101:
									case TYPE_GUBANOV_MAYK_102:
									{
										if (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '^')
										{
											if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 1 ] == 'Y' )
											{
												if ( indicationWritingTransactionCounter >= 0 )
												{
													indicationWritingTransactionCounter++;
												}
											}
											else
											{
												indicationWritingTransactionCounter = -1 ;
											}
										}
									}
									break;

									case TYPE_GUBANOV_PSH_3ART:
									case TYPE_GUBANOV_PSH_3ART_1:
									case TYPE_GUBANOV_PSH_3ART_D:
									case TYPE_GUBANOV_MAYK_301:
									{
										if ((answer[ GUBANOV_COMMAND_ANSWER_POS ] == '1') && (answer[ GUBANOV_COMMAND_ANSWER_POS + 1] == '1'))
										{
											if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 2 ] == 'Y' )
											{
												if ( indicationWritingTransactionCounter >= 0 )
												{
													indicationWritingTransactionCounter++;
												}
											}
											else
											{
												indicationWritingTransactionCounter = -1 ;
											}
										}
									}
									break;

									default:
									{
										indicationWritingTransactionCounter = -1 ;
									}
									break;
								}								
							}
							break;

							case TRANSACTION_WRITING_LOOP_DURATION:
							{
								if (answer[ GUBANOV_COMMAND_ANSWER_POS ] == '^')
								{
									if ( answer[ GUBANOV_COMMAND_ANSWER_POS + 1 ] == 'Y' )
									{
										if ( indicationWritingTransactionCounter >= 0 )
										{
											indicationWritingTransactionCounter++;
										}
									}
									else
									{
										indicationWritingTransactionCounter = -1 ;
									}
								}
							}
							break;

							default:
							{
								transactionResult = TRANSACTION_DONE_ERROR_A ;
							}
							break;
						} //switch (transactionType)
					}
					else
					{
						COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV Error CRC" );
						transactionResult = TRANSACTION_DONE_ERROR_A ;
					}

					if ( answerSize > 0 )
					{
						COMMON_FreeMemory(&answer) ;
					}
				} // if ( transactionResult == TRANSACTION_DONE_OK )
				else
				{
					//if errprs in transaction reset repeat flags
					STORAGE_ResetRepeatFlag(counterTask->counterDbId);

					//set counter status with error
					STORAGE_SetCounterStatistic(counterTask->counterDbId, FALSE, transactionType, transactionResult);

					#if REV_COMPILE_EVENT_METER_FAIL
					if ( transactionResult == TRANSACTION_DONE_ERROR_SIZE )
					{
						unsigned char evDesc[EVENT_DESC_SIZE];
						unsigned char dbidDesc[EVENT_DESC_SIZE];
						memset(dbidDesc , 0, EVENT_DESC_SIZE);
						COMMON_TranslateLongToChar(counterTask->counterDbId, dbidDesc);
						COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_METERAGES_FAIL_SIZE, evDesc);
						STORAGE_JournalUspdEvent( EVENT_COUNTER_METERAGES_FAIL , evDesc );
					}
					else if ( transactionResult == TRANSACTION_DONE_ERROR_TIMEOUT )
					{
						unsigned char evDesc[EVENT_DESC_SIZE];
						unsigned char dbidDesc[EVENT_DESC_SIZE];
						memset(dbidDesc , 0, EVENT_DESC_SIZE);
						COMMON_TranslateLongToChar(counterTask->counterDbId, dbidDesc);
						COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_METERAGES_FAIL_TIMEOUT, evDesc);
						STORAGE_JournalUspdEvent( EVENT_COUNTER_METERAGES_FAIL , evDesc );
					}
					#endif

					//quit transaction results parse loop
					break;
				}
			} // for( )

		} // if ( counterPtr )
		else
		{
			COMMON_STR_DEBUG( DEBUG_GUBANOV "counter for this task became NULL" );
			return OK ;
		}
	} // if ( STORAGE_FoundCounter )

	//save statistic if is all ok
	if(transactionResult == TRANSACTION_DONE_OK){
		STORAGE_SetCounterStatistic(counterTask->counterDbId, FALSE, 0, TRANSACTION_COUNTER_POOL_DONE);
		res = OK;
	}

	//save results of its present
	if ((res == OK) && (counterTask->resultCurrentReady == READY)) {
		res = STORAGE_SaveMeterages(STORAGE_CURRENT, counterTask->counterDbId, &counterTask->resultCurrent);
	}

	if ((res == OK) && (counterTask->resultMonthReady == READY)) {
		res = STORAGE_SaveMeterages(STORAGE_MONTH, counterTask->counterDbId, &counterTask->resultMonth);
	}

	if ((res == OK) && (counterTask->resultDayReady == READY)) {		
		res = STORAGE_SaveMeterages(STORAGE_DAY, counterTask->counterDbId, &counterTask->resultDay);
	}

	if ((res == OK) && (counterTask->resultProfileReady == READY)) {
		res = STORAGE_SaveProfile(counterTask->counterDbId, &counterTask->resultProfile);
	}

	if ( (res == OK) && (pqpOkFlag = 1) )
	{
		if ( counterPtr )
		{
			memcpy( &pqp.dts , &counterPtr->currentDts , sizeof( dateTimeStamp ) );
		}
		else
		{
			dateTimeStamp currentDts;
			memset( &currentDts , 0 , sizeof(dateTimeStamp) );
			COMMON_GetCurrentDts( &currentDts );
			memcpy( &pqp.dts , &currentDts , sizeof( dateTimeStamp ) );
		}
		res = STORAGE_SavePQP(&pqp , counterTask->counterDbId);
	}

	if ( ( res == OK ) && ( counterTask->powerControlTransactionCounter != 0 ) && (transactionResult == TRANSACTION_DONE_OK))
	{
		if ( powerControlTransactionCounter == counterTask->powerControlTransactionCounter )
		{
			ACOUNT_StatusSet( counterPtr->dbId , ACOUNTS_STATUS_WRITTEN_OK , ACOUNTS_PART_POWR_CTRL );

			unsigned char evDesc[EVENT_DESC_SIZE];
			unsigned char dbidDesc[EVENT_DESC_SIZE];
			memset(dbidDesc , 0, EVENT_DESC_SIZE);
			COMMON_TranslateLongToChar(counterPtr->dbId, dbidDesc);
			COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_PSM_RULES_WRITING_OK, evDesc);
			STORAGE_JournalUspdEvent( EVENT_PSM_RULES_WRITING , evDesc );
		}
		else
		{
			ACOUNT_StatusSet( counterPtr->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_GENERAL , ACOUNTS_PART_POWR_CTRL );

			unsigned char evDesc[EVENT_DESC_SIZE];
			unsigned char dbidDesc[EVENT_DESC_SIZE];
			memset(dbidDesc , 0, EVENT_DESC_SIZE);
			COMMON_TranslateLongToChar(counterPtr->dbId, dbidDesc);
			COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_PSM_RULES_WRITING_ERR_GEN, evDesc);
			STORAGE_JournalUspdEvent( EVENT_PSM_RULES_WRITING , evDesc );
		}
	}

	if ( (res == OK) && ( (counterTask->tariffMapTransactionCounter > 0) || (counterTask->tariffHolidaysTransactionCounter > 0) ) && (transactionResult == TRANSACTION_DONE_OK) )
	{
		if ( (counterTask->tariffMapTransactionCounter == tariffMapTransactionCounter) &&
			 (counterTask->tariffHolidaysTransactionCounter == tariffHolidaysTransactionCounter) &&
			 (counterTask->indicationWritingTransactionCounter == indicationWritingTransactionCounter) )
		{
			ACOUNT_StatusSet( counterPtr->dbId , ACOUNTS_STATUS_WRITTEN_OK , ACOUNTS_PART_TARIFF );

			unsigned char evDesc[EVENT_DESC_SIZE];
			unsigned char dbidDesc[EVENT_DESC_SIZE];
			memset(dbidDesc , 0, EVENT_DESC_SIZE);
			COMMON_TranslateLongToChar(counterPtr->dbId, dbidDesc);
			COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_TARIFF_WRITING_OK, evDesc);
			STORAGE_JournalUspdEvent( EVENT_TARIFF_MAP_WRITING , evDesc );
		}
		else
		{
			ACOUNT_StatusSet( counterPtr->dbId , ACOUNTS_STATUS_WRITTEN_ERROR_GENERAL , ACOUNTS_PART_TARIFF );

			unsigned char evDesc[EVENT_DESC_SIZE];
			unsigned char dbidDesc[EVENT_DESC_SIZE];
			memset(dbidDesc , 0, EVENT_DESC_SIZE);
			COMMON_TranslateLongToChar(counterPtr->dbId, dbidDesc);
			COMMON_CharArrayDisjunction( (char *)dbidDesc , DESC_EVENT_TARIFF_WRITING_ERR_GEN, evDesc);
			STORAGE_JournalUspdEvent( EVENT_TARIFF_MAP_WRITING , evDesc );
		}
	}

	COMMON_STR_DEBUG( DEBUG_GUBANOV "GUBANOV_SaveTaskResults return %s" , getErrorCode(res));

	return res;
}

//
//
//

int GUBANOV_UnknownCounterIdentification( unsigned char * counterType , unsigned char * version )
{
	DEB_TRACE(DEB_IDX_GUBANOV)

	int res = ERROR_GENERAL ;

   char * psh_3ta_07_1[] = {"O6","M6","M7","M9","MA",NULL};
   char * psh_3ta_07_2[] = {"S6","S7","S9","SA",NULL};
   char * psh_3ta_07[] = {"J6","K6","K9","KA","K7","L6","L7","L9","LA","N6",NULL};
   char * seb_2a_07[] = { /* without -> */"H2","G4",/* <-profiles */      "H7","H8", "H6","H9","HA","G6","Q7","Q9","QA","HD","QD",NULL};

   char * seb_2a_08[] = {"P6","P7","P9","PA",NULL};
   //char * psh_3art[] = {"TA","TB","TD",NULL};
   char * psh_3art_1[] = {"UA","UB","UD",NULL};
   char * psh_3art_2[] = {"VA","VB","VD","WA","WB","WD",NULL};

   char * mayk_102[] = { "M2" , "M3" , NULL};
   char * mayk_301[] = {"TA","TB","TD",NULL};


   int searchIndex=0;
   //searching for PSH.3TA.07.1
   for( ; psh_3ta_07_1[searchIndex] ; searchIndex++ )
   {
	   if( strstr( psh_3ta_07_1[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_PSH_3TA07_DOT_1 ;
		   return OK;
	   }
   }

	//searching for PSH.3TA.07.2

	for(searchIndex = 0 ; psh_3ta_07_2[searchIndex] ; searchIndex++ )
	{
	   if( strstr( psh_3ta_07_2[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_PSH_3TA07_DOT_2;
		   return OK;
	   }
	}
	//searching for PSH.3TA.07
	for(searchIndex = 0 ; psh_3ta_07[searchIndex] ; searchIndex++ )
	{
	   if( strstr( psh_3ta_07[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_PSH_3TA07A;
		   return OK;
	   }
	}

	//searching for SEB.2A.07
	for(searchIndex = 0 ; seb_2a_07[searchIndex] ; searchIndex++ )
	{
	   if( strstr( seb_2a_07[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_SEB_2A07;
		   return OK;
	   }
	}

	//searching for SEB.2A.08
	for(searchIndex = 0 ; seb_2a_08[searchIndex] ; searchIndex++ )
	{
	   if( strstr( seb_2a_08[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_SEB_2A08;
		   return OK;
	   }
	}

	//searching for PSH.3ART
//	for(searchIndex = 0 ; psh_3art[searchIndex] ; searchIndex++ )
//	{
//	   if( strstr( psh_3art[searchIndex] , (char *)version ) )
//	   {
//		   (*counterType) = TYPE_GUBANOV_PSH_3ART;
//		   return OK;
//	   }
//	}

	//searching for PSH.3ART_1
	for(searchIndex = 0 ; psh_3art_1[searchIndex] ; searchIndex++ )
	{
	   if( strstr( psh_3art_1[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_PSH_3ART_1;
		   return OK;
	   }
	}

	//searching for PSH.3ART_2 ,3,4
	for(searchIndex = 0 ; psh_3art_2[searchIndex] ; searchIndex++ )
	{
	   if( strstr( psh_3art_2[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_PSH_3ART_D;
		   return OK;
	   }
	}

	//searching for MAYK 102
	for(searchIndex = 0 ; mayk_102[searchIndex] ; searchIndex++ )
	{
	   if( strstr( mayk_102[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_MAYK_102;
		   return OK;
	   }
	}

	//searching for MAYK 301
	for(searchIndex = 0 ; mayk_301[searchIndex] ; searchIndex++ )
	{
	   if( strstr( mayk_301[searchIndex] , (char *)version ) )
	   {
		   (*counterType) = TYPE_GUBANOV_MAYK_301;
		   return OK;
	   }
	}

	return res ;
}
//EOF



#ifndef APP_ALGO_ALGO_H
#define APP_ALGO_ALGO_H

#include <stdbool.h>
#include <stdint.h>
typedef struct
{
	int32_t x;
	int32_t y;
	int32_t z;
}RespAxesData_t;

typedef struct
{
	RespAxesData_t	axes;
	int32_t 		ppgSample;
	int32_t			envSample;
}RespInputData_t;

typedef struct
{
	int32_t 		resData;
	int32_t			errType;
}RespOutputData_t;

void RespRate_Init(void);
void RespRate_Input(RespInputData_t *pInputData);
void RespRate_Output(RespOutputData_t *pOutputData);
char* version_info_RspRate(void);

#endif /* APP_ALGO_ALGO_H_ */




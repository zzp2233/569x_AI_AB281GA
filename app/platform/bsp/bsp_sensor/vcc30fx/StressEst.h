
#ifndef __STRESSEST_H__
#define __STRESSEST_H__



#include <stdbool.h>
#include <stdint.h>


typedef struct
{
    int32_t         strData;
    int32_t         errType;
} StressOutputData_t;

void StressEst_Init(int32_t data0);
void StressEst_Input(int32_t data1,int32_t data2,int32_t data3,int32_t data4);
void StressEst_Output(StressOutputData_t *pOutputData);
char* version_info_StressEst(void);
#endif /* __STRESSEST_H__ */




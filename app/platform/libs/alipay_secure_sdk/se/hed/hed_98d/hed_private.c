/******************************************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 File name:       private.c
 Author:          sunll
 Version:         V1.0
 Date:            2023-12-15
 Description:     comm interface definition
 History:

******************************************************************************/
#include "include.h"
#include "hed_private.h"
#include "port_hed_i2c.h"
#include <stdint.h>
#include <string.h>
#if SECURITY_PAY_EN
#if (SECURITY_PAY_VENDOR == SECURITY_VENDOR_HED)
#define COMMAND_LIST_NUM 60
#define VAR 0xFF


uint8_t g_buf[2600] AT(.alipay.hed_HEDI2CBuf);  //���˿���buf

double_queue_node g_queue_in ;
double_queue_node g_queue_out ;

uint8_t* IIC_Master_Init(void)
{
    memset(g_buf, 0, 2600);
    return g_buf;
}
void HED_IIC_Init(void)
{
    g_queue_in.q_buf = IIC_Master_Init();
    g_queue_out.q_buf= g_queue_in.q_buf + DEQUE_MAX_SIZE ;
}

void hed_rst_init(void)
{
    PORT_I2C_SE0_RST_RESET();
}

/*************************************************
Function:     api_connect
Description:  SE����Ӧ��
Input:        out_buf         ATR����
              out_buf_len     ATR���ݳ���
Output:       no
Return:       �μ�error.h
Others:       no
*************************************************/
se_error_t api_connect (uint8_t *out_buf, uint32_t *out_buf_len)
{
    printf("%s\n", __func__);
    se_error_t ret = 0;
    //�����ж�
    if(out_buf==NULL||out_buf_len==NULL)
    {
        printf("failed to api_connect output params!");
        return SE_ERR_PARAM_INVALID;
    }
    //����acl��acl_connect�ӿ�
    ret = acl_connect(out_buf,out_buf_len);
    if(ret!=SE_SUCCESS)
    {
        printf("failed to call acl_connect!\n");
        return ret;
    };

    return SE_SUCCESS;
}

/*************************************************
Function:     api_transceive
Description:  SE�����Ӧ��
Input:        in_buf      APDU������
              in_buf_len  APDU����������
Output:       out_buf     APDU��Ӧ
              out_buf_len APDU��Ӧ���ݳ���
Return:       �μ�error.h
Others:       no
*************************************************/
se_error_t api_transceive(const uint8_t *in_buf, uint32_t in_buf_len, uint8_t *out_buf, uint32_t *out_buf_len)
{
    se_error_t ret = 0;

    //�����ж�
    if(in_buf==NULL||in_buf_len==0)
    {
        LOGE("failed to api_transceive input params!");
        return SE_ERR_PARAM_INVALID;
    }
    if(out_buf==NULL||out_buf_len==NULL)
    {
        LOGE("failed to api_transceive output params!");
        return SE_ERR_PARAM_INVALID;
    }
    //����acl��acl_transceive�ӿ�
    ret = acl_transceive(in_buf,in_buf_len,out_buf,out_buf_len);
    if(ret!=SE_SUCCESS)
    {
        LOGE("failed to call acl_transceive!\n");
        return ret;
    };

    return SE_SUCCESS;
}

/*************************************************
Function:     api_reset
Description:  SE��λӦ��
Input:        out_buf         ATR����
              out_buf_len     ATR���ݳ���
Output:       no
Return:       �μ�error.h
Others:       no
*************************************************/
se_error_t api_reset (uint8_t *out_buf, uint32_t *out_buf_len)
{
    se_error_t ret = 0;
    //�����ж�
    if(out_buf==NULL||out_buf_len==NULL)
    {
        LOGE("failed to api_reset output params!");
        return SE_ERR_PARAM_INVALID;
    }
    //����acl��acl_reset�ӿ�
    ret = acl_reset(out_buf,out_buf_len);
    if(ret!=SE_SUCCESS)
    {
        LOGE("failed to call acl_reset!\n");
        return ret;
    };

    return SE_SUCCESS;

}

//*************************************************
//info.c
//*************************************************
/*************************************************
Function:     api_sdk_version_number
Description:  ��ȡ����sdk�汾��
Input:        no
Output:       num ���Ͱ汾��
Return:       �μ�error.h
Others:       no
*************************************************/
se_error_t  api_sdk_version_number(uint32_t *num)
{

    //�������
    if(num==NULL)
    {
        LOGE("failed to api_sdk_version_number pointer params!\n");
        return SE_ERR_PARAM_INVALID;
    }
    *num = SDK_VERSION_NUM;

    return SE_SUCCESS;
}

/*************************************************
Function:     api_sdk_version_string
Description:  ��ȡ�ַ����汾��
Input:        no
Output:       str �ַ����汾��
Return:       �μ�error.h
Others:       no
*************************************************/
se_error_t  api_sdk_version_string (char *str)
{

    //�������
    if(str==NULL)
    {
        LOGE("failed to api_sdk_version_string pointer params!\n");
        return SE_ERR_PARAM_INVALID;
    }

    strcpy(str,SDK_VERSION_STRING);

    return SE_SUCCESS;
}
//*************************************************
//ctrl.c
//*************************************************
/*************************************************
Function:     api_switch_mode
Description:  �л�����ģʽӦ��
Input:        type         ����ģʽ
Output:       no
Return:       �μ�error.h
Others:       no
*************************************************/
//se_error_t api_switch_mode (work_mode  type)
//{
//  se_error_t ret = 0;
//  //�������
//  if(type!=STANDBY&&type!=POWERDOWN)
//  {
//      LOGE("failed to api_switch_mode input params!\n");
//      return SE_ERR_PARAM_INVALID;
//  }
//  //����cmd��apdu_switch_mode�ӿ�
//  ret = apdu_switch_mode (type);
//  if(ret!=SE_SUCCESS)
//  {
//      LOGE("failed to call apdu_switch_mode!\n");
//      return ret;
//  }

//  return SE_SUCCESS;

//}
/*************************************************
Function:     api_control
Description:  �������Ӧ��
Input:        ctrlcode        ��������
              in_buf       ��������
                  in_buf_len   �������ݳ���
Output:       out_buf      �������
              out_buf_len  ������ݳ���
Return:       �μ�error.h
Others:       no
*************************************************/
se_error_t api_control(ctrl_type ctrlcode, const uint8_t *in_buf, uint32_t in_buf_len, uint8_t *out_buf, uint32_t *out_buf_len)
{
    se_error_t ret = 0;
    //�������
    if(in_buf==NULL||in_buf_len==0)
    {
        LOGE("failed to api_control input params!\n");
        return SE_ERR_PARAM_INVALID;
    }
    if(out_buf==NULL||out_buf_len==NULL)
    {
        LOGE("failed to api_control output params!\n");
        return SE_ERR_PARAM_INVALID;
    }
    //����cmd��apdu_control�ӿ�
    ret = apdu_control(ctrlcode, in_buf, in_buf_len,out_buf,out_buf_len);
    if(ret!=SE_SUCCESS)
    {
        LOGE("failed to call apdu_control!\n");
        return ret;
    }

    return SE_SUCCESS;
}
//*************************************************
//tudu.c
//*************************************************
/*************************************************
    Function:    tpdu_init
    Description: ��������ͷ�Ӻ���
    Input:       isoCase  APDUЭ������
                 cla      CLA
    ins          INS
    p1           P1
    p2           P2
    lc           Lc
    le           LE
    Output:      command ����ṹ��
    Return:      �μ�������񷵻���
    Others:      no
*************************************************/
iso_command_apdu_t* tpdu_init(iso_command_apdu_t *command, int32_t isoCase, int32_t cla, int32_t ins,int32_t p1, int32_t p2,int32_t lc, int32_t le)
{
#ifdef _ASSERT_DEBUG
    ASSERT(command!=NULL);
#endif

    //��ֵAPDU������
    if ( isoCase >= 0 )
        command->isoCase = isoCase;
    if ( cla >= 0 )
        command->classbyte = cla;
    if ( ins >= 0 )
        command->instruction = ins;
    if ( p1  >= 0 )
        command->p1 = p1;
    if ( p2  >= 0 )
        command->p2 = p2;
    if ( lc  >= 0 )
        command->lc = lc;
    if ( le  >  0 )
        command->le = le;

    return command;
}

/*************************************************
    Function:     tpdu_init_with_id
    Description:  ��������ͷ��ʼ���������ͷ
    Input:        commandID ����ID��
    Output:       command ����ṹ��
    Return:       �μ�������񷵻���
    Others:       no
*************************************************/
iso_command_apdu_t* tpdu_init_with_id(iso_command_apdu_t *command, uint8_t commandID)
{
    typedef struct
    {
        uint8_t  isoCase;
        uint8_t  classbyte;
        uint8_t  instruction;
        uint8_t  p1;
        uint8_t  p2;
    } command_info_t;
    command_info_t *pc = NULL;
    //�����ʼ�������������������
    command_info_t command_list[COMMAND_LIST_NUM]=
    {
        {ISO_CASE_3, 0x80, 0x50, 0x00, 0x00},   //CMD_SCP_INIT_UPDATE
        {ISO_CASE_3, 0x84, 0x82, VAR, 0x00},    //CMD_SCP_AUTH
        {ISO_CASE_3, VAR, 0xD8, 0x00, 0x00},    //CMD_PUT_KEY
        {ISO_CASE_2, 0x00, 0x84, 0x00,0x00},    //CMD_GET_RANDOM
        {ISO_CASE_3, 0x00, 0xA4, 0x00,0x00},    //CMD_SELECT_FILE
        {ISO_CASE_3, VAR, 0xD6, VAR, VAR},      //CMD_WRITE_FILE
        {ISO_CASE_3, VAR, 0xB0, VAR, VAR},      //CMD_READ_FILE
        {ISO_CASE_2, 0x80,0xC8, VAR, 0x00},     //CMD_GET_INFO
        {ISO_CASE_3, 0x80, 0xE1,0x00,VAR},      //CMD_SWITCH_MODE
        {ISO_CASE_3, 0x80, 0xCE,VAR,0x00},      //CMD_CLEAR_FILE
    };
#ifdef _ASSERT_DEBUG
    ASSERT(command!=NULL);
    ASSERT(commandID<sizeof(command_list));
#endif
    pc = &command_list[commandID];
    //����command IDָ�ֵ��������
    tpdu_init(command,
              pc->isoCase,
              pc->classbyte,
              pc->instruction,
              pc->p1,
              pc->p2,
              -1,
              -1
             );

    return command;
}
/*************************************************
    Function:     tpdu_set_cla
    Description:  �޸�CLA
    Input:        cla      CLA
    Output:       command ����ṹ��
    Return:       �μ�������񷵻���
    Others:       no
*************************************************/
iso_command_apdu_t* tpdu_set_cla  (iso_command_apdu_t *command, uint32_t cla)
{
#ifdef _ASSERT_DEBUG
    ASSERT(command!=NULL);
#endif
    //����CLA����
    tpdu_init(command, -1, cla, -1, -1, -1,  -1, -1);
    return command;

}
/*************************************************
    Function:     tpdu_set_p1p2
    Description:  �޸�P1P2
    Input:        p1       P1
                  p2       P2
    Output:       command ����ṹ��
    Return:       �μ�������񷵻���
    Others:       no
*************************************************/
iso_command_apdu_t* tpdu_set_p1p2 (iso_command_apdu_t *command, uint32_t p1, uint32_t p2)
{
#ifdef _ASSERT_DEBUG
    ASSERT(command!=NULL);
#endif
    //����P1P2
    tpdu_init(command, -1, -1, -1, p1, p2, -1, -1);
    return command;
}
/*************************************************
    Function:     tpdu_set_le
    Description:  �޸�Le
    Input:        le       LE
    Output:       command ����ṹ��
    Return:       �μ�������񷵻���
    Others:       no
*************************************************/
iso_command_apdu_t* tpdu_set_le(iso_command_apdu_t *command, uint32_t le)
{
#ifdef _ASSERT_DEBUG
    ASSERT(command!=NULL);
#endif
    //����Le
    tpdu_init(command, -1, -1, -1, -1, -1, -1, le);
    return command;
}

/*************************************************
    Function:     tpdu_pack
    Description:  �Ӻ�������APDU����ṹ�����õ�˫�˶�����
    Input:        command����ṹ��
    Output:       output  �������
                  output_len  ������ݳ���
    Return:       �μ�������񷵻���
    Others:       no
*************************************************/
se_error_t tpdu_pack(iso_command_apdu_t *command, uint8_t *output, uint32_t *output_len)
{
    //output Ϊ˫�˶���
    uint8_t front_buffer[5] = {0};
    uint8_t rear_buffer[2] = {0};
    double_queue queue_out = (double_queue)output;

    //�����ж�
    if(command==NULL||output==NULL||output_len==NULL)
    {
        LOGE("failed to tpdu_pack input params!");
        return SE_ERR_PARAM_INVALID;
    }


    //���������������������˫�����
    front_buffer[0] = command->classbyte;
    front_buffer[1] = command->instruction;
    front_buffer[2] = command->p1;
    front_buffer[3] = command->p2;
    //����APDUЭ������ƴװ4������
    //ISO_CASE_1 = 0x01,  // CLA INS P1 P2 00
    //ISO_CASE_2 = 0x02,  // CLA INS P1 P2 Le
    //ISO_CASE_3 = 0x03,  // CLA INS P1 P2 Lc Data
    //ISO_CASE_4 = 0x04   // CLA INS P1 P2 Lc Data Le

    switch ( command->isoCase )
    {
        case ISO_CASE_1:
            front_buffer[4] = 0x00;
            queue_front_push(front_buffer,5, queue_out);
            *output_len = 5;
            break;

        case ISO_CASE_2:
            front_buffer[4] = (command->le)&0xFF;
            *output_len = 5;
            queue_front_push(front_buffer,5, queue_out);
            break;

        case ISO_CASE_3:
            front_buffer[4] = queue_size(queue_out)&0xFF;
            queue_front_push(front_buffer,5, queue_out);
            *output_len = queue_size(queue_out);
            break;

        case ISO_CASE_4:
            front_buffer[4] = queue_size(queue_out)&0xFF;
            *output_len = 5+queue_size(queue_out)+1;
            queue_front_push(front_buffer,5, queue_out);
            rear_buffer[1]=(command->le)&0xFF;
            queue_rear_push(rear_buffer,1, queue_out);
            break;
    }
    output = (uint8_t*)queue_out;

    return SE_SUCCESS;

}
/*************************************************
    Function:     tpdu_unpack
    Description:  �Ӻ��������״̬�ֺ�����
    Input:         no
    Output:        response-> queue_out   ���˫�˶���
                   response-> status_word ״̬��
    Return:        �μ�������񷵻���
    Others:        no
*************************************************/
se_error_t tpdu_unpack(uint8_t *output, uint32_t *output_len)
{
    double_queue queue_out=(double_queue)output;
    //�����ж�
    if(output==NULL||output_len==NULL)
    {
        LOGE("failed to tpdu_unpack output params!");
        return SE_ERR_PARAM_INVALID;
    }
    //ȥ��״̬��
    queue_rear_pop(2,queue_out);
    output = (uint8_t *)queue_out;
    *output_len = queue_out->q_buf_len;

    return SE_SUCCESS;
}

/*************************************************
    Function:      tpdu_send
    Description:   apdu����ͽ���
    Input:         in_buf      ��������
                   in_buf_len  �������ݳ���
    Output:        output  �������
    output_len     ������ݳ���
    Return:        �μ�������񷵻���
    Others:        no
*************************************************/
se_error_t tpdu_send(uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t *output_len)
{

    double_queue queue_in=(double_queue)input;
    double_queue queue_out=(double_queue)output;
    uint8_t temp_buffer[5]= {0x00};
    se_error_t ret = 0;
    uint32_t temp_len = 0;

    //�����ж�
    if(input==NULL||input_len==0)
    {
        LOGE("failed to tpdu_send input params!");
        return SE_ERR_PARAM_INVALID;
    }
    if(output==NULL||output_len==NULL)
    {
        LOGE("failed to tpdu_send output params!");
        return SE_ERR_PARAM_INVALID;
    }


    //���������61XX��6CXX���
    if(queue_in->q_buf_len==5)
    {
        memcpy(temp_buffer,queue_in->q_buf+queue_in->front_node,5);
    }

    while(1)
    {
        ret=acl_transceive_queue((uint8_t*)queue_in,queue_in->q_buf_len, (uint8_t*)queue_out,&temp_len);
        //����61XX��6CXX���
        if((temp_len==2)&&(queue_out->q_buf[queue_out->rear_node-2]==0x61))
        {
            memcpy(temp_buffer,"\x00\xC0\x00\x00",4);
            temp_buffer[4] = queue_out->q_buf[queue_out->rear_node-1];
            queue_init(queue_in);
            queue_init(queue_out);
            queue_rear_push(temp_buffer,5, queue_in);
            continue;
        }
        if((temp_len==2)&&(queue_out->q_buf[queue_out->rear_node-2]==0x6C))
        {
            temp_buffer[4] = queue_out->q_buf[queue_out->rear_node-1];
            queue_init(queue_in);
            queue_init(queue_out);
            queue_rear_push(temp_buffer,5, queue_in);
            continue;
        }
        break;
    }
    output = (uint8_t *)queue_out;
    *output_len = queue_out->q_buf_len;
    if(ret!=SE_SUCCESS)
        return ret;


    return SE_SUCCESS;
}
/*************************************************
    Function:      tpdu_execute
    Description:   apdu��������ͽ���
    Input:         command     command����ṹ��
                   in_buf      ��������
                   in_buf_len  �������ݳ���
    Output:        output      �������
                   output_len  ������ݳ���
    Return:        �μ�������񷵻���
    Others:        no
*************************************************/
se_error_t tpdu_execute (iso_command_apdu_t *command, uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t *output_len)
{

    se_error_t ret = 0;
    uint16_t status_word = 0;
    double_queue queue_in=(double_queue)input;
    double_queue queue_out=(double_queue)output;
    uint32_t in_len = 0;
    uint32_t out_len = 0;
    uint32_t off = 0;
    uint32_t change_status = 0;

    //�����ж�
    if(command==NULL||input==NULL)
    {
        LOGE("failed to tpdu_execute input params!");
        return SE_ERR_PARAM_INVALID;
    }
    if(output==NULL||output_len==NULL)
    {
        LOGE("failed to tpdu_execute output params!");
        return SE_ERR_PARAM_INVALID;
    }
    //��APDU�������˫�����
    ret = tpdu_pack(command,(uint8_t *) queue_in,&in_len);
    if(ret!=SE_SUCCESS)
        return ret;
    //���ͽ���APDU����
    ret = tpdu_send((uint8_t *) queue_in,queue_in->q_buf_len,(uint8_t *)queue_out,&out_len);
    //���жϷ����Ƿ���ȷ
    if(ret!=SE_SUCCESS)
    {
        output = (uint8_t *)queue_out;
        *output_len = out_len;
        return ret;
    }
    //��״̬��ȡ��
    off = queue_out->rear_node;
    status_word=((queue_out->q_buf[off-2]<< 8) & 0xFF00) | (queue_out->q_buf[off-1] & 0xFF);
    //6310��Ҫ�����Ƚ����ݷ���
    if(status_word==0x6310)
    {
        change_status = tpdu_change_error_code(status_word);
        ret = tpdu_unpack((uint8_t *)queue_out,&out_len);
        output = (uint8_t *)queue_out;
        *output_len = out_len;
        return change_status;
    }
    //�������أ�ֱ�ӷ��ش����룬��ȷ��������ȥ״̬��
    if(status_word!=0x9000)
        return tpdu_change_error_code(status_word);

    //ͨ���ҷ��ض���������״̬�ִ�������ȥ������������Ч����
    ret = tpdu_unpack((uint8_t *)queue_out,&out_len);
    if(ret!=SE_SUCCESS)
        return ret;
    output = (uint8_t *)queue_out;
    *output_len = out_len;

    return SE_SUCCESS;

}
/*************************************************
    Function:      tpdu_execute_no_response
    Description:   apdu��������ͽ��գ��޷�������
    Input:         command     command����ṹ��
                   in_buf      ��������
                   in_buf_len  �������ݳ���
    Output:        no
    Return:        �μ�������񷵻���
    Others:        no
*************************************************/
se_error_t tpdu_execute_no_response (iso_command_apdu_t *command, uint8_t *input, uint32_t input_len)
{

    se_error_t ret = 0;
    uint16_t status_word = 0;
    double_queue queue_in=(double_queue)input;
    //double_queue_node queue_out={0};
    uint32_t in_len = 0;
    uint32_t out_len = 0;
    uint32_t off = 0;

    //�����ж�
    if(command==NULL||input==NULL)
    {
        LOGE("failed to tpdu_execute_no_response input params!");
        return SE_ERR_PARAM_INVALID;
    }
    //��APDU�������˫�����
    queue_init(&g_queue_out);
    ret = tpdu_pack(command,(uint8_t *) queue_in,&in_len);
    if(ret!=SE_SUCCESS)
        return ret;
    //����APDU����޷���
    ret = tpdu_send((uint8_t *) queue_in,queue_in->q_buf_len,(uint8_t *)&g_queue_out,&out_len);
    off = g_queue_out.front_node;
    status_word=((g_queue_out.q_buf[off]<< 8) & 0xFF00) | (g_queue_out.q_buf[off+1] & 0xFF);
    //�жϷ����Ƿ���ȷ
    if(ret!=SE_SUCCESS)
        return ret;
    //�ж�״̬���Ƿ���ȷ
    if(status_word!=0x9000)
        return tpdu_change_error_code(status_word);

    return SE_SUCCESS;

}


/*************************************************
    Function:     tpdu_change_error_code
    Description:  ״̬��ת��
    Input:        status_word Դ״̬��
    Output:       no
    Return:       �μ�������񷵻���
    Others:       no
*************************************************/
se_error_t tpdu_change_error_code(uint16_t status_word)
{
    se_error_t error = (se_error_t)(status_word&(0x0000FFFF));
    //״̬��װ�����μ�error.h
    if(status_word==0x9000)
        return SE_SUCCESS;
    else
        return (error|(0x10000000));

}
//*************************************************
//apdu.c
//*************************************************
/*************************************************
    Function:     apdu_switch_mode
    Description:  �л�����ģʽ�����װ
    Input:        type         ����ģʽ
    Output:       no
    Return:       �μ�error.h
    Others:       no
*************************************************/
se_error_t apdu_switch_mode (work_mode  type)
{
    iso_command_apdu_t command = {0};
    se_error_t ret = 0;
    //double_queue_node queue_in ={0} ;
    uint32_t p2 = 0;
    if(type!=STANDBY&&type!=POWERDOWN)
    {
        LOGE("failed to apdu_switch_mode input params!\n");
        return SE_ERR_PARAM_INVALID;
    }

    //˫�˶��г�ʼ��
    queue_init(&g_queue_in);
    //��������ͷ
    tpdu_init_with_id(&command,CMD_SWITCH_MODE);
    //����P1P2
    p2 = (type==STANDBY)?0x00:0x02;
    tpdu_set_p1p2 (&command,0x00,p2);

    ret = tpdu_execute_no_response(&command, (uint8_t *)&g_queue_in, queue_size (&g_queue_in));
    if(ret!=SE_SUCCESS)
        return ret;


    return SE_SUCCESS;
}

/*************************************************
    Function:     apdu_control
    Description:  ������������װ
    Input:        ctrlcode     ��������
                  in_buf       ��������
                  in_buf_len   �������ݳ���
    Output:       out_buf      �������
                  out_buf_len  ������ݳ���
    Return:       �μ�error.h
    Others:       no
*************************************************/
se_error_t apdu_control(ctrl_type ctrlcode, const uint8_t *in_buf, uint32_t in_buf_len, uint8_t *out_buf, uint32_t *out_buf_len)
{
    return SE_SUCCESS;
}

/*********************************************************************************
Function:       acl_connect
Description:    �������輰��ȡ����ATR
            1. ������������ѡ���������������ֻע����һ�����裬
            ��δѡ������裬���ص�һ��ע����豸
            2. ���ݼ��ص���������������init ��ʼ��������open����
Input:      no
Output:     out_buf �������ʼ��ַ
            out_buf_len  ������ݳ��ȵ���ʼ��ַ
Return:         ��������״̬��
Others:         no
*********************************************************************************/
se_error_t acl_connect (uint8_t *out_buf, uint32_t *out_buf_len)
{
    se_error_t ret_code = SE_SUCCESS;
    peripheral *periph = NULL;
    printf("%s\n", __func__);
    do
    {
        if ((out_buf == NULL) || (out_buf_len == NULL))
        {
            ret_code = SE_ERR_PARAM_INVALID;
            break;
        }

        //3.���豸
        ret_code = proto_i2c_open(periph, out_buf, out_buf_len);  //���豸,��ȡATR
        if(ret_code != SE_SUCCESS)
        {
            break;
        }

    }
    while(0);

    return ret_code;
}

/******************************************************************
Function:       acl_transceive_queue
Description:    ��˫����и�ʽ������豸�������������Ӧ
                1.��������Ч��
                2.������������������豸���
                3.ͨ�����صľ��������proto��transceiver�������������ݼ�����������Ӧ
Input:     in_buf ����˫����е���ʼ��ַ
           in_buf_len ����˫����е��������ݳ���
Output:     out_buf ���˫����е���ʼ��ַ
           out_buf_len  ���˫����е�������ݳ��ȵ���ʼ��ַ
Return:        ��������״̬��
Others:        no
******************************************************************/
se_error_t acl_transceive_queue(uint8_t *in_buf, uint32_t in_buf_len, uint8_t *out_buf, uint32_t *out_buf_len)
{
    printf("%s\n", __func__);
    se_error_t ret_code = SE_SUCCESS;
    peripheral *periph = NULL;

    do
    {
        //������
        if ((in_buf == NULL)||(in_buf_len == 0U) || (out_buf == NULL)||(out_buf_len==NULL))
        {
            ret_code = SE_ERR_PARAM_INVALID;
            break;
        }



        //---����proto�е�proto_spi_transceive��proto_i2c_transceive,
        //��ѡ����豸�������������������Ӧ����
        ret_code = proto_i2c_transceive(periph, in_buf, in_buf_len, out_buf, out_buf_len);
        if(ret_code != SE_SUCCESS)
        {
            break;
        }

    }
    while(0);

    return ret_code;
}


/******************************************************************
Function:       acl_transceive
Description:    ���������ݴ洢��˫����У��������豸�������ݼ�������Ӧ������˫�����ȡ����Ӧ����
                1.����洢�浵������˫�����
                2.����acl_transceive_queue ��������˫����и�ʽ���ͺͽ�������
                3.�����˫�����ȡ����Ӧ����
Input:     in_buf �������ݵ���ʼ��ַ
           in_buf_len �������ݳ���
Output:    out_buf �������ʼ��ַ
           out_buf_len  ������ݳ��ȵ���ʼ��ַ
Return:    ��������״̬��
Others:    no
******************************************************************/
se_error_t acl_transceive(const uint8_t *in_buf, uint32_t in_buf_len, uint8_t *out_buf, uint32_t *out_buf_len)
{
    se_error_t ret_code = SE_SUCCESS;
    uint32_t out_len = 0;
    uint16_t front_node = 0;

    //double_queue_node queue_in ={0} ;
    //double_queue_node queue_out ={0} ;

    do
    {
        //������
        if ((in_buf == NULL) || (out_buf == NULL)||(out_buf_len==NULL))
        {
            ret_code = SE_ERR_PARAM_INVALID;
            break;
        }

        if ((in_buf_len<ACL_TRANS_DATA_LEN_MIN)||(in_buf_len>ACL_TRANS_DATA_LEN_MAX))
        {
            ret_code = SE_ERR_PARAM_INVALID;
            break;
        }

        //˫�˶��г�ʼ��
        queue_init(&g_queue_in);
        queue_init(&g_queue_out);

        //�������ݴ���˫�˶���
        queue_rear_push((uint8_t *)in_buf,in_buf_len, &g_queue_in);

        //������˫������е����ݰ�Э���ʽ���͸��豸��������Ӧ���ݴ洢�����˫�����
        ret_code = acl_transceive_queue((uint8_t *)&g_queue_in, queue_size(&g_queue_in), (uint8_t *)&g_queue_out, &out_len);
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }

        //��˫�˶��п������������
        front_node = g_queue_out.front_node;
        memcpy(out_buf,&g_queue_out.q_buf[front_node],g_queue_out.q_buf_len);
        *out_buf_len = g_queue_out.q_buf_len;

    }
    while(0);

    return ret_code;
}


/*************************************************
Function:      queue_init
Description:   ˫�˶��г�ʼ��
Input:         no
Output:        d_queue     ˫�˶��нṹ��
Return:        �μ�������񷵻���
Others:        no
*************************************************/
double_queue queue_init(double_queue d_queue)
{
    d_queue ->q_buf_len =0;
    d_queue ->capacity=DEQUE_MAX_SIZE;
    d_queue ->front_node=20;
    d_queue ->rear_node =20;
    memset(d_queue->q_buf,0x00,DEQUE_MAX_SIZE);
    return d_queue;
}

/*************************************************
Function:      queue_front_push
Description:   ˫�˶���ͷ��������
Input:         in_buf       ��������
               in_buf_len   �������ݳ���
Output:        d_queue      ˫�˶��нṹ��
Return:        no
Others:        no
*************************************************/
void queue_front_push(uint8_t*in_buf,uint32_t in_buf_len, double_queue d_queue)
{
    uint32_t off = 0;

    if(d_queue ->front_node==0)
    {

        d_queue ->front_node = d_queue ->capacity- in_buf_len;

    }
    else
    {
        d_queue ->front_node-= in_buf_len;
    }
    off = d_queue ->front_node;
    memcpy(d_queue->q_buf+off, in_buf, in_buf_len);
    d_queue ->q_buf_len += in_buf_len;

}

/*************************************************
Function:      queue_front_pop
Description:   ˫�˶���ͷɾ������
Input:         in_buf_len   ɾ�����ݳ���
Output:        d_queue      ˫�˶��нṹ��
Return:        no
Others:        no
*************************************************/
void queue_front_pop(uint32_t in_buf_len,double_queue d_queue)
{
    //uint8_t item[DEQUE_MAX_SIZE] = {0x00};
    // uint32_t off = d_queue->front_node;
    if(d_queue ->front_node ==(d_queue ->capacity- in_buf_len))
    {
        //memcpy(item,d_queue->q_buf,in_buf_len);
        d_queue ->front_node =0;

    }
    else
    {
        //memcpy(item,d_queue->q_buf+off,in_buf_len);
        d_queue ->front_node += in_buf_len;
    }

    d_queue ->q_buf_len -= in_buf_len;

    return;
}


/*************************************************
Function:      queue_rear_push
Description:   ˫�˶���β��������
Input:         in_buf       ��������
               in_buf_len   �������ݳ���
Output:        d_queue      ˫�˶��нṹ��
Return:        no
Others:        no
*************************************************/
void queue_rear_push(uint8_t*in_buf,uint32_t in_buf_len, double_queue d_queue)
{
    uint32_t off = d_queue->rear_node;
    memcpy(d_queue->q_buf+off, in_buf, in_buf_len);
    if(d_queue ->rear_node== d_queue ->capacity- in_buf_len)
    {
        d_queue ->rear_node = 0;
    }
    else
    {
        d_queue ->rear_node += in_buf_len;
    }


    d_queue ->q_buf_len += in_buf_len;

}

/*************************************************
Function:      queue_rear_pop
Description:   ˫�˶���βɾ������
Input:         in_buf_len   ɾ�����ݳ���
Output:        d_queue      ˫�˶��нṹ��
Return:        ɾ��������
Others:        no
*************************************************/
void queue_rear_pop(uint32_t in_buf_len,double_queue d_queue)
{
    //uint8_t item[DEQUE_MAX_SIZE] = {0x00};
//        uint32_t off = d_queue->rear_node;
    if(d_queue ->rear_node == 0)
    {
        //off=d_queue ->capacity-in_buf_len;
        //memcpy(item,d_queue->q_buf+off,in_buf_len);
        d_queue ->capacity-= in_buf_len;
    }
    else
    {
        //memcpy(item,d_queue->q_buf+off,in_buf_len);
        d_queue ->rear_node -= in_buf_len;
    }

    d_queue ->q_buf_len -= in_buf_len;

    return;
}

/*************************************************
Function:      queue_rear_pop
Description:   ˫�˶���βɾ������
Input:         in_buf_len   ɾ�����ݳ���
Output:        d_queue      ˫�˶��нṹ��
Return:        no
Others:        no
*************************************************/
uint32_t queue_size (double_queue d_queue)
{
    if(d_queue ->rear_node> d_queue ->front_node)
    {
        return (d_queue ->rear_node- d_queue ->front_node);
    }
    else
    {
        return (d_queue ->front_node- d_queue ->rear_node);
    }
}

//***************************************************
//proto_i2c_impl.c
//***************************************************
/*************************************************
  Function:   proto_i2c_crc16
  Description:  ����ָ������ָ�����ݵ�CRC���
  Input:
            CRCType��CRC������sing
            Length��Ҫ��������ݳ���
            Data��Ҫ��������ݵ���ʼ��ַ
  Return:   crc������
  Others:
*************************************************/
uint16_t proto_i2c_crc16(uint32_t CRCType, uint32_t Length, uint8_t *Data)
{
    uint8_t chBlock = 0;
    uint16_t wCrc = 0;

    wCrc = (CRCType == CRC_A) ? 0x6363 : 0xFFFF;    // CRC_A : ITU-V.41 , CRC_B : ISO 3309

    do
    {
        chBlock = *Data++;
        chBlock = (chBlock^(uint8_t)(wCrc & 0x00FF));
        chBlock = (chBlock^(chBlock<<4));
        wCrc= (wCrc >> 8)^((uint16_t)chBlock << 8)^((uint16_t)chBlock<<3)^((uint16_t)chBlock>>4);
    }
    while (--Length);

    if (CRCType != CRC_A)
    {
        wCrc = ~wCrc; // ISO 3309
    }

    return wCrc;
}


/******************************************************************
Function:       porto_i2c_queue_in
Description:    ����HED I2CЭ���ʽ����˫�����������֡ͷ��֡β����
        1.��˫�����ͷ����� NAD, PCB , LEN
        2.��˫�����β�����CRC
Input:    periph �豸���
          inbuf ����˫����е���ʼ��ַ
          inbuf_len ����˫����е��������ݳ���
Output:       no
Return:       ��������״̬��
Others:        no
******************************************************************/
se_error_t porto_i2c_queue_in(uint8_t* inbuf, uint16_t inbuf_len)
{
    se_error_t ret_code = SE_SUCCESS;
    double_queue queue_in = (double_queue)inbuf;

    uint8_t i2c_data[4] = {0};
    uint8_t offset = 0;
    uint16_t i2c_crc = 0;

    do
    {

        if ((inbuf == NULL) ||(inbuf_len == 0U))
        {
            ret_code =  SE_ERR_PARAM_INVALID;
            break;
        }

        //��˫����зֱ����NAD, PCB, LENH,LENL
        i2c_data[offset++] = PROTO_I2C_NAD;
        i2c_data[offset++] = PROTO_I2C_PCB_I_BLOCK;
        i2c_data[offset++] = inbuf_len>>8;
        i2c_data[offset++] = inbuf_len;
        queue_front_push(i2c_data, offset, queue_in);

        //����CRC16������˫�������� CRC
        i2c_crc = proto_i2c_crc16(CRC_B,queue_size(queue_in), &queue_in->q_buf[queue_in->front_node]);  //4 //���� CRC
        queue_rear_push((uint8_t *)&i2c_crc, 2, queue_in);

    }
    while(0);

    return ret_code;
}


/******************************************************************
Function:       porto_i2c_send_iblock
Description:    ͨ��I2C �ӿڷ���I ������
        1.ͨ��port���豸ע��ĺ����б�ָ�룬����port��i2c�ӿڵ�
            ���ͺ���transmit������I ������
Input:    periph �豸���
          inbuf ����˫����е���ʼ��ַ
          inbuf_len ����˫����е��������ݳ���
Output:       no
Return:       ��������״̬��
Others:        no
******************************************************************/
se_error_t porto_i2c_send_iblock(peripheral *periph, uint8_t* inbuf, uint16_t inbuf_len)
{
    printf("%d%s\n", __LINE__, __func__);
    se_error_t ret_code = SE_SUCCESS;

    do
    {
        if((inbuf == NULL) ||(inbuf_len == 0U))
        {
            ret_code = SE_ERR_PARAM_INVALID;
            break;
        }

        hal_delay(PROTO_I2C_RECEIVE_TO_SEND_BGT);

        //��������
        ret_code = port_i2c_periph_transmit(inbuf, inbuf_len);

    }
    while(0);

    return ret_code;

}

/******************************************************************
Function:       porto_i2c_send_lblock
Description:    ͨ��I2C �ӿڷ���L , R��ATR ��
        1.���ݿ����ͣ���֯���ʽ��֡����
        2.ͨ��port���豸ע��ĺ����б�ָ�룬����port��i2c�ӿڵ�
            ���ͺ���transmit�����Ϳ�����
Input:    periph   �豸���
          block_type  ������
          data_len   ������ΪL ��ʱ����ʾ����Ҫ���͵�I������ݳ��ȣ�
          ������Ϊ������ʱ����ֵΪ0
Output:       no
Return:       ��������״̬��
Others:        no
******************************************************************/
se_error_t porto_i2c_send_block(uint8_t block_type, uint16_t data_len)
{
    se_error_t ret_code = SE_SUCCESS;
    uint16_t i2c_crc = 0;
    uint8_t i2c_buf[PROTO_I2C_LR_FRAME_LENGTH] = {0};
    uint8_t offset=0;


    //ƴ�ӷ�������
    i2c_buf[offset++] = PROTO_I2C_NAD;
    i2c_buf[offset++] = block_type;
    i2c_buf[offset++] = data_len>>8;
    i2c_buf[offset++] = data_len;

    //����CRC16
    i2c_crc = proto_i2c_crc16(CRC_B,offset, i2c_buf);   //4 // NAD PCB LEN1 LEN2 CRC
    i2c_buf[offset++] = i2c_crc;
    i2c_buf[offset++] = i2c_crc>>8;

    ret_code = port_i2c_periph_transmit(i2c_buf, offset);

    return ret_code;
}


/******************************************************************
Function:       porto_i2c_send_lblock
Description:    ͨ��I2C �ӿڷ���L ������
Input:    periph �豸���
          data_len L ��ĳ���ֵ������Ҫ���͵�I������ݳ���
Output:   no
Return:   ��������״̬��
Others:   no
******************************************************************/
se_error_t porto_i2c_send_lblock(peripheral *periph, uint16_t data_len)
{
    hal_delay(PROTO_I2C_RECEIVE_TO_SEND_BGT);
    return porto_i2c_send_block(PROTO_I2C_PCB_L_BLOCK, data_len);
}


/******************************************************************
Function:       porto_i2c_send_rblock
Description:    ͨ��I2C �ӿڷ���R ������
Input:    periph �豸���
Output:   no
Return:   ��������״̬��
Others:   no
******************************************************************/
se_error_t porto_i2c_send_rblock(peripheral *periph)
{
    hal_delay(PROTO_I2C_RECEIVE_TO_SEND_BGT);
    return porto_i2c_send_block(PROTO_I2C_PCB_R_BLOCK, 0);
}


/******************************************************************
Function:       porto_i2c_send_atr_block
Description:    ͨ��I2C �ӿڷ���ATR �����
Input:    periph �豸���
Output:   no
Return:   ��������״̬��
Others:   no
******************************************************************/
se_error_t porto_i2c_send_atr_block(peripheral *periph)
{
    hal_delay(PROTO_I2C_RECEIVE_TO_SEND_BGT);
    return porto_i2c_send_block(PROTO_I2C_PCB_ATR_BLOCK, 0);
}


/******************************************************************
Function:       porto_i2c_receive_block
Description:    ͨ��I2C �ӿڽ��տ�����
                1.ͨ��port���豸ע��ĺ����б�ָ�룬����port��i2c�ӿڵ�
            ���պ���receive�����տ�����
Input:        periph �豸���
Output:       rbuf ���տ����ݵĴ洢��ַ
              rlen ����Ϣ�ĳ���
Return:       ��������״̬��
Others:        no
******************************************************************/
se_error_t porto_i2c_receive_block(uint8_t *rbuf, uint16_t rlen)
{
    printf("%d%s\n", __LINE__, __func__);
    se_error_t ret_code = SE_SUCCESS;
    i2c_timer_t timer = {0};
    uint8_t crc_err_count=0;
    uint16_t crc_value = 0;

    if((rbuf == NULL)||(rlen < PROTO_I2C_FRAME_LENGTH_MINI))
    {
        return SE_ERR_PARAM_INVALID;
    }

    //���ý�������ʱ֡�ȴ��ĳ�ʱʱ��
    timer.interval = PROTO_I2C_RECEVIE_FRAME_WAIT_TIME;
    timer.start = hal_systick();

    do
    {
        do
        {
            hal_delay(PROTO_I2C_RECEVIE_POLL_TIME);   //delay poll time

            ret_code = port_i2c_periph_receive(rbuf, rlen);
            if(ret_code == SE_SUCCESS)
            {
                break;
            }

            if(i2c_time_get_diff(hal_systick(), timer.start) >= timer.interval)
            {
                LOGE("Failed:receive frame overtime,  ErrCode-%08X.", SE_ERR_TIMEOUT);
                return SE_ERR_TIMEOUT;
            }

        }
        while(1);


        if(rbuf[PROTO_I2C_PCB_OFFSET] == PROTO_I2C_PCB_I_BLOCK)
        {
            //compare CRC16
            crc_value = proto_i2c_crc16(CRC_B, rlen -PROTO_I2C_CRC_LENGTH, rbuf);
            if((rbuf[rlen -PROTO_I2C_CRC_LENGTH]==(crc_value&0xff))&&(rbuf[rlen-PROTO_I2C_CRC_LENGTH+1]==crc_value>>8))
            {
                return ret_code;  //receive data ok
            }
        }
        else if((rbuf[PROTO_I2C_PCB_OFFSET]  == PROTO_I2C_PCB_R_BLOCK) ||
                (rbuf[PROTO_I2C_PCB_OFFSET]  == PROTO_I2C_PCB_R_NAK_BLOCK) ||
                (rbuf[PROTO_I2C_PCB_OFFSET]  == PROTO_I2C_PCB_L_BLOCK) ||
                (rbuf[PROTO_I2C_PCB_OFFSET]  == PROTO_I2C_PCB_S_BLOCK))
        {
            //compare CRC16
            crc_value = proto_i2c_crc16(CRC_B, PROTO_I2C_LR_FRAME_LENGTH -PROTO_I2C_CRC_LENGTH, rbuf);
            if((rbuf[PROTO_I2C_LR_FRAME_LENGTH -PROTO_I2C_CRC_LENGTH]==(crc_value&0xff))&&(rbuf[PROTO_I2C_LR_FRAME_LENGTH-PROTO_I2C_CRC_LENGTH+1]==crc_value>>8))
            {
                return ret_code;  //receive data ok
            }
        }
        else if(rbuf[PROTO_I2C_PCB_OFFSET] == 0xFF)
        {
            continue;
        }
        else
        {
            printf("%d%s\n", __LINE__, __func__);
            return SE_ERR_PARAM_INVALID;
        }

        crc_err_count++;  //crc  err
    }
    while(crc_err_count <= PROTO_I2C_RETRY_NUM);

    return SE_ERR_LRC_CRC;
}



/******************************************************************
Function:       porto_i2c_ receive_lrblock
Description:    ͨ��I2C �ӿڽ���L���R�飬L�鳤����Ч��R�鳤��Ϊ0
                1.����porto_i2c_receive_block ��������L�顢R ��� S ��
                2.����������
Input:    periph �豸���
Output:   block_type ����������
          block_len ������ΪL�飬��ʾ ����Ҫ���� I  ����Ϣ�ĳ���
Return:       ��������״̬��
Others:        no
******************************************************************/
se_error_t porto_i2c_receive_lrblock(peripheral *periph, uint8_t *block_type, uint16_t *block_len)
{
    printf("%s\n", __func__);
    se_error_t ret_code = SE_SUCCESS;
    uint8_t i2c_buf[PROTO_I2C_LR_FRAME_LENGTH];
    uint8_t nad = 0;
    uint8_t pcb = 0;
    uint16_t rec_wtx_count = 0;

    do
    {
        ret_code = porto_i2c_receive_block(i2c_buf, PROTO_I2C_LR_FRAME_LENGTH);
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }

        nad = i2c_buf[PROTO_I2C_NAD_OFFSET];
        pcb = i2c_buf[PROTO_I2C_PCB_OFFSET];

        //the NAD recieved is the NAD sended whose high-low bit is changed
        if(((nad>>4)&0x0f)!=(PROTO_I2C_NAD&0x0f) ||
           ((nad<<4)&0xf0)!=(PROTO_I2C_NAD&0xf0))
        {
            return SE_ERR_PARAM_INVALID;
        }

        if((pcb == PROTO_I2C_PCB_R_BLOCK) ||(pcb == PROTO_I2C_PCB_R_NAK_BLOCK) || (pcb == PROTO_I2C_PCB_L_BLOCK))
        {
            *block_type = pcb;
            break;
        }

        else if(pcb == PROTO_I2C_PCB_S_BLOCK)   //S��  ��Ҫ���¼�ʱ����������
        {
            if(rec_wtx_count>=PROTO_I2C_WTX_NUM)
            {
                return SE_ERR_COMM;    //����WTX����֡����20�ν����ش����Է�ֹSE����Ϸ����������޷��˳�ѭ��
            }
            rec_wtx_count++;
            continue;
        }

        else
        {
            return SE_ERR_PARAM_INVALID;
        }
//      break;
    }
    while(1);

    *block_len = i2c_buf[PROTO_I2C_LEN_OFFSET]*256 + i2c_buf[PROTO_I2C_LEN_OFFSET+1];      //cal output len
    printf("i2c_buf\n");
    print_r(i2c_buf, PROTO_I2C_LR_FRAME_LENGTH);
    printf("block_len: %d\n", *block_len);

    return  ret_code;
}



/******************************************************************
Function:       proto_i2c_receive_iblock
Description:    ͨ��I2C �ӿڽ���I �����ݻ�R��
                1.����porto_i2c_receive_block ��������I �����ݻ�R ��
Input:    periph �豸���
          data_len ������Ϣ�����ݵĳ���
Output:   block_type ����������
          outbuf ���˫����е���ʼ��ַ
Return:   ��������״̬��
Others:   no
******************************************************************/
se_error_t proto_i2c_receive_irblock(peripheral *periph, uint8_t *block_type, uint8_t *outbuf, uint16_t data_len)
{
    se_error_t ret_code = SE_SUCCESS;
    uint8_t nad = 0;
    uint8_t pcb = 0;
    uint16_t  receive_len = 0;

    if((outbuf == NULL)||(data_len == 0U))
    {
        return SE_ERR_PARAM_INVALID;
    }

    do
    {
        ret_code = porto_i2c_receive_block(outbuf, data_len+PROTO_I2C_FRAME_LENGTH_MINI);
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }

        nad = outbuf[PROTO_I2C_NAD_OFFSET];
        pcb = outbuf[PROTO_I2C_PCB_OFFSET];

        //the NAD recieved is the NAD sended whose high-low bit is changed
        if(((nad>>4)&0x0f)!=(PROTO_I2C_NAD&0x0f) ||
           ((nad<<4)&0xf0)!=(PROTO_I2C_NAD&0xf0))
        {
            return SE_ERR_PARAM_INVALID;
        }

        //����ӦΪI ��� NAK�飬���򱨴�
        if(pcb == PROTO_I2C_PCB_R_NAK_BLOCK)
        {
            *block_type = pcb;
            return ret_code;
        }
        else if(pcb == PROTO_I2C_PCB_I_BLOCK)
        {
            *block_type = pcb;
            break;
        }
        else
        {
            return SE_ERR_PARAM_INVALID;
        }

    }
    while(0);

    receive_len = outbuf[PROTO_I2C_LEN_OFFSET]*256+outbuf[PROTO_I2C_LEN_OFFSET+1];
    if(data_len != receive_len) //L�鳤����I�鳤�Ȳ�һ��
    {
        return SE_ERR_LEN;
    }

    return ret_code;
}

/*********************************************************************************
Function:       proto_i2c_handle
Description:    ����HED I2C ͨ��Э���֡��ʽ��SE�����շ����ݽ������ڼ����
              ����֡���ͼ�LRCУ��ȣ�������ʱ֡��NAK֡������ʱ֡�ط�����
                      ��Ҫ���������º���:
              1. proto_spi_send_frame
              2. proto_spi_receive_frame_head
              3. proto_spi_receive_frame_data
Input:          handle �豸���
           input  ��������ָ��
           input_len �������ݳ���
Output:    output������ݴ洢��ַ
           outputlen������ݳ���
Return:         no
Others:         no
*********************************************************************************/
se_error_t proto_i2c_handle(peripheral *periph, uint8_t* inbuf, uint16_t inbuf_len, uint8_t* outbuf, uint16_t* outbuf_len)
{
    printf("%s\n", __func__);
    se_error_t ret_code = SE_SUCCESS;
    uint16_t block_len = 0;
    uint8_t nak_count = 0;
    uint8_t block_type = 0;

    do
    {
        if(nak_count > PROTO_I2C_NAK_NUM)
        {
            printf("%d%s\n", __LINE__, __func__);
            return SE_ERR_LRC_CRC;
        }
        nak_count++;

        ret_code = porto_i2c_send_lblock(periph, inbuf_len);//����L��
        if(ret_code != SE_SUCCESS)
        {
            printf("%d%s  ret_code: %d\n", __LINE__, __func__, ret_code);
            return ret_code;
        }

        ret_code = porto_i2c_receive_lrblock(periph, &block_type, &block_len);  //����R��
        printf("%d%s  ret_code: %d, block_type: %d\n", __LINE__, __func__, ret_code, block_type);
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }

    }
    while(block_type == PROTO_I2C_PCB_R_NAK_BLOCK);

    nak_count = 0;
    do
    {
        if(nak_count > PROTO_I2C_NAK_NUM)
        {
            printf("%d%s\n", __LINE__, __func__);
            return SE_ERR_LRC_CRC;
        }
        nak_count++;
        print_r(inbuf, inbuf_len+PROTO_I2C_FRAME_LENGTH_MINI);
        ret_code = porto_i2c_send_iblock(periph, inbuf, inbuf_len+PROTO_I2C_FRAME_LENGTH_MINI); //����I��
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }

        ret_code = porto_i2c_receive_lrblock(periph, &block_type, &block_len);  //����L�� �� R ��

        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }
    }
    while(block_type == PROTO_I2C_PCB_R_NAK_BLOCK);

    if((block_type==PROTO_I2C_PCB_L_BLOCK) && (block_len!=0))
    {
        //��ȷ�յ�L��,����R��
        nak_count = 0;
        do
        {
            if(nak_count > PROTO_I2C_NAK_NUM)
            {
                return SE_ERR_LRC_CRC;
            }
            nak_count++;

            //��ȷ�յ�L��,����R��
            ret_code = porto_i2c_send_rblock(periph);
            if(ret_code != SE_SUCCESS)
            {
                return ret_code;
            }

            ret_code = proto_i2c_receive_irblock(periph, &block_type, outbuf, block_len);//����I���R��
            if(ret_code != SE_SUCCESS)
            {
                return ret_code;
            }
        }
        while(block_type == PROTO_I2C_PCB_R_NAK_BLOCK);
    }
    else
    {
        //Ӧ���յ�L�飬ȴ�յ���R��
        return SE_ERR_COMM;
    }

    *outbuf_len = block_len+PROTO_I2C_FRAME_LENGTH_MINI;
    return ret_code;
}



/***********************************************************************
Function:       proto_i2c_get_atr
Description:   SPIͨ��Э��֮���豸�Ĳ���
                     1. ����proto_i2c_send_atr_block��������SE����ATR����顣
                     2. ����proto_i2c_receive_lrblock����L���R�顣
                     3. �����յ�ΪNAK��R�飬��������proto_i2c_send_atr_block������
                            ��ATR����飬������proto_i2c_receive_lrblock��������L���R�顣
                     4. �����յ�I�飬����proto_i2c_receive_iblock��������ATR���ݡ�
Input:        periph �豸���
Output:      rbuf  ������ATR����ʼ��ַ
             rlen  ������ATR�ĳ���
Return:      ��������״̬��
Others:      no
************************************************************************/
se_error_t proto_i2c_get_atr(peripheral *periph, uint8_t *rbuf, uint32_t *rlen)
{
    printf("%s\n", __func__);
    se_error_t ret_code = SE_SUCCESS;
    uint16_t block_len = 0;
    uint8_t nak_count = 0;
    uint8_t block_type = 0;


    if ((rbuf == NULL) ||(rlen == NULL))
    {
        return SE_ERR_PARAM_INVALID;
    }

    do
    {
        if(nak_count > PROTO_I2C_NAK_NUM)
        {
            return SE_ERR_LRC_CRC;
        }
        nak_count++;

        ret_code = porto_i2c_send_atr_block(periph);  //����ATR �����
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }

        ret_code = porto_i2c_receive_lrblock(periph, &block_type, &block_len);  //���� L �� ��R ��
        if(ret_code != SE_SUCCESS)
        {
            return ret_code;
        }
    }
    while(block_type == PROTO_I2C_PCB_R_NAK_BLOCK);

    if((block_type==PROTO_I2C_PCB_L_BLOCK) && (block_len!=0))
    {
        if(block_len>(PROTO_I2C_ATR_MAX_LEN+PROTO_I2C_FRAME_LENGTH_MINI))
        {
            return SE_ERR_LEN;
        }
        nak_count = 0;
        do
        {
            if(nak_count > PROTO_I2C_NAK_NUM)
            {
                return SE_ERR_LRC_CRC;
            }
            nak_count++;

            //��ȷ�յ�L��,����R��
            ret_code = porto_i2c_send_rblock(periph);
            if(ret_code != SE_SUCCESS)
            {
                return ret_code;
            }

            ret_code = proto_i2c_receive_irblock(periph, &block_type, rbuf, block_len);//����I��( ATR ֵ)��R��
            if(ret_code != SE_SUCCESS)
            {
                return ret_code;
            }

        }
        while(block_type == PROTO_I2C_PCB_R_NAK_BLOCK);

    }
    else
    {
        //Ӧ���յ�L�飬ȴ�յ���R��
        return SE_ERR_COMM;
    }

    *rlen = block_len;

    return ret_code;
}


/***********************************************************************
Function:       proto_i2c_open
Description:   SPIͨ��Э��֮���豸�Ĳ���
                 1. ͨ��port���豸ע��ĺ����б�ָ�룬����port���lock��������
                 2. ͨ��port���豸ע��ĺ����б�ָ�룬����port��i2c�ӿڵĺ���power_on���豸�ϵ�
                 3. ͨ��port���豸ע��ĺ����б�ָ�룬����port��i2c�ӿڵĺ������豸��λ
                 4. ����proto_i2c_get_atr��������ȡATR��
                 5. ͨ��port���豸ע��ĺ����б�ָ�룬����unlock����������
Input:        periph �豸���
Output:      rbuf  ������ATR����ʼ��ַ
             rlen  ������ATR�ĳ���
Return:      ��������״̬��
Others:      no
************************************************************************/
se_error_t proto_i2c_open(peripheral *periph, uint8_t *rbuf, uint32_t *rlen)
{
    printf("%s\n", __func__);
    se_error_t ret_code = SE_SUCCESS;
    i2c_timer_t timer = {0};
    uint8_t atr_buf[PROTO_I2C_ATR_MAX_LEN+PROTO_I2C_FRAME_LENGTH_MINI] = {0};


    if ((rbuf == NULL) || (rlen == NULL))
    {
        return  SE_ERR_PARAM_INVALID;
    }

    //�������ȴ��ĳ�ʱʱ��
    timer.interval = PROTO_I2C_COMM_MUTEX_WAIT_TIME;
    timer.start = hal_systick();

    do
    {
        if(i2c_time_get_diff(hal_systick(), timer.start) >= timer.interval)
        {
            ret_code = SE_ERR_TIMEOUT;
            LOGE("Failed:open periph mutex,  ErrCode-%08X.", ret_code);
            break;
        }


        ret_code = proto_i2c_get_atr(periph, atr_buf, rlen);
        if(ret_code == SE_ERR_BUSY)
        {
            continue;
        }
        else if(ret_code != SE_SUCCESS)
        {
            LOGE("Failed:protocol get atr,  ErrCode-%08X.", ret_code);
            break;
        }

        memcpy(rbuf, atr_buf+PROTO_I2C_DATA_OFFSET, *rlen);
        LOGI("Open Periph Success!");
        break;

    }
    while(1);


    return ret_code;
}

/******************************************************************
Function:       proto_i2c_transceive
Description:    ͨ��I2C �ӿ�����豸������������������Ӧʱ�����ô˺���
            1. ͨ��port���豸ע��ĺ����б�ָ�룬����port���lock��������
            2������ HED_I2C ͨ��Э�飬����proto_i2c_handle���������ͼ�����Э��֡���ݡ�
            3��ͨ��port���豸ע��ĺ����б�ָ�룬����unlock����������
Input:     periph �豸���
           sbuf ����˫����е���ʼ��ַ
           slen ����˫����е��������ݳ���
Output:    rbuf ���˫����е���ʼ��ַ
           rlen  ���˫����е�������ݳ��ȵ���ʼ��ַ
Return:        ��������״̬��
Others:  no
******************************************************************/
se_error_t proto_i2c_transceive(peripheral *periph, uint8_t *sbuf,  uint32_t  slen, uint8_t *rbuf, uint32_t *rlen)
{
    printf("%s\n", __func__);
    se_error_t ret_code = SE_SUCCESS;
//  se_error_t ret_code_bak = SE_SUCCESS;
    i2c_timer_t timer = {0};
    double_queue queue_in = (double_queue)sbuf;
    double_queue queue_out = (double_queue)rbuf;
    uint16_t send_len = 0;
    uint16_t rec_len = 0;
    uint8_t* p_output = NULL;
    uint8_t* p_input = NULL;

    if ((sbuf == NULL) || (rbuf == NULL) || (slen == 0U) || (rlen == NULL))
    {
        return  SE_ERR_PARAM_INVALID;
    }

    printf("%d%s\n", __LINE__, __func__);
    print_r(sbuf, slen);
    //׼���շ�������
    send_len = (uint16_t)slen;
    ret_code =porto_i2c_queue_in(sbuf, slen);
    if(ret_code != SE_SUCCESS)
    {
        return ret_code;
    }

    p_input = &queue_in->q_buf[queue_in->front_node];
    p_output = &queue_out->q_buf[queue_out->front_node];

    //�������ȴ��ĳ�ʱʱ��
    timer.interval = PROTO_I2C_COMM_MUTEX_WAIT_TIME;
    timer.start = hal_systick();

    do
    {
        if(i2c_time_get_diff(hal_systick(), timer.start) >= timer.interval)
        {
            ret_code = SE_ERR_TIMEOUT;
            LOGE("Failed:communication mutex,  ErrCode-%08X.\n", ret_code);
            break;
        }

        printf("%d%s\n", __LINE__, __func__);
        print_r(p_input, send_len);
        //����I2C  ���ݵ��շ�
        ret_code = proto_i2c_handle(periph, p_input, send_len, p_output,&rec_len);
        if(ret_code == SE_ERR_BUSY)
        {
            continue;
        }

        else if(ret_code == SE_ERR_LRC_CRC)
        {
            printf("Failed:check crc,  ErrCode-%08X.\n", ret_code);
            //����SE ��RST���Ž��и�λ����
            //ret_code_bak = p_i2c_periph->control(p_i2c_periph, PROTO_I2C_CTRL_RST, NULL, NULL);
            //if(ret_code_bak != SE_SUCCESS)
            //{
            //  LOGE("Failed:protocol communication control,  ErrCode-%08X.", ret_code_bak);
            //}
            //p_i2c_periph->power_off(p_i2c_periph);
            break;
        }

        else if(ret_code != SE_SUCCESS)
        {
            printf("Failed:protocol communication,  ErrCode-%08X.", ret_code);
            break;
        }

        queue_out->q_buf_len = rec_len;
        queue_out->rear_node =  queue_out->front_node + rec_len;

        queue_front_pop(PROTO_I2C_NAD_PCB_LEN_LENGTH, queue_out); //�Ƴ�NAD, PCB, LEN
        queue_rear_pop(PROTO_I2C_CRC_LENGTH, queue_out);//�Ƴ�CRC
        *rlen = queue_size(queue_out);
        printf("Communication Success!");
        break;
    }
    while(1);

    return ret_code;
}

#endif
#endif

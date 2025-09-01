#include "config.h"
depend(0x01010200);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
setspace(FLASH_CM_SIZE);
setuserbin(FLASH_UI_BASE, FLASH_UI_SIZE, ui.bin, 1);   #��ʼ��ַҪ��ui.xmͬ��
make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, updater.bin);
save(dcf_buf, app.dcf);


//------------------------app.xm_cmd------------------------
//**setflash(param0, param1, param2, param3, param4);
//˵����     �����û���Դ��
//param0:    type:Ĭ��1������flash
//param1:    FLASH_SIZE:flash��С
//param2:    FLASH_ERASE_4K:�Ƿ�֧��4K����
//param3:    FLASH_DUAL_READ:�Ƿ�֧�����߶�ȡ
//param4:    FLASH_QUAD_READ:�Ƿ�֧��4�߶�ȡ
//ʾ����     setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ)

//**setspace(param0)
//˵����    ���ò�������С
//param0:   size:��������С
//ʾ����    setspace(0x5000)

//**setuserbin(param0,param1,param2,param3)
//˵����     �����û���Դ��
//param0:    start_addr:�û���Դ����ʼ��ַ
//param1:    bin_size:�û���Դ����С
//param2:    bin_file:�û���Դ�ļ�
//param3:    type:0:����fot�ļ�������Դ 1������fot�ļ�Я��ѹ����Դ
//                2:����fotЯ����Դ����Դ��ѹ��
//ʾ����     setuserbin(0x87000,0x304000, ui.bin,1);

//**setunpack(param0)
//˵����          ����FOTAѹ����������loader
//param0:file:    ͬһĿ¼��bin�ļ�
//ʾ����          setunpack(unpack.bin)

//**setpkgarea(param0,param1)
//˵����     ����FOTAѹ������ѹ���������Ϣ
//param0:    start_addr:ѹ���������ʼ��ַ
//param1:    end_addr:ѹ������С
//ʾ����     setunpack(0x400000,0x300000)
//ѹ���������ʼ��ַΪ0x400000,���Ϊ0x300000
//------------------------app.xm_cmd------------------------



#define FLASH_STORAGE_GLOBAL
#define FLASH_STORAGE_ENABLE 1
#define FLASH_STORAGE_SD_ENABLE 1
#include "global.h"
#include "flash_storage.h"
//#include "nrf_fstorage.h"
#include "nrf_atomic.h"

/* �����ļ�
   name						 path								 brief
nrf_numc.c  			 modules\nrfx\hal  						 NVMC����
nrf_fstorage.c   	 components\libraries\fstorage				FS�����ļ�
nrf_fstorage_nvmc.c  components\libraries\fstorage 	   FS���nrf_fstorage_nvmc�ļ�(���)
nrf_fstorage_sd.c    components\libraries\fstorage	FS���nrf_fstorage_sd�ļ�(Softdevice)
nrf_atomic			 components\libraries\atomic	   C11��׼stdatomic.h��API
*/

#if FLASH_STORAGE_ENABLE

#if FLASH_STORAGE_NVMC_ENABLE
#include "nrf_nvmc.h"
#include "nrf_fstorage_nvmc.h"
#else if FLASH_STORAGE_SD_ENABLE
#include "nrf_fstorage_sd.h"
#endif

NRF_FSTORAGE_DEF(nrf_fstorage_t my_fs) = 
{
  .evt_handler = fstorage_evt_handler,
  .start_addr = 0x3e000,			//fsʹ��flash�ռ��6ҳ
  .end_addr = 0x3ffff,
};

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
  if(p_evt->result != NRF_SUCCESS)
  	{
  		NRF_LOG_INFO("func:%s error code:%s",__func__,NRF_LOG_ERROR_STRING_GET(p_evt->result));
  	}
  switch(p_evt->id)
  	{
  		case NRF_FSTORAGE_EVT_ERASE_RESULT:
			NRF_LOG_INFO("erase success");
			NRF_LOG_INFO("address:0x%x length:%d",p_evt->len,p_evt->addr);
			break;
		case NRF_FSTORAGE_EVT_WRITE_RESULT:
			NRF_LOG_INFO("writed success");
			NRF_LOG_INFO("address:0x%x length:%d",p_evt->len,p_evt->addr);
			break;
		case NRF_FSTORAGE_EVT_READ_RESULT:	//���¼��ж���ȴ���ᴥ��
			NRF_LOG_INFO("read success");
			NRF_LOG_INFO("address:0x%x length:%d",p_evt->len,p_evt->addr);
			break;
		default:
			NRF_LOG_INFO("default");
  	}
}
/**********************************************************
��������void flash_storage_init(void)
���룺
�����
���ã���ʼ��FS����ģ��
**********************************************************/
void flash_storage_init(void)
{
  ret_code_t err_code;
  nrf_fstorage_api_t *p_fs_api;

  //����Ҫʹ�õ�API, ���ʹ��nvmc,softderiveʹ�� sd
#if FLASH_STORAGE_NVMC_ENABLE
  p_fs_api = &nrf_fstorage_nvmc;
#else if FLASH_STORAGE_SD_ENABLE
  p_fs_api = &nrf_fstorage_sd;
#endif

//  nrf5_flash_end_addr_get();
  
  err_code = nrf_fstorage_init(&my_fs, p_fs_api,NULL);
  G_CHECK_ERROR_CODE_INFO(err_code);
}
/**********************************************************
��������ret_code_t flash_storage_write(nrf_fstorage_t const* p_fs,uint32_t dest,void const* p_src,
								uint32_t len,void *p_param)
���룺p_fs��ָ��fsʵ������ʹ�ã�dest:Ŀ���ַ��p_src��Դbuff,len:���ݳ��ȣ�p_param����ѡ����������null
�����
����ֵ���������
���ã�flash д������д�볤����Ҫʱ��̵�Ԫ���� 4�ı��� �ᴥ�� writed event
**********************************************************/
ret_code_t flash_storage_write(nrf_fstorage_t const* p_fs,uint32_t dest,void const* p_src,
								uint32_t len,void *p_param)
{
	
	return nrf_fstorage_write(&my_fs,dest,p_src,len,p_param);
}
/**********************************************************
��������ret_code_t flash_storage_read(nrf_fstorage_t const* p_fs,uint32_t addr,void *p_dest,uint32_t len)
���룺p_fs��ָ��fsʵ������ʹ�ã�addr:flashĿ���ַ; p_dest����������buff,len:���ݳ���;
�����
����ֵ���������
���ã�flash ��������
**********************************************************/
ret_code_t flash_storage_read(nrf_fstorage_t const* p_fs,uint32_t addr,void *p_dest,uint32_t len)
{
	return nrf_fstorage_read(&my_fs,addr,p_dest,len);
}

/**********************************************************
��������ret_code_t flash_storage_erase(nrf_fstorage_t const* p_fs,uint32_t page_addr,uint32_t len,void *p_param)
���룺p_fs��ָ��fsʵ������ʹ�ã�page_addr:ҳ��ַ; len:    ҳ��; p_param����ѡ����������null
�����
�����
����ֵ���������
���ã�flash ������������λʱһҳ���ᴥ�� erase event
**********************************************************/
ret_code_t flash_storage_erase(nrf_fstorage_t const* p_fs,uint32_t page_addr,uint32_t len,void *p_param)
{
	return nrf_fstorage_erase(&my_fs,page_addr,len,p_param);
}



#endif
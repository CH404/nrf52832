#define FLASH_STORAGE_GLOBAL
#define FLASH_STORAGE_ENABLE 1
#define FLASH_STORAGE_SD_ENABLE 1
#include "global.h"
#include "flash_storage.h"
//#include "nrf_fstorage.h"
#include "nrf_atomic.h"

/* 所需文件
   name						 path								 brief
nrf_numc.c  			 modules\nrfx\hal  						 NVMC驱动
nrf_fstorage.c   	 components\libraries\fstorage				FS驱动文件
nrf_fstorage_nvmc.c  components\libraries\fstorage 	   FS后端nrf_fstorage_nvmc文件(裸机)
nrf_fstorage_sd.c    components\libraries\fstorage	FS后端nrf_fstorage_sd文件(Softdevice)
nrf_atomic			 components\libraries\atomic	   C11标准stdatomic.h简化API
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
  .start_addr = 0x3e000,			//fs使用flash空间第6页
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
		case NRF_FSTORAGE_EVT_READ_RESULT:	//读事件有定义却不会触发
			NRF_LOG_INFO("read success");
			NRF_LOG_INFO("address:0x%x length:%d",p_evt->len,p_evt->addr);
			break;
		default:
			NRF_LOG_INFO("default");
  	}
}
/**********************************************************
函数名：void flash_storage_init(void)
输入：
输出：
作用：初始化FS程序模块
**********************************************************/
void flash_storage_init(void)
{
  ret_code_t err_code;
  nrf_fstorage_api_t *p_fs_api;

  //配置要使用的API, 裸机使用nvmc,softderive使用 sd
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
函数名：ret_code_t flash_storage_write(nrf_fstorage_t const* p_fs,uint32_t dest,void const* p_src,
								uint32_t len,void *p_param)
输入：p_fs：指向fs实例，无使用，dest:目标地址，p_src：源buff,len:数据长度，p_param：可选参数无用置null
输出：
返回值：操作结果
作用：flash 写操作，写入长度需要时编程单元倍数 4的倍数 会触发 writed event
**********************************************************/
ret_code_t flash_storage_write(nrf_fstorage_t const* p_fs,uint32_t dest,void const* p_src,
								uint32_t len,void *p_param)
{
	
	return nrf_fstorage_write(&my_fs,dest,p_src,len,p_param);
}
/**********************************************************
函数名：ret_code_t flash_storage_read(nrf_fstorage_t const* p_fs,uint32_t addr,void *p_dest,uint32_t len)
输入：p_fs：指向fs实例，无使用；addr:flash目标地址; p_dest：读出数据buff,len:数据长度;
输出：
返回值：操作结果
作用：flash 读操作，
**********************************************************/
ret_code_t flash_storage_read(nrf_fstorage_t const* p_fs,uint32_t addr,void *p_dest,uint32_t len)
{
	return nrf_fstorage_read(&my_fs,addr,p_dest,len);
}

/**********************************************************
函数名：ret_code_t flash_storage_erase(nrf_fstorage_t const* p_fs,uint32_t page_addr,uint32_t len,void *p_param)
输入：p_fs：指向fs实例，无使用；page_addr:页地址; len:    页数; p_param：可选参数无用置null
输出：
输出：
返回值：操作结果
作用：flash 擦除操作，单位时一页，会触发 erase event
**********************************************************/
ret_code_t flash_storage_erase(nrf_fstorage_t const* p_fs,uint32_t page_addr,uint32_t len,void *p_param)
{
	return nrf_fstorage_erase(&my_fs,page_addr,len,p_param);
}



#endif
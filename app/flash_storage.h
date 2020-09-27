//FS: Flash Storage
//�����ڴ洢������Ŀ���٣����ݱ䶯Ƶ�ʺܵ͵ĳ���ʹ��

#ifndef __FLASH_STORAGE_H
#define __FLASH_STORAGE_H

#ifdef FLASH_STORAGE_GLOBAL
#define FLASH_STORAGE_EXT
#else
#define FLASH_STORAGE_EXT extern
#endif

#ifdef __cplusplus
extern "C"{
#endif
#include "nrf_fstorage.h"

void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
FLASH_STORAGE_EXT void flash_storage_init(void);

FLASH_STORAGE_EXT ret_code_t flash_storage_write(nrf_fstorage_t const* p_fs,uint32_t dest,void const* p_src,
								uint32_t len,void *p_param);
FLASH_STORAGE_EXT ret_code_t flash_storage_read(nrf_fstorage_t const* p_fs,uint32_t addr,void *p_dest,uint32_t len);
FLASH_STORAGE_EXT ret_code_t flash_storage_erase(nrf_fstorage_t const* p_fs,uint32_t page_addr,uint32_t len,void *p_param);




#ifdef __cplusplus
}
#endif




#endif


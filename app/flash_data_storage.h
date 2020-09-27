#ifndef __FLASH_DATA_STORAGE_H
#define __FLASH_DATA_STORAGE_H

#include <stdint.h>

#include "nrf_nvmc.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"
#include "nrf_fstorage_sd.h"
#include "nrf_atomic.h"
#include "crc16.h"
#include "nrf_atfifo.h"
#include "fds.h"
#ifdef FLASH_DATA_STORAGE_GLOBAL
#define FLASH_DATA_STORAGE_EXT
#else
#define FLASH_DATA_STORAGE_EXT extern 
#endif

#ifdef __cpulspuls
extern "C"{
#endif


typedef struct
{
	char hw_version[12];	 
	char sw_version[12];    
}__attribute__((aligned(4)))device_version_t;
typedef struct
{
	uint32_t size;
}__attribute__((aligned(4)))fw_information_t;



//fds 的init,write,update,delete,gc等function都是异步的的
//完成后才会发生event


#define FW_INFORMATION_KEY 0x11		//KEY 不能是0x0000
#define DEVICE_VERSION_KEY 0x22
#define DEVICE_FILE 0x33			//file id 不能是0xFFFF
#define DEVICE_FILE1 0x34			//file id 不能是0xFFFF


static bool fds_init_compelet;

FLASH_DATA_STORAGE_EXT bool FDS_busy;


void fds_evt_handler(fds_evt_t const * p_fds_evt);

ret_code_t test_fds_record_write(fds_record_desc_t * const p_desc,fds_record_t const * const p_record);
void test_fds_record_read(fds_record_desc_t * const p_desc, fds_flash_record_t * const p_flash_rec);
void test_fds_file_delete(uint16_t file_id);
void test_fds_record_gc(void);
void test_fds_file_delete(uint16_t file_id);
void test_fds_record_delete(fds_record_desc_t * const p_desc);
void test_fds_record_update(fds_record_desc_t * p_desc, fds_record_t const * p_record);
void test_fds_record_find(uint16_t                  file_id,
                           uint16_t                  record_key,
                           fds_record_desc_t * const p_desc,
                           fds_find_token_t  * const p_token);
void test_fds_main(void);






#ifdef __cpulspuls
}
#endif

#endif


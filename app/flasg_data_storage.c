#define FLASH_DATA_STORAGE_GLOBAL
#include "flash_data_storage.h"

#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"
#include "nrf_fstorage_sd.h"
#include "nrf_atomic.h"
#include "crc16.h"
#include "nrf_atfifo.h"
#include "fds.h"
#include "global.h"
#include "nrf_delay.h"

/* 所需文件
   name						 path								 brief
nrf_numc.c  			 modules\nrfx\hal  						 NVMC驱动
nrf_fstorage.c   	 components\libraries\fstorage				FS驱动文件
nrf_fstorage_nvmc.c  components\libraries\fstorage 	   FS后端nrf_fstorage_nvmc文件(裸机)
nrf_fstorage_sd.c    components\libraries\fstorage	FS后端nrf_fstorage_sd文件(Softdevice)
nrf_atomic.c			 components\libraries\atomic	   C11标准stdatomic.h简化API
crc16.c				 components\libraries\crc16					CRC16校验库文件
nrf_atfifo.c 		 components\libraries\atomic_fifo 	    	atomic FIFO
fds.c				 components\libraries\fds				    FDS 库文件
*/



//需要先注册FDS事件句柄，注册FDS事件需要先写事件处理函数
static void fds_evt_handler(fds_evt_t const * p_fds_evt)
{
	switch(p_fds_evt->id)
	{
		case FDS_EVT_INIT:
			NRF_LOG_INFO("FDS_EVT_INIT %d",p_fds_evt->result);
			FDS_busy = false;
			break;
		case FDS_EVT_WRITE:
			NRF_LOG_INFO("FDS_EVT_WRITE %d",p_fds_evt->result);
			break;
		case FDS_EVT_UPDATE:
			NRF_LOG_INFO("FDS_EVT_UPDATE %d",p_fds_evt->result);
			break;
		case FDS_EVT_DEL_RECORD:
			NRF_LOG_INFO("FDS_EVT_DEL_RECORD %d",p_fds_evt->result);
			break;
		case FDS_EVT_GC:
			NRF_LOG_INFO("FDS_EVT_GC %d",p_fds_evt->result);
			break;
		case FDS_EVT_DEL_FILE:
			NRF_LOG_INFO("FDS_EVT_DEL_FILE %d",p_fds_evt->result);
			if(p_fds_evt->result)
		default:
			NRF_LOG_INFO("default %d",p_fds_evt->result);
			break;
			
	}
}




static void wait_for_fds_init(void)
{
	while (FDS_busy);
}
void flash_data_storage_init(void)
{
        FDS_busy = false;//可以注册多个handler函数,注册后发生event 就会触发handler函数
	fds_register(fds_evt_handler);	//可多次初始化fds_init，多次注册同个handler,注册多少次就会调用多少次
	//初始化是异步的，需要等待
	//fds_init_compelet = false;
	FDS_busy = true;
 	ret_code_t err_code = fds_init();
	if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
			wait_for_fds_init();
	}
	else
	NRF_LOG_INFO("%d",err_code);

}




static device_version_t device_version=
{
	.hw_version = "HW-V1.11",
	.sw_version = "SW-V2.22"
};
static fw_information_t fw_information = 
{
	.size = 11111,
};
static fds_record_t sd_version_record = 
{
	.key = DEVICE_VERSION_KEY,
	.file_id = DEVICE_FILE,
	.data.p_data = &device_version,  //换算成字，+3 取商 是为了对齐 	  
	.data.length_words = (sizeof(device_version)+3)/sizeof(uint32_t),
};

static fds_record_t  information_record = {
	.key = FW_INFORMATION_KEY,
	.file_id = DEVICE_FILE,
	.data.p_data = &fw_information,
	.data.length_words = (sizeof(fw_information)+3)/sizeof(uint32_t),
};

static fds_record_t sd_version_record1 = 
{
	.key = DEVICE_VERSION_KEY,
	.file_id = DEVICE_FILE1,
	.data.p_data = &device_version,  //换算成字，+3 取商 是为了对齐 	  
	.data.length_words = (sizeof(device_version)+3)/sizeof(uint32_t),
};

	
	fds_record_desc_t desc[4] = {0};
	fds_find_token_t tok[4] = {0};
	fds_record_desc_t find_desc[4]= {0};

ret_code_t test_fds_record_write(fds_record_desc_t * const p_desc,fds_record_t const * const p_record)
{
	ret_code_t err_code = fds_record_write(p_desc,p_record);
	if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
	else
	NRF_LOG_INFO("%d",err_code);
	return 0;
}

void test_fds_record_find(uint16_t                  file_id,
                           uint16_t                  record_key,
                           fds_record_desc_t * const p_desc,
                           fds_find_token_t  * const p_token)
{
	ret_code_t err_code = fds_record_find(file_id, record_key,p_desc,p_token);
	
	if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
	else
	NRF_LOG_INFO("%d",err_code);
}

						   
/**********************************************************************
p_desc: 传入指向要修改的record句柄，输出修改后的新record句柄
p_record：新record
*************************************************************************/						   
void test_fds_record_update(fds_record_desc_t * p_desc, fds_record_t const * p_record)
{
	ret_code_t err_code = fds_record_update(p_desc,p_record);
		if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
		else
	NRF_LOG_INFO("%d",err_code);
}

/**********************************************************************
回收内存碎片
*************************************************************************/		

void test_fds_record_gc(void)
{
	ret_code_t err_code = fds_gc();
		if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
		else
	NRF_LOG_INFO("%d",err_code);
}
void test_fds_record_read(fds_record_desc_t * const p_desc, fds_flash_record_t * const p_flash_rec)
{
	ret_code_t err_code = fds_record_open(p_desc,p_flash_rec);
		if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
		else
	NRF_LOG_INFO("%d",err_code);
	err_code = fds_record_close(p_desc);
	if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
	else
	NRF_LOG_INFO("%d",err_code);
}
void test_fds_record_delete(fds_record_desc_t * const p_desc)
{
	ret_code_t err_code = fds_record_delete(p_desc);
		if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
	else
	NRF_LOG_INFO("%d",err_code);
}
void test_fds_file_delete(uint16_t file_id)
{
	ret_code_t err_code = fds_file_delete(file_id);
		if(err_code == FDS_SUCCESS)
	{
		NRF_LOG_INFO("FDS_SUCCESS");
	}
		else
	NRF_LOG_INFO("%d",err_code);
}


void test_fds_main(void)
{
	fds_flash_record_t st1 = {0};
	fds_flash_record_t st2 = {0};
	static device_version_t st3={0};

	static fw_information_t st4 = {0};

	
	flash_data_storage_init();	
	
/*	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <FW_INFORMATION_KEY> in the <DEVICE_FILE>");		//information  
	test_fds_record_find(DEVICE_FILE,FW_INFORMATION_KEY,&desc[0],&tok[0]);
        NRF_LOG_INFO("record id: %d",desc[0].record_id);
	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");
	test_fds_record_find(DEVICE_FILE,DEVICE_VERSION_KEY,&desc[1],&tok[1]);	//device
        NRF_LOG_INFO("record id: %d",desc[1].record_id);
	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE1>");
	test_fds_record_find(DEVICE_FILE1,DEVICE_VERSION_KEY,&desc[2],&tok[2]);
        NRF_LOG_INFO("record id: %d",desc[2].record_id);
        */
        
         test_fds_file_delete(DEVICE_FILE);
        nrf_delay_ms(1000);
	NRF_LOG_INFO("write <information_record> in the <DEVICE_FILE>"); //在DEVICE_FILE 再写一次information
	test_fds_record_write(&desc[0], &information_record);	//
	NRF_LOG_INFO("record id: %d",desc[3].record_id);		//观察记录位置
        
        
    nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");; //在DEVICE_FILE 再写一次information
	test_fds_record_update(&desc[1], &sd_version_record);	//
	NRF_LOG_INFO("record id: %d",desc[1].record_id);		//观察记录位置


		nrf_delay_ms(1000);
	NRF_LOG_INFO("find <FW_INFORMATION_KEY> in the <DEVICE_FILE>");		//information  
	test_fds_record_find(DEVICE_FILE,FW_INFORMATION_KEY,&desc[0],&tok[0]);
        NRF_LOG_INFO("record id: %d",desc[0].record_id);
	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");
	test_fds_record_find(DEVICE_FILE,DEVICE_VERSION_KEY,&desc[1],&tok[1]);	//device
        NRF_LOG_INFO("record id: %d",desc[1].record_id);
	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE1>");
	test_fds_record_find(DEVICE_FILE1,DEVICE_VERSION_KEY,&desc[2],&tok[2]);
        NRF_LOG_INFO("record id: %d",desc[2].record_id);
 /*       
            nrf_delay_ms(1000);
	NRF_LOG_INFO("find <information> in the <DEVICE_FILE>");; //在DEVICE_FILE 再写一次information
	test_fds_record_update(&desc[1], &information_record);	//
	NRF_LOG_INFO("record id: %d",desc[1].record_id);		//观察记录位置*/


	// test_fds_file_delete(DEVICE_FILE);
 
  /*  nrf_delay_ms(1000);
	NRF_LOG_INFO("find <FW_INFORMATION_KEY> in the <DEVICE_FILE>");		//information  
	test_fds_record_find(DEVICE_FILE,FW_INFORMATION_KEY,&desc[0],&tok[0]);
    NRF_LOG_INFO("record id: %d",desc[0].record_id);
	
	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");
	test_fds_record_find(DEVICE_FILE,DEVICE_VERSION_KEY,&desc[1],&tok[1]);	//device
    NRF_LOG_INFO("record id: %d",desc[1].record_id);



		nrf_delay_ms(1000);
		 NRF_LOG_INFO("gc the <DEVICE_FILE>");
		test_fds_record_gc();

		memset(&tok[0],0,sizeof(fds_find_token_t));
		memset(&tok[1],0,sizeof(fds_find_token_t));
		nrf_delay_ms(1000);
		NRF_LOG_INFO("find <FW_INFORMATION_KEY> in the <DEVICE_FILE>"); 	//information  
		test_fds_record_find(DEVICE_FILE,FW_INFORMATION_KEY,&desc[0],&tok[0]);
		NRF_LOG_INFO("record id: %d",desc[0].record_id);
		
		nrf_delay_ms(1000);
		NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");
		test_fds_record_find(DEVICE_FILE,DEVICE_VERSION_KEY,&desc[1],&tok[1]);	//device
		NRF_LOG_INFO("record id: %d",desc[1].record_id);


*/
	/*	
		nrf_delay_ms(1000);
	NRF_LOG_INFO("find <FW_INFORMATION_KEY> in the <DEVICE_FILE>");		//information  
	test_fds_record_find(DEVICE_FILE,FW_INFORMATION_KEY,&desc[3],&tok[0]);
        NRF_LOG_INFO("record id: %d",desc[3].record_id);
        
        
        nrf_delay_ms(1000);
        NRF_LOG_INFO("find <FW_INFORMATION_KEY> in the <DEVICE_FILE>");	
        test_fds_record_update(&desc[1],&information_record);
        NRF_LOG_INFO("record id: %d",desc[1].record_id);
        
        nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");
	test_fds_record_find(DEVICE_FILE,DEVICE_VERSION_KEY,&desc[1],&tok[1]);	//device
        NRF_LOG_INFO("record id: %d",desc[1].record_id);*/
		
	/*	  nrf_delay_ms(1000);
        NRF_LOG_INFO("delete the <DEVICE_FILE>");
        test_fds_file_delete(DEVICE_FILE);

		nrf_delay_ms(1000);
		 NRF_LOG_INFO("gc the <DEVICE_FILE>");
		test_fds_record_gc();*/
		
		
       // test_fds_file_delete(DEVICE_FILE)
/*	nrf_delay_ms(1000);
	NRF_LOG_INFO("update <information_record> in the <DEVICE_FILE>");	//在DEVICE_FILE 将 device 改 information
	test_fds_record_update(&desc[1], &information_record);
	NRF_LOG_INFO("record id: %d",desc[1].record_id);	//观察记录位置
	
	nrf_delay_ms(1000);
	NRF_LOG_INFO("find <DEVICE_VERSION_KEY> in the <DEVICE_FILE>");
	test_fds_record_find(DEVICE_FILE,DEVICE_VERSION_KEY,&desc[1],&tok[1]);	//再找device
	
	nrf_delay_ms(1000);
	NRF_LOG_INFO("update <information_record> in the <DEVICE_FILE>"); //在DEVICE_FILE 再写一次information
	test_fds_record_write(&desc[3], &information_record);	//
	NRF_LOG_INFO("record id: %d",desc[3].record_id);		//观察记录位置

	
	nrf_delay_ms(1000);
	NRF_LOG_INFO("read <information_record> in the <DEVICE_FILE>");//在DEVICE_FILE 内读出 ，是information
	test_fds_record_read(&desc[1],&st1);
	
	memcpy(&st3,st1.p_data,sizeof(device_version_t));

	NRF_LOG_INFO("read <information_record> in the <DEVICE_FILE>");//在DEVICE_FILE 内读出 ，是information
	test_fds_record_read(&desc[3],&st2);
	memcpy(&st4,st2.p_data,sizeof(device_version_t));*/

	
	
	
	
}

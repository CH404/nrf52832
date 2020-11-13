#ifndef __SAADC_H
#define __SAADC_H

#include <stdio.h>
#include <nrf_saadc.h>
#include <nrfx_saadc.h>

#define SAADC_INT_MODE SUE_BATTERY_INIT_DETECT


#ifdef SAADC_GLOBAL
#define SAADC_EXT
#else
#define SAADC_EXT extern
#endif

SAADC_EXT uint16_t SAADC_Value;

SAADC_EXT void SAADC_Init(void);



#if (!SAADC_INT_MODE) 
SAADC_EXT void SAADC_Start(uint8_t channel, nrf_saadc_value_t * p_value);
#else
SAADC_EXT void SAADC_Start();
#endif
#endif
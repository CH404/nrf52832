#ifndef __SAADC_H
#define __SAADC_H

#include <stdio.h>
#include <nrf_saadc.h>
#include <nrfx_saadc.h>

#ifdef SAADC_GLOBAL
#define SAADC_EXT
#else
#define SAADC_EXT extern
#endif

SAADC_EXT void SAADC_Init(void);


#endif
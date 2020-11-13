#ifndef __BMP_H
#define __BMP_H

#include "stdint.h"

#ifdef BMP_GLOBAL
#define BMP_EXT
#else
#define BMP_EXT extern
#endif


typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;


 BMP_EXT FontDef Font_16x26;
 BMP_EXT FontDef Font_7x10;

 BMP_EXT const uint8_t gImage_disableble[1828];

 BMP_EXT const uint8_t gImage_bmp[115208];
 BMP_EXT const uint16_t Font16x26 [];
 BMP_EXT const uint16_t Font7x10 [];
 //BMP_EXT const uint16_t Font16x16[];
 

 
BMP_EXT const char *mainPageFont;
BMP_EXT const char *blePageFont;
BMP_EXT const char *hrsPageFont;
BMP_EXT const char *espPageFont;
BMP_EXT const char *onFont;
BMP_EXT const char *offFont;
BMP_EXT const char *hrsfont;

#endif

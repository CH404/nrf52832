#ifndef __BMP_H
#define __BMP_H

#ifdef BMP_GLOBAL
#define BMP_EXT
#else
#define BMP_EXT extern
#endif
 BMP_EXT const uint8_t gImage_bmp[112808];
 BMP_EXT const uint8_t gImage_ble[8806];
#endif
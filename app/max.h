#ifndef __MAX_H
#define __MAX_H

#ifdef MAX_GLOBAL
#define MAX_EXT 
#else
#define MAX_EXT extern
#endif


MAX_EXT void MAX_Init(void);
MAX_EXT void max_test(void);
#endif
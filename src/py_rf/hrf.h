#ifndef __HACKRF_INCLUDE_FILE__
#define __HACKRF_INCLUDE_FILE__

#include <libusb-1.0/libusb.h>

#include "hrf_data.h"

int hrf_init(HRF *hrf);
int hrf_exit(HRF *hrf);

int hrf_is_device(int vid, int pid);

#ifdef HRF_BIG_ENDIAN
#define TO_LE(x) __builtin_bswap32(x)
#define TO_LE64(x) __builtin_bswap64(x)
#define FROM_LE16(x) __builtin_bswap16(x)
#define FROM_LE32(x) __builtin_bswap32(x)
#else
#define TO_LE(x) x
#define TO_LE64(x) x
#define FROM_LE16(x) x
#define FROM_LE32(x) x
#endif

#endif

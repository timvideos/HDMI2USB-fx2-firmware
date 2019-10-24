#ifndef MACROS_H
#define MACROS_H

#include <stdint.h>

#define LSB(word)          ((uint8_t)   ((word) & 0xff))
#define MSB(word)          ((uint8_t)   (((word) >> 8) & 0xff))
#define MAKEWORD(msb, lsb) (((uint16_t) (msb) << 8) | (lsb))

#define MSW(dword)         ((uint16_t)  (((dword) >> 16) & 0xffff))
#define LSW(dword)         ((uint16_t)  ((dword) & 0xffff))
#define MAKEDWORD(msw,lsw) (((uint32_t) (msw) << 16) | (lsw))

#endif /* MACROS_H */

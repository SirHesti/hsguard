/**
 @file c_crc32.h
 @brief Header fuer c_crc32.c
*/
#ifndef CRC32_H
#define CRC32_H
#ifdef  __cplusplus
extern "C" {
#endif

// CRC Funktionen ALT und bewaehrt
//_________________________________

#ifndef crc32_t
//__BEGIN_DECLS
typedef unsigned long crc32_t;
//__END_DECLS
#endif

crc32_t AddStringCRC (char *Buffer,                   crc32_t crc32);
crc32_t AddCRCByte   (char byte,                      crc32_t crc32);
crc32_t AddCRC       (int LenOfBuffer, char *Buffer,  crc32_t crc32);
int CRC32File        (char *filename,                 crc32_t *out_crc32);


#ifdef  __cplusplus
}
#endif
#endif

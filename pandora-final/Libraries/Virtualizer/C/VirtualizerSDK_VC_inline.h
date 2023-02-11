/******************************************************************************
 * Header: VirtualizerSDK_VC_inline.h
 * Description: VC inline assembly macros definitions
 *
 * Author/s: Oreans Technologies 
 * (c) 2013 Oreans Technologies
  ******************************************************************************/

/***********************************************
 * Definition as inline assembly
 ***********************************************/

#define VIRTUALIZER_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43\
  __asm _emit 0x56\
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0C \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43\
  __asm _emit 0x56\
  __asm _emit 0x20 \
  __asm _emit 0x20 \

#define VIRTUALIZER_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43\
  __asm _emit 0x56\
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0D \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43\
  __asm _emit 0x56\
  __asm _emit 0x20 \
  __asm _emit 0x20 \

#define VIRTUALIZER_STR_ENCRYPT_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x12 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  
 #define VIRTUALIZER_STR_ENCRYPT_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x13 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

  #define VIRTUALIZER_STR_ENCRYPTW_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x22 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  
 #define VIRTUALIZER_STR_ENCRYPTW_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x23 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 

  #define VIRTUALIZER_UNPROTECTED_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  
 #define VIRTUALIZER_UNPROTECTED_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x21 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x43 \
  __asm _emit 0x56 \
  __asm _emit 0x20 \
  __asm _emit 0x20 

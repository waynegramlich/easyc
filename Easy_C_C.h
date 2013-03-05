#ifndef EasyC_C_H_INCLUDED
#define EasyC_C_H_INCLUDED 1

/* Copyright (c) 2007-2008 by Wayne C. Gramlich */
/* All rights reserved. */

/* Do typedef's first */
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

typedef struct Array__Struct *Array;
typedef unsigned char Byte;
typedef unsigned Character;
typedef double Double;
typedef float Float;
typedef FILE *In_Stream;
typedef int Integer;
typedef int Logical;
typedef FILE *Out_Stream;
typedef signed short Short;
typedef union String__Union *String;
typedef unsigned int Unsigned;
typedef void *EZC;
typedef void **Pointer_Pointer;
typedef jmp_buf System_Jump_Buffer;
typedef long long int Long_Integer;
typedef unsigned long long int Long_Unsigned;

#define SCALARS___DEFINED 1

struct Array__Struct {
    Unsigned size;
    Unsigned limit;
    void **data;
};

typedef union {
  Character xcharacter;
  Float xfloat;
  Integer xinteger;
  Logical xlogical;
  Unsigned xunsigned;
  void* xpointer;
} CAST;

struct String__Literal {
    unsigned char size;
    unsigned char data[3];
};

struct String__Header {
  unsigned char size;
  unsigned char data[3];
  unsigned char *buffer;
  unsigned limit;
  unsigned front_size;
  unsigned total_size;
};

union String__Union {
  struct String__Literal literal;
  struct String__Header header;
  char *unix_string;
};

extern FILE In_Stream__Initial;
extern FILE Out_Stream__Initial;
union String__Union String__Initial;

extern unsigned char *String__Unix(String);
extern void String__Realloc(String, Unsigned);
extern void String__unix_append(String, char *);
extern void System__fail(String, String, Unsigned);

/* <stdio.h> defines the following lower-case (evil) macros: */
#ifndef STDIO_H_MACROS_NEEDED
#undef stderr
#undef getc
#undef stdin
#undef stdout
#undef putc
#endif /* STDIO_H_MACROS_NEEDED */

#ifdef SIMPLE___TYPEDEFS
SIMPLE___TYPEDEFS
#endif

#endif /* EasyC_C_H_INCLUDED */

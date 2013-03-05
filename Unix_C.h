#ifndef UNIX_C_H_INCLUDED
#define UNIX_C_H_INCLUDED 1



#ifndef UNIX_H_INCLUDED
#include "Unix.h"
#endif

#ifndef EASY_C_INCLUDED
#include "Easy_C.h"
#endif

#ifndef EASY_C_C_INCLUDED
#include "Easy_C_C.h"
#endif

#ifndef _ERRNO_H
#include <errno.h>
#endif

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef _SYS_STAT_H
#include <sys/stat.h>
#endif

#ifndef _DIRENT_H
#include <dirent.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

/* This is real ugly.  struct __dirstream is undefined and we need it
 * defined so we can create Unix_Directory_Stream__Initial: */
struct __dirstream {
    int zilch;
};

/* typedefs defined here: */
typedef struct Unix_File_Set__Struct *Unix_File_Set;
typedef DIR *Unix_Directory_Stream;
typedef struct stat *Unix_Status;

typedef unsigned char *Unix_SHA1_Pointer;
typedef unsigned long int Unix_SHA1_UInt4;
typedef struct Unix_SHA1_Struct *Unix_SHA1_Context;

#define SHS_DATASIZE    64
#define SHS_DIGESTSIZE  20

struct Unix_SHA1_Struct
{
    Unix_SHA1_UInt4 digest[5];		/* Message digest */
    Unix_SHA1_UInt4 countLo, countHi;	/* 64-bit bit count */
    Unix_SHA1_UInt4 data[16];		/* SHS data buffer */
    int endianness;
    Byte final[SHS_DIGESTSIZE];		/* Final digest */
};

struct Unix_File_Set__Struct {
    fd_set file_set;	/* File set proper: */
    Integer maximum;	/* Maximum bit set */
};

extern struct Unix_File_Set__Struct Unix_File_Set__Initial;
extern DIR Unix_Directory_Stream__Initial;
extern struct stat Unix_Status__Initial;
extern struct Unix_SHA1_Struct Unix_SHA1_Context__Initial;

extern Unix_SHA1_Context Unix_SHA1_Context__null;
void Unix_SHA1_Context__Initialize(void);
Byte Unix_SHA1_Context__fetch1(Unix_SHA1_Context context, Unsigned index);
void Unix_SHA1_Context__final(Unix_SHA1_Context context);
Unix_SHA1_Context Unix_SHA1_Context__new(void);
void Unix_SHA1_Context__reset(Unix_SHA1_Context context);
void Unix_SHA1_Context__update(Unix_SHA1_Context context, String buffer);

#endif /* UNIX_C_H_INCLUDED */

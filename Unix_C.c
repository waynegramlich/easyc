/*
 * Copyright (c) 1998-2010 by Wayne C. Gramlich.
 * All rights reserved.
 */

/*
 * This module implements interfaces to the Unix operating system.
 */

#ifndef UNIX_H_INCLUDED
#include "Unix.h"
#endif

#ifndef UNIX_C_H_INCLUDED
#include "Unix_C.h"
#endif

#ifndef EASY_C_C_H_INCLUDED
#include "Easy_C_C.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>

/*
 * What were the GNU folks thinking!!!  Lower case marcros are totally
 * evil!!!
 */
#undef unix

/* {Unix_Status} stuff */

struct stat Unix_Status__Initial;

Unix_Status Unix_Status__null = &Unix_Status__Initial;

void Unix_Status__Initialize(void)
{
}

Unix_Status Unix_Status__new(void)
{
    Unix_Status status;

    status = (Unix_Status)malloc(sizeof(*status));
    return status;
}

Logical Unix_Status__is_directory(
  Unix_Status status)
{
    Logical result;

    result = (Logical)0;
    if (S_ISDIR(status->st_mode)) {
	result = (Logical)1;
    }
    return result;
}

Logical Unix_Status__is_regular_file(
  Unix_Status status)
{
    Logical result;

    result = (Logical)0;
    if (S_ISREG(status->st_mode)) {
	result = (Logical)1;
    }
    return result;
}

Logical Unix_Status__is_symbolic_link(
  Unix_Status status)
{
    return S_ISLNK(status->st_mode);
}

Long_Unsigned Unix_Status__size_get(
  Unix_Status status)
{ 
    return (Long_Unsigned) status->st_size;
}

Unsigned Unix_Status__mode_get(
  Unix_Status status)
{
    return status->st_mode;
}

Unsigned Unix_Status__modification_time_get(
  Unix_Status status)
{
    return status->st_mtime;
}

/* {Unix_Directory_Stream} stuff: */

DIR Unix_Directory_Stream__Initial;

DIR *Unix_Directory_Stream__null =
  &Unix_Directory_Stream__Initial;


void Unix_Directory_Stream__close(
  Unix_Directory_Stream directory_stream)
{
    if (directory_stream != Unix_Directory_Stream__null) {
	(void)closedir(directory_stream);
    }
}

Unix_Directory_Stream Unix_Directory_Stream__open(
  String path_name)
{
    Unix_Directory_Stream directory_stream = opendir(String__Unix(path_name));
    if (directory_stream == (Unix_Directory_Stream)0) {
	directory_stream = Unix_Directory_Stream__null;
    }
    return directory_stream;
}

String Unix_Directory_Stream__read(
  Unix_Directory_Stream directory_stream,
  String file_name)
{
    char character;
    struct dirent *directory_entry;
    const char *pointer;

    directory_entry = readdir(directory_stream);
    if (directory_entry == (struct dirent *)0) {
	file_name = String__null;
    } else {
	if (file_name == String__null) {
	    file_name = String__new();
	}
	pointer = directory_entry->d_name;
	while ((character = *pointer++) != (char)0) {
	    String__character_append(file_name, character);
	}
    }
    return file_name;
}

void Unix_Directory_Stream__Initialize(void)
{
}

/* {Unix_File_Set} stuff: */

void Unix_File_Set__erase(
  Unix_File_Set unix_file_set)
{
    FD_ZERO(&unix_file_set->file_set);
    unix_file_set->maximum = -1;
    
}

Unix_File_Set Unix_File_Set__new(void)
{
    Unix_File_Set unix_file_set;
    extern void *malloc(size_t);

    unix_file_set = (Unix_File_Set)malloc(sizeof(*unix_file_set));
    Unix_File_Set__erase(unix_file_set);
    return unix_file_set;
}

struct Unix_File_Set__Struct Unix_File_Set__Initial;
Unix_File_Set Unix_File_Set__null = &Unix_File_Set__Initial;

void Unix_File_Set__update(
  Unix_File_Set unix_file_set)
{  
    Integer maximum;
    fd_set *file_set;

    file_set = &unix_file_set->file_set;
    maximum = unix_file_set->maximum;
    while (maximum >= 0 && !FD_ISSET(maximum, file_set)) {
	maximum--;
    }
    unix_file_set->maximum = maximum;
}


void Unix_File_Set__Initialize()
{
    FD_ZERO(&Unix_File_Set__null->file_set);
    Unix_File_Set__null->maximum = -1;
}

void Unix_File_Set__clear(
  Unix_File_Set unix_file_set,
  Integer fd)
{
    if (0 <= fd && fd < 1024) {
	FD_CLR(fd, &unix_file_set->file_set);
	if (fd >= unix_file_set->maximum) {
	    Unix_File_Set__update(unix_file_set);
	}
    }
}


void Unix_File_Set__contents_copy(
  Unix_File_Set to_unix_file_set,
  Unix_File_Set from_unix_file_set)
{
    *to_unix_file_set = *from_unix_file_set;
}


Logical Unix_File_Set__is_set(
  Unix_File_Set unix_file_set,
  Integer fd)
{
    Logical result;

    result = 0;
    if (0 <= fd && fd < 1024) {
	result = FD_ISSET(fd, &unix_file_set->file_set);
    }
    return (Logical)result;
}


void Unix_File_Set__set(
  Unix_File_Set unix_file_set,
  Integer fd)
{
    Integer maximum;

    if (0 <= fd && fd < 1024) {
	maximum = unix_file_set->maximum;
	if (fd > maximum) {
	    unix_file_set->maximum = fd;
	}
	FD_SET(fd, &unix_file_set->file_set);
    }
}


Integer Unix_File_Set__maximum_get(
  Unix_File_Set unix_file_set)
{
    return unix_file_set->maximum;
}


void Unix_File_Set__zero(
  Unix_File_Set unix_file_set)
{
    FD_ZERO(&unix_file_set->file_set);
    unix_file_set->maximum = -1;
}


/* {Unix} system calls: */

Integer Unix__accept(
  Integer socket_number)
{
    Integer result_socket_number;
    struct sockaddr_in address;
    Integer address_length;

    address_length = sizeof address;
    errno = 0;

    return accept(socket_number,
      (struct sockaddr *)&address, (socklen_t *)&address_length);
    /* return accept(socket_number, (struct sockaddr *)0, (socklen_t *)0); */
}

Integer Unix__bind(
    Integer socket_number,
    Unsigned internet_address,
    Unsigned port_number)
{
    struct sockaddr_in address;
    char *address_pointer;
    Integer index;
    Integer shift;

    address_pointer = (char *)&address.sin_addr;
    shift = 24;
    for (index = 0; index < 4; index++) {
	*address_pointer++ = (internet_address>>shift)&255;
	shift -= 8;
    }
    address_pointer = (char *)&address.sin_port;
    shift = 8;
    for (index = 0; index < 2; index++) {
	*address_pointer++ = (port_number>>shift)&255;
	shift -= 8;
    }
    address.sin_family = AF_INET;
    errno = 0;
    return bind(socket_number, (struct sockaddr *)&address, sizeof address);
}

Integer Unix__connect(
    Integer socket_number,
    Unsigned internet_address,
    Unsigned port_number)
{
    struct sockaddr_in address;
    char *address_pointer;
    int index;
    int shift;

    address_pointer = (char *)&address.sin_addr;
    shift = 24;
    for (index = 0; index < 4; index++) {
	*address_pointer++ = (internet_address>>shift)&255;
	shift -= 8;
    }
    address_pointer = (char *)&address.sin_port;
    shift = 8;
    for (index = 0; index < 2; index++) {
	*address_pointer++ = (port_number>>shift)&255;
	shift -= 8;
    }
    address.sin_family = AF_INET;
    errno = 0;
    return connect(socket_number, (struct sockaddr *)&address, sizeof address);
}


Integer Unix__close(
  Integer file_descriptor_number)
{
    Integer result;

    errno = EBADF;
    result = close(file_descriptor_number);
    return result;
}

Logical Unix__current_working_directory(
  String buffer)
{
    char temporary[1000];
    Logical result;
    char *directory;

    String__trim(buffer, 0);
    result = 1;
    directory = getcwd(temporary, sizeof temporary - 1);
    if (directory != (char *)0) {
        String__unix_append(buffer, directory);
	result = 0;
    }
    return result;
}

Logical Unix__environment_fetch(
  Unsigned index,
  String buffer)
{
    Unsigned size;
    extern char **environ;
    Logical result;

    String__trim(buffer, 0);
    result = 1;
    size = Unix__environment_size();
    if (index < size) {
	String__unix_append(buffer, environ[index]);
	result = 0;
    }
    return result;
}

Logical Unix__environment_lookup(
  String name,
  String buffer)
{
    char *value;
    Logical result;

    result = 1;
    String__trim(buffer, 0);
    value = getenv(String__Unix(name));
    if (value != (char *)0) {
	String__unix_append(buffer, value);
	result = 0;
    }
    return result;
}

Unsigned Unix__environment_size(void)
{
    Unsigned index;
    extern char **environ;

    index = 0;
    while (environ[index] != (char*)0) {
	index++;
    }
    return index;
}

String Unix__errno_convert(
  String string,
  Unsigned error_number)
{
  char *pointer;
    char chr;

    if (string == String__null) {
      string = String__new();
    } else {
	(void)String__trim(string, 0);
    }
    for (pointer = strerror(error_number); *pointer != '\0'; pointer++) {
	(void)String__character_append(string, *pointer);
    }
    return string;
}

Unsigned Unix__errno(void)
{
    return errno;
}

Integer
Unix__execve(
    String file_name,
    Array arguments,
    Array environment)
{
    char *filename;
    char **args;
    char **env;
    Unsigned index;
    Unsigned size;
    Integer result;

    filename = String__Unix(file_name);

    /* Fill in {args}: */
    size = Array__size_get(arguments);
    args = (char **)malloc(sizeof (char *) * (size + 1));
    for (index = 0; index < size; index++) {
	args[index] = String__Unix(Array__fetch1(arguments, index));
    }
    args[index] = (char *)0;

    /* Fill in {env}: */
    size = Array__size_get(environment);
    env = (char **)malloc(sizeof (char *) * (size + 1));
    for (index = 0; index < size; index++) {
	env[index] = String__Unix(Array__fetch1(environment, index));
    }
    env[index] = (char *)0;

    /* Exec the program: */
    errno = 0;
    result = execve(filename, args, env);

    /* execve only returns if it fails: */
    return result;
}

Integer
Unix__fork(void)
{    
    return fork();
}

/*
 * This procedure will perform a fcntl(2) system call.
 */
Integer
Unix__file_control(
    Integer file_descriptor_number,
    Integer operation,
    Integer value)
{
    Integer result;

    errno = 0;
    return fcntl(file_descriptor_number, operation, value);
}

/*
 * This routine will return the number of microseconds since January 1, 1970.
 */

Long_Integer Unix__get_time_of_day(void)
{
    struct timeval time_value;

    (void)gettimeofday(&time_value, (struct timezone *)0);

    return (((Long_Integer)time_value.tv_sec) * 1000000LL) +
      ((Long_Integer)time_value.tv_usec);
}

Logical
Unix__host_lookup(
  Unsigned internet_address,
  String host_name)
{
    return 0;
}

Unsigned
Unix__internet_address_lookup(
  String host_name)
{
    Unsigned internet_address;

    char *hostname;
    struct hostent *host_entry;
    extern int h_errno;

    errno = 0;
    h_errno = 0;
    hostname = String__Unix(host_name);
    internet_address = 0;
    host_entry = gethostbyname(hostname);
    if (host_entry == (struct hostent *)0) {
	switch (h_errno) {
	  case HOST_NOT_FOUND:
	    internet_address = 0;
	    break;
	  case NO_ADDRESS:
	    internet_address = 0;
	    break;
	  case NO_RECOVERY:
	    internet_address = 0;
	    break;
	  case TRY_AGAIN:
	    internet_address = 0;
	    break;
	  default:
	    internet_address = 0;
	}
    } else {
	char *address;
	unsigned index;

	if (host_entry->h_length == 4 && host_entry->h_addrtype == AF_INET) {
	    address = host_entry->h_addr_list[0];
	    if (address != (char *)0) {
		for (index = 0; index < 4; index++) {
		    internet_address =
		      (internet_address << 8) | (address[index] & 255);
		}
	    } else {
		internet_address = 0;
	    }
	} else {
	    internet_address = 0;
	}
    }
    return internet_address;
}


void Unix__listen(
    Integer socket_number,
    Unsigned queue_length)
{
    errno = EBADF;
    (void)listen(socket_number, (int)queue_length);
}


Integer Unix__lstat(
  String path,
  Unix_Status status)
{
    errno = 0;
    return lstat(String__Unix(path), status);
}

Integer Unix__mkdir(
    String new_directory_name,
    Unsigned new_directory_mode)
{
    Integer result;

    errno = 0;
    result = mkdir(String__Unix(new_directory_name), new_directory_mode);
    return result;
}


Integer Unix__open(
  String pathname,
  Unsigned flags,
  Unsigned mode)
{
    int file_descriptor_number;

    errno = 0;
    file_descriptor_number = open(String__Unix(pathname), flags, mode);
    return file_descriptor_number;
}

Integer Unix__read(
    Integer file_descriptor_number,
    String string,
    Unsigned offset,
    Unsigned amount)
{
    Integer result;
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	Unsigned total_size;
	Unsigned limit;

	limit = string->header.limit;
	total_size = string->header.total_size;
	if (amount > limit - total_size) {
	    String__Realloc(string, total_size + amount);
	}
	String__gap_set(string, offset);
	errno = 0;
	result = read(file_descriptor_number,
	  string->header.buffer + offset, amount);
	if (result >= 0) {
	    string->header.front_size += result;
	    string->header.total_size = total_size + result;
	}
    } else {
	/* Literal string; Read-only; error! */
	result = -1;
    }
    return result;
}


Integer Unix__readlink(
  String symbolic_link,
  String value)
{
    Integer result;
    Byte buffer[1024];
    Unsigned size;
    Unsigned index;

    result = readlink((const char *)String__Unix(symbolic_link),
      buffer, sizeof(buffer));
    if (result >= 0) {
	String__trim(value, 0);
        for (index = 0; index < result; index++) {
	    String__character_append(value, buffer[index]);
	}
    }
    return result;
}


Logical Unix__real_path(
  String path,
  String resolved_path)
{
    Logical result;
    char *file;
    char real_path[4096];

    result = 1;
    file = String__Unix(path);
    /* (void)printf("file='%s'\n", file); */
    if (realpath(file, real_path) != (char *)0) {
	result = 0;
	String__trim(resolved_path, 0);
	String__unix_append(resolved_path, real_path);
    }
    /* (void)printf("real_file='%s'\n", real_path); */
    return result;
}

Integer Unix__rename(
  String old_file_name,
  String new_file_name)
{
    Integer result;

    errno = 0;
    result = rename((const char *)String__Unix(old_file_name),
      (const char *)String__Unix(new_file_name));
    return result;
}

Integer Unix__select(
  Unix_File_Set read_file_set,
  Unix_File_Set write_file_set,
  Unix_File_Set exception_file_set,
  Unsigned seconds,
  Unsigned microseconds)
{
    Integer result;
    fd_set *exception_fd_set;
    Integer exception_maximum;
    Integer maximum;
    fd_set *read_fd_set;
    Integer read_maximum;
    struct timeval time_val;
    struct timeval *time_val_pointer;
    fd_set *write_fd_set;
    Integer write_maximum;

    /* Extract the read/write/exception information: */
    read_fd_set = (fd_set *)0;
    read_maximum = 0;
    if (read_file_set != Unix_File_Set__null) {
	read_maximum = read_file_set->maximum;
	read_fd_set = &read_file_set->file_set;
    }
    write_fd_set = (fd_set *)0;
    write_maximum = 0;
    if (write_file_set != Unix_File_Set__null) {
	write_maximum = write_file_set->maximum;
	write_fd_set = &write_file_set->file_set;
    }
    exception_fd_set = (fd_set *)0;
    exception_maximum = 0;
    if (exception_file_set != Unix_File_Set__null) {
	exception_maximum = exception_file_set->maximum;
	exception_fd_set = &exception_file_set->file_set;
    }

    /* Compute the maximum fd: */
    maximum = read_maximum;
    if (write_maximum > maximum) {
	maximum = write_maximum;
    }
    if (exception_maximum > maximum) {
	maximum = exception_maximum;
    }

    /* Figure out what to do about the timeout: */
    time_val.tv_sec = seconds;
    time_val.tv_usec = microseconds;
    time_val_pointer = &time_val;
    if (seconds == 0xffffffff) {
	time_val_pointer = (struct timeval *)0;
    }

    /* Perform the pselect(2) system call: */
    errno = 0;
    /* (void)fprintf(stderr, "select(%d, 0x%x, 0x%x, 0x%x, 0x%x)=>%d\n",
      maximum + 1, read_file_set, write_file_set,
      exception_file_set, time_val_pointer, result); */
    result = select(maximum + 1, read_fd_set, write_fd_set,
      exception_fd_set, time_val_pointer);

    /* Update the file sets: */
    if (read_file_set != Unix_File_Set__null) {
	Unix_File_Set__update(read_file_set);
    }
    if (write_file_set != Unix_File_Set__null) {
	Unix_File_Set__update(write_file_set);
    }
    if (exception_file_set != Unix_File_Set__null) {
	Unix_File_Set__update(exception_file_set);
    }

    /* Return the result: */
    return result;
}


Integer Unix__set_socket_option(
    Integer socket_number,
    Integer socket_option,
    Integer value)
{
    errno = 0;
    return setsockopt(socket_number,
      SOL_SOCKET, socket_option, &value, sizeof value);
}


Integer Unix__socket_stream_create(void)
{
    errno = 0;
    return socket(AF_INET, SOCK_STREAM, 0);
}

Integer Unix__stat(
  String path,
  Unix_Status status)
{
    errno = 0;
    return stat(String__Unix(path), status);
}

Integer Unix__symlink(
  String old_path,
  String new_path)
{
    Integer result;

    errno = 0;
    result = symlink((const char *)String__Unix(old_path),
      (const char *)String__Unix(new_path));
    return result;
}

Integer Unix__system(
  String command)
{
    return system(String__Unix(command));
}

Integer Unix__write(
    Integer file_descriptor_number,
    String string,
    Unsigned offset,
    Unsigned amount)
{
    unsigned char *data;
    Integer result;
    Unsigned size;

    data = (unsigned char *)0;
    result = -1;
    errno = EFAULT;
    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	Unsigned front_size;
	Unsigned total_size;

	front_size = string->header.front_size;
	total_size = string->header.total_size;
	if (offset + amount <= total_size) {
	    data = (unsigned char *)string->header.buffer;
	    if (offset + amount > front_size) {
		/* Move the gap: */
		String__gap_set(string, total_size);
	    }
	}
    } else {
	/* Literal string: */
	if (offset + amount <= size) {
	    data = (unsigned char *)string->literal.data;
	}
    }

    if (data != (unsigned char *)0) {
	errno = 0;
	result = write(file_descriptor_number, data + offset, amount);
	/* if (result < 0) {
	    (void)perror("Unix__write");
	} */
    }
    return result;
}

Integer Unix__unlink(
  String path_name)
{
    Integer result;
    unsigned char *unix_path_name;

    errno = 0;
    unix_path_name = String__Unix(path_name);
    result = unlink(unix_path_name);
    return result;
}


/* Everything below here is old code that is not being compiled: */

#ifdef UNUSED

#ifndef FILE_SET_H
#include "file_set.h"
#endif

#ifndef MEMORY_H
#include "memory.h"
#endif

#ifndef STRING_H
#include "string.h"
#endif

#ifndef STHEADERS_H
#include "stheaders.h"
#endif

#ifndef FILE_SET_H
#include "file_set.h"
#endif

#ifndef UNIX_SYSTEM_ASSERT_H
#include "unix_assert.h"
#endif

#ifndef UNIX_ERRNO_H
#include "unix_errno.h"
#endif

#ifndef UNIX_FCNTL_H
#include "unix_fcntl.h"
#endif

#ifdef LINUX
#ifndef UNIX_NETDB_H
#include "unix_netdb.h"
#endif
#endif /* LINUX */

#ifndef UNIX_SYS_TYPES_H
#include "unix_sys_types.h"
#endif

#ifdef LINUX
#ifndef UNIX_SYS_SOCKET_H
#include "unix_sys_socket.h"
#endif

#ifndef UNIX_SYS_RESOURCE_H
#include "unix_sys_resource.h"
#endif

#ifndef UNIX_SYS_UTSNAME_H
#include "unix_sys_utsname.h"
#endif
#endif /* LINUX */

#ifndef UNIX_UNISTD_H
#include "unix_unistd.h"
#endif

#ifdef LINUX
#ifndef UNIX_NETINET_IN_H
#include "unix_netinet_in.h"
#endif

#ifndef UNIX_SYS_WAIT_H
#include "unix_sys_wait.h"
#endif
#endif /* LINUX */

typedef struct unix_system_struct *Unix_system, Unix_system_struct;
typedef void *String_array;
typedef void *System;


#define FD_MAX	__FD_SETSIZE

struct unix_system_struct {
#ifdef LINUX
    File_set file_set;		/* Open files */
#endif /* LINUX */
    int pipes[2];		/* Pipes */
#ifdef LINUX
    Rusage_struct rusage;	/* Resource usage information */
#endif /* LINUX */
#ifdef WINDOWS
    int rusage;
#endif /* WINDOWS */
    int wait_status;		/* Status from last wait4(2) system call */
};

extern String_array string_array__create(String, unsigned);
extern void string_array__store1(String_array, unsigned, String);
extern Str *string_array__unix_strings_get(String_array);
extern String string__allocate();
extern int string__unix_load(String, Str);
extern System system__standard(void);
extern Str *environ;		/* Environment vector */

extern module___object unix_system__module__object;
extern type___reference unix_system_type_ref;

static Unix_system_struct
  unix_system___initial_value; /* Initialize to zeros */
Unix_system unix_system___initial = &unix_system___initial_value;

extern module___object unix_system__module__object;
extern type___reference unix_system_type_ref;

static object___object unix_system_initial_object = {
    "",				/* Package name */
    "unix_system",			/* Type name */
    "??",			/* Object name */
    (type___reference *)0,	/*XXX: Fix me */
    (int *)0,			/* Size of float object */
    0,				/* Static needs count */
    (need___entry *)0,		/*XXX: Fix me Static needs */
    0,				/* Parameter needs count */
    (need___entry *)0,		/* Parameter needs */
    (void **)&unix_system___initial,	/* Object pointer */
    (instantiation___object *)0	/* Instantiation list */
};
static object___object *object_list[1] = {
    &unix_system_initial_object,
};

static int
unix_system__is_open(
    Unix_system unix_system,
    unsigned file_descriptor_number)
{
#ifdef LINUX
    if (file_descriptor_number < FD_MAX &&
      file_set__is_in(unix_system->file_set, file_descriptor_number)) {
	return 1;
    }
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
    return 0;    
}
	
static void
unix_system__mark_closed(
    Unix_system unix_system,
    unsigned file_descriptor_number)
{
#ifdef LINUX
    if (file_descriptor_number < FD_MAX) {
	file_set__remove(unix_system->file_set, file_descriptor_number);
    }
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}

static void
unix_system__mark_open(
    Unix_system unix_system,
    unsigned file_descriptor_number)
{
#ifdef LINUX
    if (file_descriptor_number < FD_MAX) {
	file_set__enter(unix_system->file_set, file_descriptor_number);
    }
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}

/*
 * unix_system__external__initialize()
 *	This routine will initialize the unix_system module.
 */
void
unix_system__external__initialize(void)
{
    Unix_system unix_system;
    File_set file_set;

    unix_system = unix_system___initial;
#ifdef LINUX
    unix_system->file_set = file_set__create();
    unix_system__mark_open(unix_system, 0);	/* Standard in */
    unix_system__mark_open(unix_system, 1);	/* Standard out */
    unix_system__mark_open(unix_system, 2);	/* Standard error */
#endif /* LINUX */
}

/*
 * unix_system__close(unix_system, file_descriptor_number)
 * This procedure will close {file_descriptor_number}.  A call to
 * {status_get}@{unix_system}() will indicate how successful the
 * operation is; the possible return values are {ok} or {bad_file}.
 * This operation does not permit the closing of file descriptors
 * 0 (stdin), 1 (stdout), or 2 (stderr).
 *
 * It is really important to check the return status of this call
 * when writing out a file  because sometimes the operating system
 * does not realize that there is no disk space left until after a
 * write(2) system call has successfully returned.  By verifying
 * that the close call succeeded, there is assurance that the file
 * was written successfully to disk.
 */
void
unix_system__duplicate2(
    Unix_system unix_system,
    unsigned from_file_descriptor_number,
    unsigned to_file_descriptor_number)
{
    errno = EBADF;
    if (unix_system__is_open(unix_system, from_file_descriptor_number)) {
	errno = 0;
	dup2(from_file_descriptor_number, to_file_descriptor_number);
	unix_system__mark_open(unix_system, to_file_descriptor_number);
    }
}

/*
 * This procedure will return the environment vector passed into
 * the currently executing process.
 */
String_array
unix_system__environment_get(
    Unix_system unix_system)
{
    Str *environment;
    unsigned index;
    unsigned size;
    String_array string_array;
    System system;
    Str var;
    String variable;

    /* First, figure out how big the environment is: */
    size = 0;
    environment = environ;
    while ((var = *environment++) != (Str)0) {
	size += 1;
    }

    /* Second, create and fill in the {string_array} array object: */
    system = system__standard();
    variable = string__allocate();
    string_array = string_array__create(variable, size);
    environment = environ;
    index = 0;
    while ((var = *environment++) != (Str)0) {
	variable = string__allocate();
	string__unix_load(variable, var);
	string_array__store1(string_array, index, variable);
	index += 1;
    }

    return string_array;
}

Unsigned Unix__errno_get(
  Unix unix)
{
    return errno;
}

/*
 * This procedure will make a directory named {new_directory_name}
 * with access mode bits of {new_directory_mode}.
 */
void
unix_system__directory_create(
    Unix_system unix_system,
    String new_directory_name,
    unsigned new_directory_mode)
{
#ifdef LINUX
    Str new_dir_name;

    errno = 0;
    new_dir_name = string__unix_string(new_directory_name);
    (void)mkdir(new_dir_name, new_directory_mode);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}


/*
 * This procedure will cause the current process to immediately
 * exit with a status of {status}.  This procedure never returns.
 * This procedure invokes the _exit(2) system call.
 */
void
unix_system__exit(
    Unix_system unix_system,
    unsigned status)
{
    _exit(status);
}

/*
 * This procedure will performe fork(2) unix system call.
 * In general, this procudure is intended to be used right
 * before calling the {execute}@{unix_system}() procedure.
 * The child process will get a return value of 0.  The parent
 * process will get the child process id.
 */
unsigned
unix_system__fork(
    Unix_system unix_system)
{
#ifdef LINUX
    pid_t child_process_number;

    child_process_number = fork();
    if (child_process_number != -1) {
	errno = 0;
    }
    return (unsigned)child_process_number;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}


/* Kludge for windows port: */
#ifndef SO_BSDCOMPAT
#define SO_BSDCOMPAT 0
#endif

/*
 * This procedure will return the {socket_option} value for
 * {socket_number}.
 */
Integer Unix__get_socket_option(
  Integer socket_number,
  Integer socket_option)
{
    int value;
    int value_length;

    value = 0;
    errno = EBADF;
    if (unix_system__is_open(unix_system, socket_number)) {
	errno = ENOPROTOOPT;
	if (socket_option <= SO_BSDCOMPAT) {
	    errno = 0;
	    value_length = sizeof value;
	    (void)getsockopt(socket_number,
	      SOL_SOCKET, socket_option, &value, &value_length);
	}
    }
    return value;
}

/*
 * This procedure will store the kernel system name into the writable
 * string {system_name}.
 */
void
unix_system__kernel_system_name(
    Unix_system unix_system,
    String system_name)
{
#ifdef LINUX
    Uts_name_struct uts_name;

    errno = 0;
    assert(uname(&uts_name) == 0);
    string__unix_append(system_name, uts_name.sysname);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}

/*
 * This procedure will store the kernel system name into the writable
 * string {node_name}.
 */
void
unix_system__kernel_node_name(
    Unix_system unix_system,
    String node_name)
{
#ifdef LINUX
    Uts_name_struct uts_name;

    errno = 0;
    assert(uname(&uts_name) == 0);
    string__unix_append(node_name, uts_name.nodename);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}

/*
 * This procedure will store the kernel system name into the writable
 * string {domain_name}.
 */
void
unix_system__xxxkernel_domain_name(
    Unix_system unix_system,
    String domain_name)
{
    /*Uts_name_struct uts_name; */

    /* FIXME: How is this done with the new glibc()? */
    /*uname(&uts_name); */
    /* string__unix_append(domain_name, uts_name.domainname); */
}

/*
 * This procedure will send signal {signal_number} to process
 * {process_number}.
 */
unsigned
unix_system__kill(
    Unix_system unix_system,
    unsigned process_number,
    unsigned signal_number)
{
#ifdef LINUX
    if (kill(process_number, signal_number) == 0) {
	errno = 0;
    }
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
    return errno;
}

/*
 * This procedure will perform the actual unix pipe(2) system call.  The
 * read file descriptor is obtained via {pipe_read_get}@{unix_system}()
 * procedure call.  The write file descriptor is obtained via
 * {pipe_write_get}@{unix_system}() procedure call.
 *
 * Note: This procedure should only be called from {pipe}@{unix_system}().
 */
void
unix_system__pipe_actual(
    Unix_system unix_system)
{
#ifdef LINUX
    if (pipe(&unix_system->pipes[0]) == 0) {
	errno = 0;
	unix_system__mark_open(unix_system, unix_system->pipes[0]);
	unix_system__mark_open(unix_system, unix_system->pipes[1]);
    }
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}

/*
 * This procedure will return the read file descriptor for the last
 * pipe created via a call to the {pipe_actual}@{unix_system}()
 * procedure.
 *
 * Note: This procedure should only be called from {pipe}@{unix_system}().
 */
unsigned
unix_system__pipe_read_get(
    Unix_system unix_system)
{
    return unix_system->pipes[0];
}

/*
 * This procedure will return the write file descriptor for the last
 * pipe created via a call to the {pipe_actual}@{unix_system}()
 * procedure.
 *
 * Note: This procedure should only be called from {pipe}@{unix_system}().
 */
unsigned
unix_system__pipe_write_get(
    Unix_system unix_system)
{
    return unix_system->pipes[1];
}

/*
 * unix_system__read(unix_system,
 *   file_descriptor_number, memory, offset, amount)
 *	This procedure will read in up to {amount} bytes from
 *	{file_descriptor_number} and store them into {memory} starting
 *	at {offset}.
 */
unsigned
unix_system__read(
    Unix_system unix_system,
    unsigned file_descriptor_number,
    Memory memory,
    unsigned offset,
    unsigned amount)
{
    errno = EBADF;
    if (unix_system__is_open(unix_system, file_descriptor_number) &&
      offset + amount <= memory->limit) {
	int result;

	errno = 0;
	result = read(file_descriptor_number, memory->buffer + offset, amount);
	if (result >= 0) {
	    return (unsigned)result;
	}
    }
    return 0;
}

/*
 * This procedure will return the number of user time seconds
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_user_time_seconds_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_utime.tv_sec;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of user time microseconds
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_user_time_microseconds_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_utime.tv_usec;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of system time seconds
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_system_time_seconds_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_stime.tv_sec;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of system time microseconds
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_system_time_microseconds_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_stime.tv_usec;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the maximum resident set size
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_maximum_resident_set_size_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_maxrss;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the maximum shared memory size
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_maximum_shared_memory_size_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_ixrss;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}


/*
 * This procedure will return the maximum unshared data size
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_maximum_unshared_data_size_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_idrss;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the maximum unshared stack size
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_maximum_unshared_stack_size_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_isrss;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of minor page faults
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_minor_page_faults_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_minflt;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of major page faults
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_major_page_faults_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_majflt;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of swaps
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_swaps_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_nswap;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of input blocks
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_input_blocks_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_inblock;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of output blocks
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_output_blocks_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_oublock;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of message send
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_message_sends_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_msgsnd;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of message receives
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_message_receives_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_msgrcv;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of signals
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_signals_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_nsignals;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of voluntary context switches
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_voluntary_context_switches_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_nvcsw;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the number of involuntary context switches
 * associated with {unix_system}.  This value is set via a call
 * to either {resources_obtain}@{unix_system}() or via a call
 * {wait}@{unix_system}().
 */
unsigned
unix_system__resource_involuntary_context_switches_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return unix_system->rusage.ru_nivcsw;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will cause the I/O pointer for {file_descriptor_number}
 * to be positioned to {offset}.  The actual offset seeked to is returned.
 * If an error occurs, 0xffffffff is returned and a call to
 * {status_get}@{unix_system}() returns one of {ok}, {bad_file},
 * {illegal_seek}, or {invalid}.
 */
unsigned
unix_system__seek(
    Unix_system unix_system,
    unsigned file_descriptor_number,
    unsigned offset)
{
    errno = 0;
    return (unsigned)lseek(file_descriptor_number, (off_t)offset, SEEK_SET);
}


/*
 * This procedure will the {socket_option} value for {socket_number}
 * to {value}.
 */
void
unix_system__set_socket_option(
    Unix_system unix_system,
    unsigned socket_number,
    unsigned socket_option,
    unsigned value)
{
#ifdef LINUX
    value = 0;
    errno = EBADF;
    if (unix_system__is_open(unix_system, socket_number)) {
	errno = ENOPROTOOPT;
	if (socket_option <= SO_BSDCOMPAT) {
	    errno = 0;
	    (void)setsockopt(socket_number,
	      SOL_SOCKET, socket_option, &value, sizeof value);
	}
    }
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
}

/*
 * This procedure will return the status of the last unix system call.
 */
unsigned
unix_system__status_get(
    Unix_system unix_system)
{
    return errno;
}
    
/*
 * This procedure will truncate the file refered to by
 * {file_descriptor_number} to no more than {length} bytes.
 * This is equivalent to the ftruncate(2) system call.
 */
void
unix_system__truncate(
    Unix_system unix_system,
    unsigned file_descriptor_number,
    unsigned length)
{
    assert(0);
}

/*
 * This procedure will perform a unix wait4(2) system call.
 * If {process_number} is 0, the procedure will wait until
 * any child process exits/stops; in which case the process
 * id of the child process that exited/stopped is returned.
 * If {process_number} is non-zero, the procedure will wait for
 * the specified child process to exit/stop.  If {no_hang}
 * is {true}, this procedure will not block waiting for the child
 * process exit/stop, instead it will return immediately with
 * a value of 0xffffffff if specified child process is not
 * exited/stopped.  If {stopped_too} is {true}, stopped processes
 * are reported in addition to exited processes; otherwise, only
 * exited processes are reported.  There are a number of
 * {wait_*_get}@{unix_system} procedures for figuring out whether
 * a process exited normally, via a signal, or is just stopped
 * (i.e. {stopped_too} is {true}.)  Finally, the resource usage
 * information for the exited/stopped process can be obatianed
 * via the {resource_*_get}@{unix_system} procedures.
 */
unsigned
unix_system__wait(
    Unix_system unix_system,
    unsigned process_number,
    int no_hang,
    int	stopped_too)
{
    pid_t result;
#ifdef LINUX
    int options;

    options = 0;
    if (no_hang) {
	options |= WNOHANG;
    }
    if (stopped_too) {
	options |= WUNTRACED;
    }

    errno = 0;
    result = wait4(process_number,
      &unix_system->wait_status, options, &unix_system->rusage);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
#endif /* WINDOWS */
    return (unsigned)result;
}

/*
 * This procedure will return {true}@{logical} if the last call
 * to {wait}@{unix_system}() resulted in a child process that
 * exited normally; otherwise, {false} is returned.
 */
int
unix_system__wait_exited_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return (WIFEXITED(unix_system->wait_status) == 0) ? 0 : 1;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the least significant 8 bits of
 * the return code for exiting process.  These 8 bits are only
 * valid if {wait_exited_get}@{unix_system} returned {true}.
 */
unsigned
unix_system__wait_exit_status_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return WEXITSTATUS(unix_system->wait_status);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return {true}@{logical} if the last call
 * to {wait}@{unix_system}() resulted in a child process that
 * exited because of a signal; otherwise {false} is returned.
 */
int
unix_system__wait_signaled_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return (WIFSIGNALED(unix_system->wait_status) == 0) ? 0 : 1;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the signal value that caused the
 * the last process to exit.  The signal value is only valid if
 * {wait_signaled_get}@{unix_system} returned {true}.
 */
unsigned
unix_system__wait_signal_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return WTERMSIG(unix_system->wait_status);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return {true}@{logical} if the last call
 * to {wait}@{unix_system}() resulted in a child process that
 * exited because of a signal; otherwise {false} is returned.
 */
int
unix_system__wait_stopped_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return (WIFSTOPPED(unix_system->wait_status) == 0) ? 0 : 1;
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

/*
 * This procedure will return the signal value that caused the
 * the last process to stop.  The signal value is only valid if
 * {wait_stopped_get}@{unix_system} returned {true}.
 */
unsigned
unix_system__wait_stop_get(
    Unix_system unix_system)
{
#ifdef LINUX
    return WSTOPSIG(unix_system->wait_status);
#endif /* LINUX */
#ifdef WINDOWS
    assert(0);
    return 0;
#endif /* WINDOWS */
}

#endif /* UNUSED */

/* The code below was yanked off the net: */

/* sha1.c : Implementation of the Secure Hash Algorithm */

/* SHA: NIST's Secure Hash Algorithm */

/*	This version written November 2000 by David Ireland of 
	DI Management Services Pty Limited <code@di-mgt.com.au>

	Adapted from code in the Python Cryptography Toolkit, 
	version 1.0.0 by A.M. Kuchling 1995.
*/

/* AM Kuchling's posting:- 
   Based on SHA code originally posted to sci.crypt by Peter Gutmann
   in message <30ajo5$oe8@ccu2.auckland.ac.nz>.
   Modified to test for endianness on creation of SHA objects by AMK.
   Also, the original specification of SHA was found to have a weakness
   by NSA/NIST.  This code implements the fixed version of SHA.
*/

/* Here's the first paragraph of Peter Gutmann's posting:
   
The following is my SHA (FIPS 180) code updated to allow use of the "fixed"
SHA, thanks to Jim Gillogly and an anonymous contributor for the information on
what's changed in the new version.  The fix is a simple change which involves
adding a single rotate in the initial expansion function.  It is unknown
whether this is an optimal solution to the problem which was discovered in the
SHA or whether it's simply a bandaid which fixes the problem with a minimum of
effort (for example the reengineering of a great many Capstone chips).
*/

/* The structure for storing SHS info */

/* Message digest functions */

static void SHAUpdate(Unix_SHA1_Context context, Byte *buffer, int count);
static void endian_test(int *endianness);
static void SHAtoByte(Byte *output, Unix_SHA1_UInt4 *input, unsigned int len);

/* The SHS block size and message digest sizes, in bytes */


/* The SHS f()-functions.  The f1 and f3 functions can be optimized to
   save one boolean operation each - thanks to Rich Schroeppel,
   rcs@cs.arizona.edu for discovering this */

/*#define f1(x, y, z) ((x & y) | (~x & z))		/* Rounds  0-19 */
#define f1(x ,y, z)   (z ^ (x & (y ^ z)))		/* Rounds  0-19 */
#define f2(x, y, z)   (x ^ y ^ z)			/* Rounds 20-39 */
/*#define f3(x, y, z) (( x & y) | (x & z) | (y & z))	/* Rounds 40-59 */
#define f3(x, y, z)   (( x & y) | (z & (x | y)))	/* Rounds 40-59 */
#define f4(x, y, z)   (x ^ y ^ z)			/* Rounds 60-79 */

/* The SHS Mysterious Constants */

#define K1  0x5A827999L				/* Rounds  0-19 */
#define K2  0x6ED9EBA1L				/* Rounds 20-39 */
#define K3  0x8F1BBCDCL				/* Rounds 40-59 */
#define K4  0xCA62C1D6L				/* Rounds 60-79 */

/* SHS initial values */

#define h0init  0x67452301L
#define h1init  0xEFCDAB89L
#define h2init  0x98BADCFEL
#define h3init  0x10325476L
#define h4init  0xC3D2E1F0L

/* Note that it may be necessary to add parentheses to these macros if they
   are to be called with expressions as arguments */
/* 32-bit rotate left - kludged with shifts */

#define ROTL(n, X)  (((X) << n) | ((X) >> (32 - n)))

/* The initial expanding function.  The hash function is defined over an
   80-UINT2 expanded input array W, where the first 16 are copies of the input
   data, and the remaining 64 are defined by

        W[ i ] = W[ i - 16 ] ^ W[ i - 14 ] ^ W[ i - 8 ] ^ W[ i - 3 ]

   This implementation generates these values on the fly in a circular
   buffer - thanks to Colin Plumb, colin@nyx10.cs.du.edu for this
   optimization.

   The updated SHS changes the expanding function by adding a rotate of 1
   bit.  Thanks to Jim Gillogly, jim@rand.org, and an anonymous contributor
   for this information */

#define expand(W, i) (W[i & 15] = ROTL(1, (W[i & 15] ^ W[(i - 14) & 15] ^ \
                                         W[(i - 8) & 15] ^ W[(i - 3) & 15])))


/* The prototype SHS sub-round.  The fundamental sub-round is:

        a' = e + ROTL( 5, a ) + f( b, c, d ) + k + data;
        b' = a;
        c' = ROTL( 30, b );
        d' = c;
        e' = d;

   but this is implemented by unrolling the loop 5 times and renaming the
   variables ( e, a, b, c, d ) = ( a', b', c', d', e' ) each iteration.
   This code is then replicated 20 times for each of the 4 functions, using
   the next 20 values from the W[] array each time */

#define subRound(a, b, c, d, e, f, k, data) \
    (e += ROTL(5, a) + f(b, c, d) + k + data, b = ROTL(30, b))

/* Initialize the SHS values */

Unix_SHA1_Context Unix_SHA1_Context__null = (Unix_SHA1_Context)0;

struct Unix_SHA1_Struct Unix_SHA1_Context__Initial;

void Unix_SHA1_Context__Initialize(void)
{
    Unix_SHA1_Context__reset(&Unix_SHA1_Context__Initial);
}

Unix_SHA1_Context Unix_SHA1_Context__new(void)
{
    Unix_SHA1_Context context;     

    context = (Unix_SHA1_Context)malloc(sizeof(*context));
    Unix_SHA1_Context__reset(context);
    return context;
}

Byte Unix_SHA1_Context__fetch1(
  Unix_SHA1_Context context,
  Unsigned index)
{
    if (index >= 20) {
	index = 20;
    }
    return context->final[index];
}

void Unix_SHA1_Context__reset(
  Unix_SHA1_Context context)
{
    /* Figure out the endianness of this machine: */
    endian_test(&context->endianness);

    /* Set the h-vars to their initial values: */
    context->digest[0] = h0init;
    context->digest[1] = h1init;
    context->digest[2] = h2init;
    context->digest[3] = h3init;
    context->digest[4] = h4init;

    /* Initialise bit count: */
    context->countLo = 0;
    context->countHi = 0;
}

/* Perform the SHS transformation.  Note that this code, like MD5, seems to
   break some optimizing compilers due to the complexity of the expressions
   and the size of the basic block.  It may be necessary to split it into
   sections, e.g. based on the four subrounds

   Note that this corrupts the context->data area */

static void shs_transform(
  Unix_SHA1_UInt4 *digest,
  Unix_SHA1_UInt4 *data)
{
    Unix_SHA1_UInt4 A, B, C, D, E;     /* Local vars */
    Unix_SHA1_UInt4 eData[16];       /* Expanded data */

    /* Set up first buffer and local data buffer */
    A = digest[0];
    B = digest[1];
    C = digest[2];
    D = digest[3];
    E = digest[4];
    memcpy((Unix_SHA1_Pointer)eData, (Unix_SHA1_Pointer)data, SHS_DATASIZE);

    /* Heavy mangling, in 4 sub-rounds of 20 interations each. */
    subRound(A, B, C, D, E, f1, K1, eData[0]);
    subRound(E, A, B, C, D, f1, K1, eData[1]);
    subRound(D, E, A, B, C, f1, K1, eData[2]);
    subRound(C, D, E, A, B, f1, K1, eData[3]);
    subRound(B, C, D, E, A, f1, K1, eData[4]);
    subRound(A, B, C, D, E, f1, K1, eData[5]);
    subRound(E, A, B, C, D, f1, K1, eData[6]);
    subRound(D, E, A, B, C, f1, K1, eData[7]);
    subRound(C, D, E, A, B, f1, K1, eData[8]);
    subRound(B, C, D, E, A, f1, K1, eData[9]);
    subRound(A, B, C, D, E, f1, K1, eData[10]);
    subRound(E, A, B, C, D, f1, K1, eData[11]);
    subRound(D, E, A, B, C, f1, K1, eData[12]);
    subRound(C, D, E, A, B, f1, K1, eData[13]);
    subRound(B, C, D, E, A, f1, K1, eData[14]);
    subRound(A, B, C, D, E, f1, K1, eData[15]);
    subRound(E, A, B, C, D, f1, K1, expand(eData, 16));
    subRound(D, E, A, B, C, f1, K1, expand(eData, 17));
    subRound(C, D, E, A, B, f1, K1, expand(eData, 18));
    subRound(B, C, D, E, A, f1, K1, expand(eData, 19));

    subRound(A, B, C, D, E, f2, K2, expand(eData, 20));
    subRound(E, A, B, C, D, f2, K2, expand(eData, 21));
    subRound(D, E, A, B, C, f2, K2, expand(eData, 22));
    subRound(C, D, E, A, B, f2, K2, expand(eData, 23));
    subRound(B, C, D, E, A, f2, K2, expand(eData, 24));
    subRound(A, B, C, D, E, f2, K2, expand(eData, 25));
    subRound(E, A, B, C, D, f2, K2, expand(eData, 26));
    subRound(D, E, A, B, C, f2, K2, expand(eData, 27));
    subRound(C, D, E, A, B, f2, K2, expand(eData, 28));
    subRound(B, C, D, E, A, f2, K2, expand(eData, 29));
    subRound(A, B, C, D, E, f2, K2, expand(eData, 30));
    subRound(E, A, B, C, D, f2, K2, expand(eData, 31));
    subRound(D, E, A, B, C, f2, K2, expand(eData, 32));
    subRound(C, D, E, A, B, f2, K2, expand(eData, 33));
    subRound(B, C, D, E, A, f2, K2, expand(eData, 34));
    subRound(A, B, C, D, E, f2, K2, expand(eData, 35));
    subRound(E, A, B, C, D, f2, K2, expand(eData, 36));
    subRound(D, E, A, B, C, f2, K2, expand(eData, 37));
    subRound(C, D, E, A, B, f2, K2, expand(eData, 38));
    subRound(B, C, D, E, A, f2, K2, expand(eData, 39));

    subRound(A, B, C, D, E, f3, K3, expand(eData, 40));
    subRound(E, A, B, C, D, f3, K3, expand(eData, 41));
    subRound(D, E, A, B, C, f3, K3, expand(eData, 42));
    subRound(C, D, E, A, B, f3, K3, expand(eData, 43));
    subRound(B, C, D, E, A, f3, K3, expand(eData, 44));
    subRound(A, B, C, D, E, f3, K3, expand(eData, 45));
    subRound(E, A, B, C, D, f3, K3, expand(eData, 46));
    subRound(D, E, A, B, C, f3, K3, expand(eData, 47));
    subRound(C, D, E, A, B, f3, K3, expand(eData, 48));
    subRound(B, C, D, E, A, f3, K3, expand(eData, 49));
    subRound(A, B, C, D, E, f3, K3, expand(eData, 50));
    subRound(E, A, B, C, D, f3, K3, expand(eData, 51));
    subRound(D, E, A, B, C, f3, K3, expand(eData, 52));
    subRound(C, D, E, A, B, f3, K3, expand(eData, 53));
    subRound(B, C, D, E, A, f3, K3, expand(eData, 54));
    subRound(A, B, C, D, E, f3, K3, expand(eData, 55));
    subRound(E, A, B, C, D, f3, K3, expand(eData, 56));
    subRound(D, E, A, B, C, f3, K3, expand(eData, 57));
    subRound(C, D, E, A, B, f3, K3, expand(eData, 58));
    subRound(B, C, D, E, A, f3, K3, expand(eData, 59));

    subRound(A, B, C, D, E, f4, K4, expand(eData, 60));
    subRound(E, A, B, C, D, f4, K4, expand(eData, 61));
    subRound(D, E, A, B, C, f4, K4, expand(eData, 62));
    subRound(C, D, E, A, B, f4, K4, expand(eData, 63));
    subRound(B, C, D, E, A, f4, K4, expand(eData, 64));
    subRound(A, B, C, D, E, f4, K4, expand(eData, 65));
    subRound(E, A, B, C, D, f4, K4, expand(eData, 66));
    subRound(D, E, A, B, C, f4, K4, expand(eData, 67));
    subRound(C, D, E, A, B, f4, K4, expand(eData, 68));
    subRound(B, C, D, E, A, f4, K4, expand(eData, 69));
    subRound(A, B, C, D, E, f4, K4, expand(eData, 70));
    subRound(E, A, B, C, D, f4, K4, expand(eData, 71));
    subRound(D, E, A, B, C, f4, K4, expand(eData, 72));
    subRound(C, D, E, A, B, f4, K4, expand(eData, 73));
    subRound(B, C, D, E, A, f4, K4, expand(eData, 74));
    subRound(A, B, C, D, E, f4, K4, expand(eData, 75));
    subRound(E, A, B, C, D, f4, K4, expand(eData, 76));
    subRound(D, E, A, B, C, f4, K4, expand(eData, 77));
    subRound(C, D, E, A, B, f4, K4, expand(eData, 78));
    subRound(B, C, D, E, A, f4, K4, expand(eData, 79));

    /* Build message digest */
    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
    digest[4] += E;
    }

/* When run on a little-endian CPU we need to perform byte reversal on an
   array of long words. */

static void long_reverse(
  Unix_SHA1_UInt4 *buffer,
  int byteCount,
  int endianness)
{
    Unix_SHA1_UInt4 value;

    if (endianness == 1) {
	return;
    }
    byteCount /= sizeof(Unix_SHA1_UInt4);
    while (byteCount--) {
        value = *buffer;
        value = ((value & 0xFF00FF00L) >> 8) | ((value & 0x00FF00FFL) << 8);
        *buffer++ = (value << 16) | (value >> 16);
    }
}

/* Update SHS for a block of data */

void Unix_SHA1_Context__update(
  Unix_SHA1_Context context,
  String string)
{
    Unsigned size = String__size_get(string);
    Byte *buffer;

    buffer = (Byte *)0;
    if (string->literal.size == 255) {
	String__gap_set(string, size);
	buffer = string->header.buffer;
    } else {
	buffer = &string->literal.data[0];
    }
    SHAUpdate(context, buffer, size);
}

static void SHAUpdate(
  Unix_SHA1_Context context,
  Byte *buffer,
  int count)
{
    Unix_SHA1_UInt4 tmp;
    int dataCount;

    /* Update bitcount */
    tmp = context->countLo;
    if ((context->countLo = tmp + ((Unix_SHA1_UInt4)count << 3)) < tmp) {
        context->countHi++;             /* Carry from low to high */
    }
    context->countHi += count >> 29;

    /* Get count of bytes already in data */
    dataCount = (int)(tmp >> 3) & 0x3F;

    /* Handle any leading odd-sized chunks */
    if (dataCount) {
        Byte *p = (Byte *)context->data + dataCount;

        dataCount = SHS_DATASIZE - dataCount;
        if (count < dataCount) {
            memcpy(p, buffer, count);
            return;
	}
        memcpy(p, buffer, dataCount);
        long_reverse(context->data, SHS_DATASIZE, context->endianness);
        shs_transform(context->digest, context->data);
        buffer += dataCount;
        count -= dataCount;
    }

    /* Process data in SHS_DATASIZE chunks */
    while (count >= SHS_DATASIZE) {
        memcpy((Unix_SHA1_Pointer)context->data,
	  (Unix_SHA1_Pointer)buffer, SHS_DATASIZE);
        long_reverse(context->data, SHS_DATASIZE, context->endianness);
        shs_transform(context->digest, context->data);
        buffer += SHS_DATASIZE;
        count -= SHS_DATASIZE;
    }

    /* Handle any remaining bytes of data. */
    memcpy((Unix_SHA1_Pointer)context->data, (Unix_SHA1_Pointer)buffer, count);
}

/* Final wrapup - pad to SHS_DATASIZE-byte boundary with the bit pattern
   1 0* (64-bit count of bits processed, MSB-first) */

void Unix_SHA1_Context__final(
  Unix_SHA1_Context context)
{
    int count;
    Byte *data_pointer;

    /* Compute number of bytes mod 64 */
    count = (int) context->countLo;
    count = (count >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    data_pointer = (Byte *)context->data + count;
    *data_pointer++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = SHS_DATASIZE - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(data_pointer, 0, count);
        long_reverse(context->data, SHS_DATASIZE, context->endianness);
        shs_transform(context->digest, context->data);

        /* Now fill the next block with 56 bytes */
        memset((Unix_SHA1_Pointer)context->data, 0, SHS_DATASIZE - 8);
    } else {
        /* Pad block to 56 bytes */
        memset(data_pointer, 0, count - 8);
    }

    /* Append length in bits and transform */
    context->data[14] = context->countHi;
    context->data[15] = context->countLo;

    long_reverse(context->data, SHS_DATASIZE - 8, context->endianness);
    shs_transform(context->digest, context->data);

    /* Output to an array of bytes */
    SHAtoByte(context->final, context->digest, SHS_DIGESTSIZE);

    /* Zeroise sensitive stuff */
    /* memset((Unix_SHA1_Pointer)context, 0, sizeof(context)); */
    Unix_SHA1_Context__reset(context);
}

static void SHAtoByte(
  Byte *output,
  Unix_SHA1_UInt4 *input,
  unsigned int len)
{
    /* Output SHA digest in byte array */
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
	output[j+3] = (Byte)( input[i]        & 0xff);
	output[j+2] = (Byte)((input[i] >> 8)  & 0xff);
	output[j+1] = (Byte)((input[i] >> 16) & 0xff);
	output[j]   = (Byte)((input[i] >> 24) & 0xff);
    }
}

unsigned char digest[20];
unsigned char message[3] = {'a', 'b', 'c' };
unsigned char *mess56 = 
	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

/* Correct solutions from FIPS PUB 180-1 */
char *dig1 = "A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D";
char *dig2 = "84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1";
char *dig3 = "34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F";

/* Output should look like:-
 a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d
 A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D <= correct
 84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1
 84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1 <= correct
 34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f
 34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F <= correct
*/

Integer Unix_SHA1_Context__test(void)
{
    struct Unix_SHA1_Struct sha;
    int i;
    Byte big[1000];
    Unix_SHA1_Context context = &sha;

    Unix_SHA1_Context__reset(context);
    SHAUpdate(context, message, 3);
    Unix_SHA1_Context__final(context);

    for (i = 0; i < 20; i++) {
	if ((i % 4) == 0) {
	    printf(" ");
	}
	printf("%02x", (unsigned int)context->final[i]);
    }
    printf("\n");
    printf(" %s <= correct\n", dig1);

    Unix_SHA1_Context__reset(context);
    SHAUpdate(context, mess56, 56);
    Unix_SHA1_Context__final(context);

    for (i = 0; i < 20; i++) {
	if ((i % 4) == 0) {
	    printf(" ");
	}
	printf("%02x", (unsigned int)context->final[i]);
    }
    printf("\n");
    printf(" %s <= correct\n", dig2);

    /* Fill up big array */
    for (i = 0; i < 1000; i++) {
	big[i] = 'a';
    }

    Unix_SHA1_Context__reset(context);
    /* Digest 1 million x 'a' */
    for (i = 0; i < 1000; i++) {
	SHAUpdate(context, big, 1000);
    }
    Unix_SHA1_Context__final(context);

    for (i = 0; i < 20; i++) {
	if ((i % 4) == 0) {
	    printf(" ");
	}
	printf("%02x", (unsigned int)context->final[i]);
    }
    printf("\n");
    printf(" %s <= correct\n", dig3);

    return 0;
}

static void endian_test(
  int *endian_ness)
{
    if ((*(unsigned short *) ("#S") >> 8) == '#') {
	/* printf("Big endian = no change\n"); */
	*endian_ness = 1;
    } else {
	/* printf("Little endian = swap\n"); */
	*endian_ness = 0;
    }
}


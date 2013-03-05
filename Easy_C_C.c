/*
 * C run-time library for basic EZC types.
 *
 * Copyright (c) 2007-2008 by Wayne C. Gramlich
 * All rights reserved.
 */

#define STDIO_H_MACROS_NEEDED 1

#include "Easy_C.h"
#include "Easy_C_C.h"

#include <string.h>

Unsigned OGLS_Commands__address_get(
  void *commands)
{
    return (Unsigned)commands;
}

static String easy_c_file_string = (String)"\012Easy_C_C.c";

static void array_fail(
  Unsigned line_number)
{
    System__fail((String)"\005Array", easy_c_file_string, line_number);
}		       

/* {Array} routines: */

void Array__Initialize(void)
{
    /* do nothing */
}

Array Array__new(void)
{
    Array array;

    array = (Array)malloc(sizeof (Array *));
    array->size = 0;
    array->limit = 1;	
    array->data = (void **)malloc(sizeof (void *));
    if (array->limit >= 10000000) {
	array_fail(__LINE__);
    }
    return array;
}

void Array__erase(
  Array array)
{
    array->size = 0;
}

void Array__append(
  Array array,
  void *element)
{
    Unsigned limit;
    Unsigned size;
    void **data;

    /* Make sure we have enough room: */
    limit = array->limit;
    size = array->size;
    data = array->data;
    if (size >= limit) {
	limit <<= 1;
	data = (void **)realloc(data, limit * sizeof(void *));
	if (data == (void **)0) {
	    (void)fprintf(stderr,
	      "Unable to grow Array to size %d\n", size);
	    array_fail(__LINE__);
	}
	array->data = data;
	array->limit = limit;
    }
    data[size] = element;
    array->size = size + 1;
}

void *Array__fetch1(
  Array array,
  Unsigned index)
{
    if (index >= array->size) {
	(void)fprintf(stderr,
	  "Array index (%d) >= array size (%d)\n", index, array->size);
	array_fail(__LINE__);
    }
    return array->data[index];
}

void Array__range_delete(
  Array array,
  Unsigned index,
  Unsigned count)
{
    void **data;
    Unsigned size;

    data = array->data;
    size = array->size;

    if (index + count > size) {
	array_fail(__LINE__);
    }
    data += index;
    (void)memmove(data, data + count, (size - index - count) * sizeof(void *));
    array->size = size - count;
}

void Array__range_insert(
  Array array,
  Unsigned index,
  Unsigned count,
  void *element)
{
    void **data;
    Unsigned limit;
    Unsigned size;

    /* Make sure we have enough room: */
    limit = array->limit;
    size = array->size;
    data = array->data;
    if (size + count > limit) {
	while (size + count > limit) {
	    limit <<= 1;
	}
	data = (void **)realloc(array->data, limit * sizeof(void *));
	if (data == (void **)0) {
	    array_fail(__LINE__);
	}
	array->data = data;
	array->limit = limit;
    }
    array->size = size + count;

    /* Make room for the new data elements: */
    data += index;
    (void)memmove(data + count, data, (size - index) * sizeof(void *));

    /* Fill in the new locations with {element}: */
    while (count != 0) {
	*data++ = element;
	count--;
    }
}

Unsigned Array__size_get(
  Array array)
{
    return array->size;
}

void Array__store1(
  Array array,
  Unsigned index,
  void *element)
{
    if (index >= array->size) {
	array_fail(__LINE__);
    }
    array->data[index] = element;
}


void Array__transfer(
  Array to_array,
  Unsigned to_index,
  Array from_array,
  Unsigned from_index,
  Unsigned count)
{
    if (to_index + count > to_array->size ||
      from_index + count > from_array->size) {
	array_fail(__LINE__);
    }
    (void)memmove(to_array->data + to_index, from_array->data + from_index,
      count * sizeof(void *));
}

void Array__trim(
  Array array,
  Unsigned new_size)
{
    if (new_size > array->size) {
	array_fail(__LINE__);
    }
    array->size = new_size;
}


/* {Byte} stuff: */

Byte Byte__null = 0;

void Byte__erase(
  Byte byte)
{
    /* Do nothing */
}

void Byte__Initialize(void)
{
    /* Do nothing */
}

/* {Character} stuff: */

Character Character__null = 0;

void Character__Initialize(void)
{
    /* Do nothing */
}

void Character__put(
  Character character,
  Out_Stream out_stream)
{
    (void)fputc(character, out_stream);
}

/* {Double} stuff: */

void Double__Initialize(void)
{
    /* Do nothing */
}

void Double__erase(
  Double number)
{
    /* Do nothing */
}

Double Double__null = 0.0;

void Double__f_helper(
  Double x,
  Unsigned precision,
  Character character,
  String result)
{
    char format[100];
    char buffer[1000];

    if (precision == 0xffffffff) {
	/* No precision specified */
	(void)sprintf(format, "%%%c", character);
    } else {	
	/* Precision specified */
	(void)sprintf(format, "%%.%d%c", precision, character);
    }
    (void)sprintf(buffer, format, x);
    String__unix_append(result, buffer);
}

/* {Easy_C} stuff:*/

void Easy_C__Initialize(void)
{
    In_Stream__Initial = *stdin;
    In_Stream__null = stdin;
    In_Stream__standard = stdin;

    Out_Stream__Initial = *stderr;
    Out_Stream__null = stderr;
    Out_Stream__error = stderr;
    Out_Stream__standard = stdout;

    System__null->error_out_stream = stderr;
    System__null->standard_out_stream = stdout;
    System__null->standard_in_stream = stdin;
}

void Easy_C__abort(void)
{
    abort();
}


/* {Float} stuff: */

Float Float__null = 0.0f;

void Float__Initialize(void)
{
    /* Do Nothing */
}

void Float__erase(
  Float number)
{
    /* Do Nothing */
}

Unsigned Float__hash(
  Float number)
{
  union {Float f; Unsigned u;} n;

  n.f = number;
  return n.u;
}

/* {Integer} stuff: */

void Integer__Initialize(void)
{
    /* Do nothing */
}

int Integer__null = 0;
int Integer__zero = 0;
int Integer__one = 1;
int Integer__negative_one = -1;
int Integer__maximum = 0x7fffffff;
int Integer__minimum = 0x80000000;


/* {In_Stream} stuff: */

FILE In_Stream__Initial;

/* FIXME: remove!!! */
In_Stream In_Stream__Null;
In_Stream In_Stream__standard;


void In_Stream__Initialize(void)
{
    In_Stream__Null = stdin;  
    In_Stream__standard = stdin;
}

Character In_Stream__character_read(
  In_Stream in_stream)
{
    return fgetc(in_stream);
}

void In_Stream__character_unread(
  In_Stream in_stream,
  Character character)
{
    ungetc(character, in_stream);
}

void In_Stream__close(
  In_Stream in_stream)
{
    (void)fclose(in_stream);
}

In_Stream In_Stream__fd_open(
  Integer fd)
{
    In_Stream result;

    result = fdopen(fd, "r");
    if (result == (In_Stream)0) {
	result = In_Stream__Null;
    }
    return result;
}


In_Stream In_Stream__open(
  String file_name)
{
    In_Stream result;

    result = fopen(String__Unix(file_name), "ra");
    if (result == (In_Stream)0) {
	result = In_Stream__Null;
    }
    return result;
}

In_Stream In_Stream__pipe_read(
  String command)
{
    In_Stream result;

    result = popen(String__Unix(command), "r");
    if (result ==  (In_Stream)0) {
	result = In_Stream__Null;
    }
    return result;
}

/* {Logical} stuff: */

Logical Logical__null = 0;

Logical Logical__true = 1;

Logical Logical__false = 0;

void Logical__Initialize(void)
{
    /* Do Nothing */
}

/* {Long_Integer} stuff: */

void Long_Integer__Initialize(void)
{
    /* Do nothing */
}

Long_Integer Long_Integer__null = 0ll;
Long_Integer Long_Integer__zero = 0ll;
Long_Integer Long_Integer__one = 1ll;
Long_Integer Long_Integer__negative_one = -1ll;
Long_Integer Long_Integer__maximum = 0x7fffffffffffffffll;
Long_Integer Long_Integer__minimum = 0x8000000000000000ll;


/* {Long_Unsigned} stuff: */

void Long_Unsigned__Initialize(void)
{
    /* Do nothing */
}

Long_Unsigned Long_Unsigned__null = 0ull;
Long_Unsigned Long_Unsigned__zero = 0ull;
Long_Unsigned Long_Unsigned__one = 1ull;
Long_Unsigned Long_Unsigned__maximum = 0xffffffffffffffffull;
Long_Unsigned Long_Unsigned__minimum = 0ull;


/* {Out_Stream} routines: */

FILE Out_Stream__Initial;

/* FIXME: remove!!! */
Out_Stream Out_Stream__Null;

Out_Stream Out_Stream__null;

Out_Stream Out_Stream__standard;

Out_Stream Out_Stream__error;

void Out_Stream__Initialize(void)
{
    Out_Stream__Null = stderr;
    Out_Stream__error = stderr;
    Out_Stream__standard = stdout;
}

void Out_Stream__close(
  Out_Stream out_stream)
{
    (void)fclose(out_stream);
}

Out_Stream Out_Stream__fd_open(
  Integer fd)
{
    Out_Stream result;

    result = fdopen(fd, "w");
    if (result == (Out_Stream)0) {
	result = Out_Stream__Null;
    }
    return result;
}


void Out_Stream__flush(
  Out_Stream out_stream)
{
    (void)fflush(out_stream);
}

Out_Stream Out_Stream__open(
  String file_name)
{
    Out_Stream result;

    result = fopen(String__Unix(file_name), "w");
    if (result == (Out_Stream)0) {
	result = Out_Stream__Null;
    }
    return result;
}

Out_Stream Out_Stream__pipe_write(
  String command)
{
    Out_Stream result;

    result = popen(String__Unix(command), "w");
    if (result == (Out_Stream)0) {
	result = Out_Stream__Null;
    }
    return result;
}

/* {Short} stuff: */

void Short__Initialize(void)
{
    /* Do nothing */
}

Short Short__null = 0;


/* {String} stuff: */

union String__Union String__Initial;

String String__null = (String)&String__Initial;

void String__Initialize(void)
{
    /* Do Nothing */
}

static void string_fail(
  Unsigned line_number)
{
    System__fail((String)"\006String", easy_c_file_string, line_number);
}

void String__erase(
  String string)
{
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	string->header.total_size = 0;
	string->header.front_size = 0;
    }
}

String String__new(void)
{
    String string;
    unsigned char *buffer;

    string = (String)malloc(sizeof *string);
    buffer = (unsigned char *)malloc(4);
    if (string == (String)0 || buffer == (unsigned char *)0) {
	string_fail(__LINE__);
    }
    string->header.size = 255;
    string->header.limit = 4;
    string->header.total_size = 0;
    string->header.front_size = 0;
    string->header.buffer = buffer;
    return string;
}

void String__unix_append(
  String string,
  char *text)
{
    /* This routine will append {text} to {string}. */

    char chr;

    while ((chr = *text++) != '\0') {
	String__character_append(string, chr);
    }
}

unsigned char *String__Unix(
  String string)
{
    /* This routine returns a pointer to Unix style null-terminated string. */

    Unsigned total_size;
    Unsigned limit;
    unsigned char *result;

    if (string->literal.size == 255) {
	/* Buffered string: */
	total_size = string->header.total_size;
	limit = string->header.limit;
	if (total_size == limit) {
	    /* Bummer; there is no room for the terminatin null: */
	    String__Realloc(string, total_size + 4);
	}
	String__gap_set(string, total_size);
	result = string->header.buffer;
	result[total_size] = '\0';
    } else {
	/* Literal string: */
	result = &string->literal.data[0];
    }
    return result;
}

void String__gap_set(
  String string,
  Unsigned gap)
{
    Unsigned total_size;
    Unsigned front_size;
    Unsigned back_size;
    unsigned char *buffer;
    Unsigned delta;
    Unsigned limit;

    total_size = string->header.total_size;
    front_size = string->header.front_size;

    if (string->literal.size != 255 ||
      gap > total_size || front_size > total_size) {
	string_fail(__LINE__);
    }
    back_size = total_size - front_size;
    limit = string->header.limit;
    if (total_size > limit) {
	string_fail(__LINE__);
    }
    buffer = string->header.buffer;
    if (gap < front_size) {
	delta = front_size - gap;
	(void)memmove(buffer + limit - back_size - delta, buffer + gap, delta);
    } else if (gap > front_size) {
	delta = gap - front_size;
	(void)memmove(buffer + front_size, buffer + limit - back_size, delta);
    }
    string->header.front_size = gap;
}

void String__Realloc(
  String string,
  Unsigned new_limit)
{
    unsigned size;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	Unsigned total_size;

	total_size = string->header.total_size;
	if (total_size < new_limit) {
	    /* Everything will fit in the new buffer; */
	    unsigned char *buffer;
	    Unsigned limit;
	    Unsigned total_size;
	    Unsigned front_size;
	    Unsigned back_size;

	    total_size = string->header.total_size;
	    front_size = string->header.front_size;
	    back_size = total_size - front_size;
	    buffer = string->header.buffer;
	    limit = string->header.limit;
	    if (new_limit < limit) {
		/* Buffer is getting smaller: */
		/* Move the back string forward; then reallocate: */
		(void)memmove(buffer + new_limit - back_size,
		  buffer + limit - back_size, back_size);
		buffer = (unsigned char *)realloc((void *)buffer, new_limit);
		if (buffer == (unsigned char *)0) {
		    string_fail(__LINE__);
		}
		string->header.buffer = buffer;
		string->header.limit = new_limit;
	    } else if (new_limit > limit) {
		/* Buffer is getting larger: */
		/* Realloacate; then move the back string backward: */
		buffer = (unsigned char *)realloc((void *)buffer, new_limit);
		if (buffer == (unsigned char *)0) {
		    string_fail(__LINE__);
		}
		string->header.buffer = buffer;
		(void)memmove(buffer + new_limit - back_size,
		  buffer + limit - back_size, back_size);
		string->header.limit = new_limit;
	    }
	} else {
	    string_fail(__LINE__);
	}
    } else {
	/* Literal string */
	string_fail(__LINE__);
    }
}

Double String__double_convert(
  String string)
{
    return atof(String__Unix(string));
}

Float String__float_convert(
  String string)
{
    return (float)(String__double_convert(string));
}

unsigned String__fetch1(
  String string,
  unsigned index)
{
    Unsigned size;
    Unsigned character;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string */
	Unsigned total_size;

	total_size = string->header.total_size;
	if (index < string->header.total_size) {
	    Unsigned front_size;
	    unsigned char *buffer;

	    buffer = string->header.buffer;
	    front_size = string->header.front_size; 
	    if (index < front_size) {
		character = buffer[index];
	    } else {
		character = buffer[string->header.limit - total_size + index];
	    }
	} else {
	    Unsigned i;
	    (void)fputc('"', stderr);
	    for (i = 0; i < total_size; i++) {
		unsigned char c;
		c = (unsigned char)String__fetch1(string, i);
		if (' ' <= c && c <= '~' && c != '\\') {
		    (void)fputc(c, stderr);
		} else {
		    (void)fprintf(stderr, "\\%d\\", c);
		}
	    }
	    (void)fputc('"', stderr);
	    (void)fprintf(stderr, "size=%d index=%d\n", total_size, index);
	    string_fail(__LINE__);
	}
    } else {
	/* String Literal */
	if (index < size) {
	    character = string->literal.data[index];
	} else {
	    string_fail(__LINE__);
	}
    }
    return character & 0xff;
}

Unsigned String__limit_get(
  String string)
{
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	size = string->header.limit;
    }
    return size;
}

Unsigned String__front_size_get(
  String string)
{
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	size = string->header.front_size;
    }
    return size;
}

Logical String__is_buffered(
  String string)
{
    return string->literal.size != 255;
}

Logical String__is_literal(
  String string)
{
    return string->literal.size == 255;
}

#ifdef UNFINISHED
void String__range_copy(
  String to_string,
  Unsigned to_offset,
  String from_string,
  Unsigned from_offset,
  Unsigned count)
{
    Unsigned from_size;
    Unsigned from_front_size;
    Unsigned to_size;
    Unsigned to_front_size;
    unsigned char *to_buffer;

    assert (to_string->literal.size == 255);
    to_size = to_string->header.total_size;
    assert (to_offset + count < to_size);
    to_front_size = to_string->header.front_size;
    to_buffer = to_string->header.buffer;

    from_size = from_string->literal.size;
    if (from_size  == 255) {
	/* {from_string} is buffered: */
	from_size = from_string->header.total_size;
	from_front_size = from_string->header.front_size;
    } else {
	/* {from_string} is a literal: */
	Unsigned front_count;
	Unsigned back_count;

	assert (from_offset + count < from_size);
	if (to_offset < to_front_size) {
	    /* At least one byte will land in front: */
	    if (to_offset + count <= to_front_size) {
		/* Everything fits into the front: */
		front_count = count;
	    } else {
		/* Both front and back are spanned: */
		front_count = to_front_size - to_offset;
	    }
	} else {
	    /* Everything fits into the back: */
	    front_count = 0;
	}
	back_count = count - front_count;

	if (front_count != 0) {
	    (void)memmove(to_buffer + to_offset,
	      from_string->literal.data + from_offset, front_count);
	}
	if (back_count != 0) {
	    assert (0);
	    (void)memmove(to_buffer, from_string->literal.data + from_offset + front_count, back_count);
	}
    }
}  
#endif /* UNFINISHED */

void String__range_delete(
  String string,
  Unsigned offset,
  Unsigned count)
{
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	size = string->header.total_size;
	if (offset + count > size) {
	    string_fail(__LINE__);
	}
	String__gap_set(string, offset);
	string->header.total_size = size - count;
    } else {
	/* Literal string: */
	string_fail(__LINE__);
    }
}  

void String__range_nulls_insert(
  String string,
  Unsigned offset,
  Unsigned count)
{
    Unsigned size;
    Unsigned back_size;
    Unsigned front_size;
    unsigned char *buffer;
    Unsigned limit;
    Unsigned total_size;
    Unsigned new_limit;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	total_size = string->header.total_size;
	if (offset > total_size) {
	    string_fail(__LINE__);
	}
	String__gap_set(string, offset);
	front_size = string->header.front_size;
	if (front_size > total_size) {
	    string_fail(__LINE__);
	}
	back_size = total_size - front_size;
	limit = string->header.limit;
	buffer = string->header.buffer;
	if (total_size > limit) {
	    string_fail(__LINE__);
	}

	new_limit = limit;
	while (total_size + count > new_limit) {
	    new_limit <<= 1;
	}
	if (limit != new_limit) {
	    String__Realloc(string, new_limit);
	    limit = string->header.limit;
	    buffer = string->header.buffer;
	}
	(void)memset(buffer + front_size, 0, count);
	string->header.front_size = front_size + count;
	string->header.total_size = total_size + count;
    } else {
	/* Literal string: */
	string_fail(__LINE__);
    }
}  

Unsigned String__read(
  String string,
  Unsigned offset,
  Unsigned amount,
  In_Stream in_stream)
{
    Unsigned total_size;
    Unsigned result;

    if (string->literal.size = 255) {
	/* Buffered String: */
	total_size = string->header.total_size;
	if (offset + amount <= total_size) {
	    String__gap_set(string, total_size);
	    result = fread(string->header.buffer + offset,
	      1, amount, in_stream);
	} else {
	    string_fail(__LINE__);
	}
    } else {
	/* Literal String: */
	string_fail(__LINE__);
    }
    return result;
}

String String__read_only_copy(
  String string)
{
    String result;
    Unsigned total_size;

    if (string->literal.size == 255) {
	/* Buffered String: */
	total_size = string->header.total_size;     
	if (total_size < 255) {
	    /* {string} is small enough to represent as a literal string: */
	    String__gap_set(string, total_size);
	    result = (String)malloc(total_size + 2);
	    (void)memcpy((void *)&result->literal.data[0],
	      (void *)(string->header.buffer), total_size);
	    result->literal.size = total_size;
	    result->literal.data[total_size] = '\0';
	} else {
	    /* We'll just make a copy: */
	    result = String__new();
	    String__string_append(result, string);
	}
    } else {
	/* Literal String: */
	result = string;
    }    
    return result;
}


Unsigned String__size_get(
  String string)
{
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	size = string->header.total_size;
    } else {
	/* Literal string: */
    }
    return size;
}

void String__store1(
  String string,
  Unsigned index,
  Unsigned character)
{
    Unsigned size;

    size = string->literal.size;
    if (size == 255) {
	/* Buffered string: */
	Unsigned total_size;

	total_size = string->header.total_size;
	if (index < total_size) {
	    Unsigned front_size;
	    unsigned char *buffer;

	    buffer = string->header.buffer;
	    front_size = string->header.front_size; 
	    if (index < front_size) {
		buffer[index] = character;
	    } else {
		buffer[string->header.limit - total_size + index] = character;
	    }
	} else {
	    string_fail(__LINE__);
	}
    } else {
	/* Literal string: */
	if (index < size) {
	    string->literal.data[index] = character;
	} else {
	    string_fail(__LINE__);
	}
    }
}

/* {System} stuff: */

void System__abort(void)
{
    abort();
}

Integer System__execute(
  String command)
{
    return system(String__Unix(command));
}

void System__exit(
  Unsigned result)
{
    exit(result);
}


/* {System_Jump_Buffer} stuff: */

System_Jump_Buffer System_Jump_Buffer___initial;
System_Jump_Buffer System_Jump_Buffer__null;

void System_Jump_Buffer__Initialize(void)
{
    /* Do nothing */
}


/* {Unsigned} stuff: */

Unsigned Unsigned__null = 0;

Unsigned Unsigned__maximum = 0xffffffff;

void Unsigned__Initialize(void)
{
    /* Do Nothing */
}

/* {Void} stuff: */

void Void__Initialize(void)
{
    /* Do Nothing */
}


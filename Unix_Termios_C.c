/*
 * Copyright (c) 2007 by Wayne C. Gramlich
 * All rights reserved.
 */

#include "Easy_C.h"
#include "Easy_C_C.h"
#include <termios.h>
#include "Unix_Termios.h"
#include "Unix_Termios_C.h"

int Unix_Termios__fetch1(
  Unix_Termios termios,
  Integer index)
{
    int result;

    result = -1;
    if (0 <= index && index < NCCS) {
	termios->c_cc[index];
    }
    return result;
}

void Unix_Termios__store1(
  Unix_Termios termios,
  int index,
  int value)
{
    if (0 <= index && index < NCCS) {
	termios->c_cc[index] = value;
    }
}


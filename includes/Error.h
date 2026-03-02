#ifndef _ERROR_H__
#define _ERROR_H__

#include <stdbool.h>

#include "Token.h"

   /* Centralized Error Flags */
/* ----------  Flags ----------- */

static bool WARN = false;
static bool WERROR = false;
static bool WALL = false;
static bool WEXTRA = false;

char* TokenToStr(TokenType type);

#endif
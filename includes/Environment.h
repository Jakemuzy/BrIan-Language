#ifndef _ENVIRONMENT_H__
#define _ENVIRONMENT_H__

#include "Dict.h"
#include "Type.h"

/* ----------- Environments ---------- */

typedef struct EnvironmentEntry {
    enum { ENV_VAR_ENTRY, ENV_FUNC_ENTRY } kind;
    union {
        struct { TYPE* ty; } var;
        struct { TYPE_LIST* formals; TYPE* result; } func;
    } u;
} EnvironmentEntry;

EnvironmentEntry ENV_VarEntry(TYPE* ty);
EnvironmentEntry ENV_FuncEntry(TYPE_LIST* formals, TYPE* result);

/* TODO: Need to use my own map of linked lists that store void* */
Dict ENV_BaseTenv(); /* Base Type Environment (ie int -> TY_INT ... )*/
Dict Env_BaseVenv(); /* Base Variable Environment (TODO: will contain predefined functions)*/

#endif
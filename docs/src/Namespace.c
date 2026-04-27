#include "Namespace.h"

/* ----- Namespaces ----- */

Namespaces* InitalizeNamespaces(Arena* arena) 
{
   Namespaces* nss = malloc(sizeof(Namespaces));
   nss->ns[0] = InitalizeEnvironment(arena, N_VAR);
   nss->ns[1] = InitalizeEnvironment(arena, N_TYPE);

   return nss;
}

/* ----- Scopes ----- */

void EnterScope(Arena* arena, Namespaces* nss)
{
   for (int i = 0; i < NS_COUNT; i++) {
      Environment* env = nss->ns[i];
      Environment* newEnv = InitalizeEnvironment(arena, env->nskind);
      newEnv->prev = env;

      nss->ns[i] = newEnv;
   }
}

void ExitScope(Namespaces* nss)
{
   for (int i = 0; i < NS_COUNT; i++) {
      Environment* env = nss->ns[i];

      Environment* prev = env->prev;
      DestroyEnvironment(env);

      nss->ns[i] = prev;
   }
}
#ifndef _TYPE_CHECKER_H__
#define _TYPE_CHECKER_H__

#include <stdbool.h>

#include "NameResolver.h"
#include "Environment.h"
#include "OperatorRules.h"

/* 
    TODO: 
        Disallow divide by 0
        Cap Integers to max and min
        Environment will need to track more if safety is guaranteed (ownership, lifetime, mutability, etc)
        Implement closures (For lambda functions)
*/

/* ---------- Type Semantic Analysis ----------- */

TYPE* TypeCheck(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckVarDecl(Namespaces* nss, ASTNode* expr); /* TODO: assign type to symbols (currently null) */
TYPE* TypeCheckArrDecl(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckVar(Namespaces* nss, ASTNode* var, TYPE* type);
TYPE* TypeCheckArrInit(Namespaces* nss, ASTNode* decl);
TYPE* TypeCheckFunc(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckParams(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckParam(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckBinExpr(Namespaces* nss, ASTNode* expr);  
TYPE* TypeCheckUnaExpr(Namespaces* nss, ASTNode* expr);  
TYPE* TypeCheckAsgn(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckArrIndex(Namespaces* nss, ASTNode* decl);
TYPE* TypeCheckCallFunc(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckTypedef(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckEnumBody(Namespaces* nss, ASTNode* expr);

/* Errors in OperatorRules.h */

#endif 
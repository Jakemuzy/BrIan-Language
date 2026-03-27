#ifndef _TYPE_CHECKER_H__
#define _TYPE_CHECKER_H__

#include <stdbool.h>

#include "Error.h"
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
TYPE* TypeCheckFunc(Namespaces* nss, ASTNode* expr);
    TYPE* TypeCheckParams(Namespaces* nss, ASTNode* expr);
    TYPE* TypeCheckParam(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckBinExpr(Namespaces* nss, ASTNode* expr);  
TYPE* TypeCheckUnaExpr(Namespaces* nss, ASTNode* expr);  
TYPE* TypeCheckAsgn(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckWhileLoop(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckDoWhileLoop(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckForLoop(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckIfStmt(Namespaces* nss, ASTNode* expr);
    TYPE* TypeCheckIfElif(Namespaces* nss, ASTNode* expr);
    TYPE* TypeCheckElse(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckSwitchStmt(Namespaces* nss, ASTNode* expr);
    TYPE* TypeCheckCase(Namespaces* nss, ASTNode* expr);
    TYPE* TypeCheckDefault(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckReturnStmt(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckMemberAccess(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckArrInitializer(Namespaces* nss, ASTNode* decl);
TYPE* TypeCheckStructInitalizer(Namespaces* nss, ASTNode* decl);

TYPE* TypeCheckArrIndex(Namespaces* nss, ASTNode* decl);
TYPE* TypeCheckCallFunc(Namespaces* nss, ASTNode* expr);

TYPE* TypeCheckTypedef(Namespaces* nss, ASTNode* expr);
TYPE* TypeCheckStructDecl(Namespaces* nss, ASTNode* expr);
TYPE_FIELD_LIST* TypeCheckStructBody(Namespaces* nss, ASTNode* expr);
TYPE_FIELD_LIST* TypeCheckEnumBody(Namespaces* nss, ASTNode* expr);

/* Errors in OperatorRules.h */

#endif 

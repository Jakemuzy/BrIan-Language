## General
* Visual diagrams for documantation
* Type system explanation
* Have a better way to track bugs, progress, things to do, etc
* Give steps on how to build
* Convert to using cmake instead of bash X
* Use a dedicated tools for test cases
* Use clang instead of gcc ( llvm support )
* Run the program with werror, wall, wextra
* Give solutions on how BrIan fixes C pain points in the documentation
* Concurrency design needs to be accounted for both in the compiler code and the language semantics
* Interactions between qualifiers not specified
* Remove signed and unsigned type qualifiers

## Pipeline Interactions
* Once typechecker finished, namespaces aren't used again (discared), although this works since type is stored in the symbol, it could lead to issues in later steps

## Grammar
* Allow pointers symbol before idents ( including in vardecl ) X
* Allow dereference symbol before previously declared idents ( not vardecls ) X
* Change type to allow arbitrary idents ( custom defined types, will check validity in later stages )
* Idents can be custom types, need to allow grammar to use custom types 
* Maybe two string types? Mutable and immutable X
* Add break and return statements X
* Add true and false ( built into the language ) X
* Simplify switch statements, allow only literals / constants ( jump table optimization ) x
* Handle directives better ( #include, etc ) X
* Strings may have to be pointers, maybe constant? ( unsure, lookup why C strings suck )
* Design a way to handle generics X
* Allow operator overloading for structs X
* Think about how interfaces will be implemented X
* Allow type qualifiers X
* Build Ternary operator X
* Some stmts like ForStmt are confusing X
* VarExprList in for stmt allows both exprs and var decls, a bit weird X
* Sub rules such as if elif and else should be removed X
* Should gotos be added? makes desugaring easier X
* Hex type ONLY for @ ( which is memory location ) X
* Mayhaps matricies as a built in type? ( doubtful ) X
* '%' symbol for registers (ie U32% r1 = 0x...), inheriently volatile X
* interfaces as a key word X
* default struct member variables X
* Matrices and vectors as built in types X
* Func declarations vs definitions?  X 
* Maybe some bit masking tokens?



## Error handling
* Need to have a centralized area for error handling
* Error needs to propogate properly
* Erroring should have a recovery system
* Warnings should also be allowed 
* Maybe have an enum for ERROR_TYPE, and pass that as well as multiple flags casted to void* for variadic error message handling

## General Design
* Need to think of how strings will work for embedded systems (don't want just pointers)
* Characters as U8? 
* Floats as 32 bit doubles as 64 bit
* Void meaning lack of a type 
* Callee and Caller needs to be specified, otherwise return function won't have any idea of where are what its returning

## Tokenizer
* Line numbers are handled in a fragile and confusing manner
* Needs to track columns
* Keywords are analyzed via a weird map system
* State should not be global or mutable
* Don't use macros for getting characters 
* Lexeme never frees ( memory leak )
* Lexeme grows unbounded, MAX_LEXEME exists but a single token can expand past this 
* No bounds checking for updating lexeme
* Should use state machine instead of if-else chain 
* Comments are not handled elegantly block comments bug out sometimes 
* Escape characters ( \n \t etc ) are not handled X
* Comments are discarded here 
* DFA table for operators, hash map for keywords, manual for rest
* Buffer size of 4096 bytes to reduce syscalls overhead 
* Double buffer system 

## Preprocessor
* Resolve typedefs ( prevents delayed custom type checking )
* Resolve file includes
* Maybe function overloading???
* conditional compilation
* Don't want preprocessor to be a monolithic god file for all stages preprocessing, maybe have different multi step phases for differnt stages of the pipeline?

## Parser
* Lots of boilder plate for binary operators 
* Ambiguity resolution is fragile ( 3 token lookahead is done for every function )
* Put token back is a buffer but can grow unbounded
* Some AST Nodes are redudnant, dont need list wrapper nodes
* AST nodes that represent parse tree levels (STMT_LIST_NODE) should be removed
* AST should include a CAST_NODE, its currently treated as a unary operator (its not)
* No distinction between prefix and postfix operators 
* Should only parse Type ( not idents ) since the type checking is relocated to later stages, will verify typedefs in the preprocessor
* GetNextTokenP (preporcessor is fragile)

## Name Resolver 
* Shouldn't be doing partial type checking, type checking redoes the work ( given in terms of sym->stype, but I think this is necessary for verifiying names inside of structs )
* Scope context is kind of weird, especially with multiple namespaces
* Check return name exists
* Enum fields should be validated, need a defined way to access them 
* Function overloading should be allowed, need to change name based on the type, but unsure how to implement this without type checking, could defer, but then name resolution is messed up. Maybe a preprocessor check? 
* Add better capture ( lambda ) support, right now persistent scope is what I use, but doesn't capture everything, need to specify where it begins and ends. Whereas a capture will capture everything currently in scope. 
* Need to define how captures will work on embedded systems, could be difficult to store it on small RAM
* Think about the structs underlying representation, maybe I can reprsent it as an array of serparate types? But this wouldn't make sense for name resolution, so its a bit ambiguous. 

## Type Checker
* Design a formal type hierarchy 
* Design a formal narrowing and widening conventions 
* Function overloading ?
* Solidify union for TYPE, it gets a bit confusing
* Things like TYPE_CHAR and TYPE_INT get implicitly converted to U8 and I32, make sure to clarify this or add their specific types
* UNTYPED_INT is a weird way to handled rval types, causes implicit typing issues and type comparrison issues
* TYPE_NAME and TYPE_STRUCT are kind of redundant 
* Symbol needs forward declaration of TYPE\*, maybe there is a better way to handle this?
* For nested expressions, TypeCheck is called once per node, leading to rechecked nodes, maybe have a cached type in Symbol or ASTNode ???
* Environments don't exist yet, need a better system than PersistentScope, and also need to handle multiple namespaces
* Environments should store a few things: 
    - All variables within the current scope 
    _ Along with this their memory ownership 
    - Along with this their varialbe lifetimes
* Table driven type checking needs to include EVERY single operator / lval operation
* ImplicitCasting rules need to be more robust
* Should check array initalizing, based on size of array and type of array
* Should check compound literals based on type of struct / enum and what members it has
* Return type should check type of current function ( can't just recursively pass down TYPE* of function since that would mean writing new TypeCheck functions for every type )

## Desugarizer
* SHOULD BE PLACED BEFORE TYPE CHECKER, after name res, could reorder type checking, also simplifies type checking  
* Prefix vs postfix aren't handled, ++x works, but x++ should return the old value, and have another expr do x = x + 1; afterwards
* Fragile backwards child recursion due to destructive AST transform in this stage, find a way to either make in non destructive or fix the backwards recursion ( DON'T MUTATE , COPY INSTEAD)
* Hard coded child indices, especially in FOR_STMT


## Dependency Analyzer 
* Look for circular dependencies
* Identify 

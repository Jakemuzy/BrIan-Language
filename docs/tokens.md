| Token | Enum | Purpose | 
|---|---|---|
| if | IF | If Keyword |
| elif | ELIF | Else If Keyword |
| else | ELSE | Else Keyword |
| do | DO | Do Keyword |
| while | WHILE | While Keyword |
| for | FOR | For Keyword |
| switch | SWITCH | Switch Keyword |
| case | CASE | Case Keyword |
| default | DEFAULT | Default Keyword |
| break | BREAK | Break Keyword |
| continue | CONTINUE | Continue Keyword |
| return | RETURN | Return Keyword |
|---|---|---|
| lock | LOCK | Lock Keyword |
| critical | CRITICAL | Critical Keyword | 
| spawn | SPAWN | Spawn Keyword |
| await | AWAIT | Await Keyword |
|---|---|---|
| sizeof | SIZEOF | Size Of Type Keyword | 
|---|---|---|
| char | CHAR | Char Type |
| bool | BOOL | Bool Type |
| int | INT | Int Type |
| float | FLOAT | Float Type |
| double | DOUBLE | Double Type |
| long | LONG | Long Type |
| i8 | I8 | Signed 8 Byte Type |
| i16 | I16 | Signed 16 Byte Type |
| i32 | I32 | Signed 32 Byte Type |
| i64 | I64 | Signed 64 Byte Type |
| u8 | U8 | Unsigned 8 Byte Type |
| u16 | U16 | Unsigned 16 Byte Type |
| u32 | U32 | Unsigned 32 Byte Type |
| u64 | U64 | Unsigned 64 Byte Type |
| mat | MAT | Matrix Type |
| vec | VEC | Vector Type |
| chan | CHANNEL | Channel Type |
| mutex | MUTEX | Mutex Type |
| semaphore | SEMAPHORE | Semaphore Type |
| task | TASK | Task Type |
| void | VOID | Void Type |
| string | STRING | String Type |
|---|---|---|
| as | AS | Cast Type Keyword |
| let | LET | Let Type Keyword |
| gen | GEN | Generic Type Keyword |
|---|---|---|
| enum | ENUM | Enum Type Keyword |
| struct | STRUCT | Struct Type Keyword |
| typedef | TYPEDEF | Typdef Type Keyword |
| fn | FUNCTION | Function Type Keyword |
| interface | INTERFACE | Struct Interface Keyword |
| operator | OPERATOR | Operator Overload Keyword |
| lambda | LAMBDA | Anonymous Function Keyword |
|---|---|---|
| true | TRUE | True Predefined Var |
| false | FALSE | False Predefined Var |
| null | NILL | Null Predefined Var |
|---|---|---|
| const | CONST | Const Type Qualifier |
| static | STATIC | Static Type Qualifier |
| volatile | VOLATILE | Volatile Type Qualifier |
| inline | INLINE | Inline Type Qualifier |
| atomic | ATOMIC | Atommic Type Qualifier |
| extern | EXTERN | Extern Linkage Specifier |
|---|---|---|
| = | EQ | Equal Operator |
| += | PEQ | Plus Equal Operator |
| -= | SEQ | Subtract Equal Operator |
| *= | MEQ | Multiply Equal Operator |
| /= | DEQ | Divide Equal Operator |
| %= | MODEQ | Modulus Equal Operator |
| &= | ANDEQ | And Equal Operator |
| |= | OREQ | Or Equal Operator |
| &&= | ANDLEQ | Logical And Operator |
| ||= | ORLEQ | Logical Or Equal Operator |
| ~= | NEGEQ | Negate Equal Operator |
| ^= | XOREQ | Xor Equal Operator |
| >>= | RIGHTEQ | Right Shift Equal Operator |
| <<= | LEFTEQ | Left Shift Equal Operator |
| ++ | INC | Increment Operator |
| -- | DEC | Decrement Operator |
|---|---|---|
| + | PLUS | Plus Operator, Positive Prefix |
| - | MINUS | Minus Operator, Negative Prefix |
| / | DIV | Divide Operator |
| * | MULT | Multiply Operator |
| ** | POW | Power Operator |
| % | MOD | Modulus Operator |
|---|---|---|
| == | EQQ | Equals Comparison Operator |
| != | NEQQ | Not Equals Comparison Operator |
| >= | GEQQ | Greater Than Or Eqauls Comparison Operator |
| <= | LEQQ | Less Than Or Equals Comparison Operator |
| ! | NOT | Not Comparison Operator |
| && | ANDL | And Logical Operator |
| || | ORL | Or Logical Operator |
| > | GREAT | Greater Than Logical Operator |
| < | LESS | Less Than Logical Operator |
|---|---|---|
| ~ | NEG | Negate Bitwise Operator |
| ^ | XOR | Xor Bitwise Operator |
| | | OR | Or Bitwise Operator |
| & | AND | And Bitwise Operator |
| << | LSHIFT | Left Shift Bitwise Operator |
| >> | RSHIFT | Right Shift Bitwise Operator |
|---|---|---|
| @ | DOTPROD | Vector Dot Product Operator |
|---|---|---|
| ? | QUESTION | Ternary Operator |
| : | COLON | Ternary Operator |
|---|---|---|
| ->? | SREF | Safe Reference Access Operator | 
| .? | SMEM | Safe Member Access Operator |
| -> | REF | Reference Access Operator | 
| . | MEM | Member Access Operator |
|---|---|---|
| <- | SEND | Message Send/Receive Operator |
|---|---|---|
| ( | LPAREN | Left Parenthesis |
| ) | RPAREN | Right Parenthesis |
| [ | LBRACK | Left Bracket | 
| ] | RBRACK | Right Bracket |
| { | LBRACE | Left Brace |
| } | RBRACE | Right Brace |
|---|---|---|
| ; | SEMI | Statement End |
| , | COMMA | Statement Split |
| # | HASH | Preprocessor Directive |
| /* { . } */ | COMMENT | Comment |
| // { . } \n | COMMENT | Comment |
|---|---|---|
| import | - | Import Directive Keyword |
| endif | - | Endif Directive Keyword |
| pragma | - | Pragma Directive Keyword |
| error | - | ERROR Directive Keyword |
|---|---|---|
| \n | NEWLINE | New Line Whitespace |
| \t | TAB | Tab Whitespace |
|---|---|---|
| { a-Z | 0-9 | _ } | IDENT | Identifier |
| { 0-9 } | INTEGRAL | Integral Number | 
| { 0-9 } [ '.' { 0-9 } ] [ 'e' [ '+' | '-' ] { 0 - 9 } ] | REAL | Real Number |
| \' . \' | CLITERAL | Character Literal |
| \" { . } \" | SLITERAL | String Literal |
|---|---|---|
| EOF | END | End Of File Indicator |
| . | ERR | Error Indicator | 





## Tokenizer 

### DFAs

The tokenizer is an interesting phase. The naive approach to a tokenizer would be to use functions to simulate a deterministic finite automate that fits our tokens.
This approach works great for most cases, however, for operators it suffers a lot leading to .tedious and hard to maintain state machines.
This is where table based DFA's shine: operator resolution is simple character sequence matching, meaning our tables are quite small and benefit from caching. 
If we were to implement table based DFAs for every token, the size of the table would explode exponentially, causing adverse performance.
This is precisely why BrIan takes a hybrid approach: using table based resolution for operators which are small and predictable, and function based for everything else.

### Buffering

Just like the DFAs buffering is not as simple as it might originally seem. 
A simple character buffer can be thought of as a single character slot of memory that stores a character. If we acccidentally consume too many characters in search of a token, we simply put the character back into the buffer.
This might seem fine at first glance, however, once you realize each character read is a system call. Even if just for this reason, having a larger buffer could provide beneficial, leading to less system calls. 
This isn't the end of the story though; if instead of one buffer, we have two buffers, we can trivially solve the problem of tokens being split between buffers. 
Upon the sentinel which is placed at the end of the buffer (typically EOF or \0) is found the next buffer is loaded.
This system proves to be perfect until the token becomes longer than the buffer; hence we restrict identifiers length.

### Impact 

real    0m0.015s
user    0m0.004s
sys     0m0.011s

real    0m0.122s
user    0m0.015s
sys     0m0.064s

The improvements singlehandedly droped the time to tokenize every token in the gramm from a tenth of a second to a hundredth of a second. A 10x improvement as well as more cache efficient and in my opion expandable and maintanable. 

## Parser 

The three main benefits over the previous parser system are pratt parsing, utilizing FIRST and FOLLOW sets more efficiently and using an arena allocator to reduce the overhead of memory allocations.
Pratt parsing simplifies the operator resolution a lot. Instead of deep stack calls due to recursive descent, pratt parsing handles precedence far more elegantly.

Starting with the most simple of fixes; in V1.0 when parsing the file BrIan often relegated checking FIRST sets to the child function. In practice this works, and is actually quite a clean looking design. 
V2.0, however, aims for higher performance, maintainabilty and scalability, for this reason I've decided to sacrifice a bit of aesthetics and transition this check in the parent function.
By moving the FIRST set check to the parent function, we can avoid deeply nested recursion that bloats the call stack before a first set is discovered. 
Although a small change, it should have significant impact on deeply recursive areas of the code such as expressions.

For BrIan, operator parsing is divided into three categories for easier use during future phases: assignment, unary, and binary operators.
As for arena allocation; it allows AST nodes to be allocated in builk, and having uniform lifetimes, allowing for easier cleanup during error resolation or recovery as well.
A little note on the arena allocator, it allows for better locality among datatypes during BrIans compilation such as the ASTNodes and the temporary scopes used during name resolution. Although allowing for better locality, less frequent memory allocations and easier memory freeing, it does have a small caveat. 
The main drawback with the arena allocator in our scenario is that we don't know how much space to allocate at runtime, which can lead to a lot of allocated memory that we don't end up using. For this reason BrIan enacts a couple of different methods.
The first method being; BrIan estimates how much memory will be used during each step based on the file size (this could potentially be explictly specified via a compiler flag as well)
The second method BrIan enacts is to use a linked list of arena's if the first one fills up. Although this is a viable solution, we again don't know how much space to allocate, and even worse, if paired with our previous estimate, we could grossly misestimate the amount of space we need to allocate for the second arena. For this reason it is extremely important to specify the growth pattern of the next arena (ie double space, half, etc)

The way the compiler interacts with the Parser is also of note. CompilationState* passes down key information to every phase in order to allow for proper error propagation and compiler flag information

## Name Resolution

## Type Checking 

The main decision I had to make was whether or not to allow implicit casting for safety. I decided to go with a hybrid approach: whereas C may warn you if you're cast narrows, BrIan makes this an outright error. If one desires to cast down they must use the 'as' keyword following the variable they wish to cast. This not only allows for casting to be LL(1), but it also makes casting inheriently deterministic. Although we can't get away from potential issues with narrowing conventions, we can mitigate the damage ahead of time by making the user aware.

# ----------

## Compilation 

Brian handles all its compilation needs in the CompileBrian function. This function takes the command line arguments of the brian compiler and executes different code paths depending on what was provided. The following flags are provided for your conveinence.

NOTE: Debug flags require you to pass -DDEBUG to the cli

| Flag | Purpose | Category | Additional |
|---|---|---|---|
| tok | Executes BrIan up until tokenization | Debug Info |-|
| parse | Executes BrIan up until parsing | Debug Info |-|
| pre | Executes BrIan up until preprocessing | Debug Info |-|
| name | Executes BrIan up until name resolution | Debug Info |-|
| type | Executes BrIan up until type checking | Debug Info |-|
| ir | Executes BrIan up until LLVM IR assembly | Debug Info |-|
| assemble | Executes BrIan up until target assembly is output |-|
|---|---|---|---|
| target | Specifies target machine | LLVM Flag | ='target_goes_here' | 
| target | Specifies target machine | LLVM Flag | ='target_goes_here' | 
|---|---|---|---|
| arena | Specifies total memory to allocate to each arena | Memory Flag | ='size_goes_here' |

## Error Handling

Handles errors, error messages, what should happen upon errors, etc. Calls a label in Compiler.c to cleanup if flags allow it, in order to cleanup program
before failure.

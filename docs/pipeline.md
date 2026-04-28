
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


        3,459,057      task-clock                       #    0.574 CPUs utilized             
                1      context-switches                 #  289.096 /sec                      
                0      cpu-migrations                   #    0.000 /sec                      
            65      page-faults                      #   18.791 K/sec                     
    <not counted>      cpu_atom/instructions/                                                  (0.00%)
        2,103,469      cpu_core/instructions/           #    1.25  insn per cycle            
    <not counted>      cpu_atom/cycles/                                                        (0.00%)
        1,678,730      cpu_core/cycles/                 #    0.485 GHz                       
    <not counted>      cpu_atom/branches/                                                      (0.00%)
        393,891      cpu_core/branches/               #  113.872 M/sec                     
    <not counted>      cpu_atom/branch-misses/                                                 (0.00%)
            8,870      cpu_core/branch-misses/          #    2.25% of all branches           
    30.1 %  tma_backend_bound      
                                                #      9.5 %  tma_bad_speculation    
                                                #     36.6 %  tma_frontend_bound     
                                                #     23.8 %  tma_retiring           

    0.006030643 seconds time elapsed

    0.001813000 seconds user
    0.004534000 seconds sys


The improvements singlehandedly droped the time to tokenize every token. A 6x improvement as well as more cache efficient and in my opion expandable and maintanable. 


    19,668,459      task-clock                       #    0.546 CPUs utilized             
                6      context-switches                 #  305.057 /sec                      
                3      cpu-migrations                   #  152.528 /sec                      
            977      page-faults                      #   49.673 K/sec                     
        5,616,880      cpu_atom/instructions/           #    0.79  insn per cycle              (9.94%)
    14,935,737      cpu_core/instructions/           #    1.55  insn per cycle              (84.98%)
        7,154,828      cpu_atom/cycles/                 #    0.364 GHz                         (9.86%)
        9,628,633      cpu_core/cycles/                 #    0.490 GHz                         (84.98%)
        1,022,737      cpu_atom/branches/               #   51.999 M/sec                       (13.44%)
        2,816,323      cpu_core/branches/               #  143.190 M/sec                       (84.98%)
        47,792      cpu_atom/branch-misses/          #    4.67% of all branches             (15.02%)
        41,281      cpu_core/branch-misses/          #    1.47% of all branches             (84.98%)
#     28.6 %  tma_backend_bound      
                                                #     10.7 %  tma_bad_speculation    
                                                #     32.4 %  tma_frontend_bound     
                                                #     28.2 %  tma_retiring             (84.98%)
#     16.4 %  tma_bad_speculation    
                                                #     20.6 %  tma_retiring             (15.02%)
#     20.5 %  tma_backend_bound      
                                                #     42.5 %  tma_frontend_bound       (15.02%)

    0.036047625 seconds time elapsed

    0.007774000 seconds user
    0.017492000 seconds sys



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

----- Technical Aspects -----
An interesting observation one might make is that nested channel types are kind of ambiguous; chan<chan<TYPE>>. '>>' Is tehnically a right shift operation, so when parsing, right shift will appear instead of two disctinct '>'. It is for this reason we have a SetEdgeCaseFlag() function that forces the tokenizer to split '>>' into two dinstinct operators. My language handles this ambiguity in a kind of weird way, but since I disallow nested generics this is the only case of the ambiguity in the entire language.

Another thing to note: since I decided to go with a predictive parser architecture, the parent function typically predicts which child function it would go to. This means that the first token of the child output is immediately consumed in most cases. I tried to lean as heavy as I could into the predictive parsing, meaning double checking of node types is rare, at the expensive of a little bit of readability. As my goal for v2.0 of the compiler is performance, I believe this is a worthy sacrafice.

### Impact

    18,577,755      task-clock                       #    0.803 CPUs utilized             
                0      context-switches                 #    0.000 /sec                      
                0      cpu-migrations                   #    0.000 /sec                      
            1,007      page-faults                      #   54.205 K/sec                     
    <not counted>      cpu_atom/instructions/                                                  (0.00%)
    15,349,930      cpu_core/instructions/           #    1.66  insn per cycle            
    <not counted>      cpu_atom/cycles/                                                        (0.00%)
        9,237,402      cpu_core/cycles/                 #    0.497 GHz                       
    <not counted>      cpu_atom/branches/                                                      (0.00%)
        2,919,993      cpu_core/branches/               #  157.177 M/sec                     
    <not counted>      cpu_atom/branch-misses/                                                 (0.00%)
        39,562      cpu_core/branch-misses/          #    1.35% of all branches           
    25.5 %  tma_backend_bound      
                                                #     10.4 %  tma_bad_speculation    
                                                #     34.4 %  tma_frontend_bound     
                                                #     29.8 %  tma_retiring           

    0.023129638 seconds time elapsed

    0.007703000 seconds user
    0.015406000 seconds sys


A minor improvement in parsing time, but this is at a minor scale, small programs. Additionally v2.0 has a much expanded grammar, in very operator heavy programs v2.0 will likely shine due to pratt parsing. Additionally the main improvements for the parser are actually memory usage and stack depth rather than execution speed.

Performance counter stats for './build/compiler ./tests/parser/Main.b -parse':

        6,182,023      task-clock                       #    0.682 CPUs utilized             
                1      context-switches                 #  161.759 /sec                      
                1      cpu-migrations                   #  161.759 /sec                      
            75      page-faults                      #   12.132 K/sec                     
    <not counted>      cpu_atom/instructions/                                                  (0.00%)
        4,069,015      cpu_core/instructions/           #    1.34  insn per cycle            
    <not counted>      cpu_atom/cycles/                                                        (0.00%)
        3,028,327      cpu_core/cycles/                 #    0.490 GHz                       
    <not counted>      cpu_atom/branches/                                                      (0.00%)
        816,604      cpu_core/branches/               #  132.093 M/sec                     
    <not counted>      cpu_atom/branch-misses/                                                 (0.00%)
        15,111      cpu_core/branch-misses/          #    1.85% of all branches           
    23.7 %  tma_backend_bound      
                                                #     10.5 %  tma_bad_speculation    
                                                #     37.4 %  tma_frontend_bound     
                                                #     28.5 %  tma_retiring           

    0.009066598 seconds time elapsed

## Name Resolution

Originally v1.0 of BrIan updated the enviornment in a hybrid imperative/functional style: that being a destructive update where we store a call stack, yet also have a linked list of symbol tables. Not very efficient at all and let to a lot of complications in later passes. Version 2 aims to keep this more in line with a imperative pass, where $\sigma$<sub>1</sub> is kept in pristine condition prior to entering a scope. A new scope is simply a new symbol table, storing a pointer to the previous one.

I chose not to use a persistent red black tree data structure for symbol resolution as this effictively baloons the complexity of the functional pass. This is exactly why I chose the imperative pass as well. Although providing O(logn) instead of O(n) that imperative offers: imperative is much easier to implement, and symbol resolution is often never the bottleneck in performance.

Additionally, each symbol is again allocated inside of an arena, similar to our parser and tokenizer. This comes with a caveat however: remember the issue we had where whenever we wanted to realloc, we would ditch the old memory? To avoid this the symbol table cannot be a dynamic size. Dynamic sized symbol tables are extremely useful to have, however. This means that we must malloc the buckets manually. 

Another thing to note: namespaces. Each time a scope is entered, a new environment is created for each namespace. This intuitively makes sense: a new namespace should have completely new bindings, yet still be able to access previous information. This, however, can exponentially increase the amount of unused symbols being initalized. Think of an example where we enter a scope, yet only the variable namespace is used. This means we initalized an entire type namespace for no reason. We can solve this by binding scopes as we please, however, for BrIan in particular, only 2 namespaces are used at the moment: type and name. Additionally when declaring a new variable we would have to check if the namespace even exists first, adding another operationg to symbol table additon. This is something to consider if BrIan ever decides to move towards user defined namespaces, though, as dynamic namespace bindings are required anyways in that case.

A couple more considerations: because symbol tables are a linked list based on scope, as soon as we exit a scope, this symbol informatoin is discarded. For this reason we store a Symbol* inside of each ASTNode* as well for further traversal. 

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

## Testing 

Testing is completely done via golden tests. I implemented a complete golden tester pipeline from scratch. All tests aer located in ./tests/ under their respective phase of the pipeline. For example if you wanted to unit test all of the parser files to ensure your code didn't break anything you would reference ./tests/parser/ and if you wanted to reference a specific file or directory you would do ./tests/parser/functions/xxx/xxx.b. That being said there are a couple of flags you can pass directly to the golden tester. 


| Flag | Purpose |
|---|---|---|---|
| --regenerate | Regenerates Golden Files |
| --file | Specifies Directory or File to Test | 
| OTHER | Any Other Flag is Passed Directly to the Compiler |

As the table shows, any additional flags passed to the tester will be forwarded to the compiler. So if I wanted to test the entire parser I would do something like this. ./bin/tester --file ./tests/parser/ -parse
Where the -parse flag is the flag for the compiler to execute only up to the parsing stage of the pipeline. This allows each for different testing on dedicated phase tests.
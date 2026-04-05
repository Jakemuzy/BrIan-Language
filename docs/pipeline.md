
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

# ----------

## Compilation 

Brian handles all its compilation needs in the CompileBrian function. This function takes the command line arguments of the brian compiler and executes different code paths depending on what was provided. The following flags are provided for your conveinence.

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

## Error Handling

Handles errors, error messages, what should happen upon errors, etc. Calls a label in Compiler.c to cleanup if flags allow it, in order to cleanup program
before failure.

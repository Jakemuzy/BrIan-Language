
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
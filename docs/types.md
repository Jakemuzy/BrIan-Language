# Types  

$\Gamma \vdash \texttt{u8} : \texttt{Type}$  
$\Gamma \vdash \texttt{u16} : \texttt{Type}$  
$\Gamma \vdash \texttt{u32} : \texttt{Type}$  
$\Gamma \vdash \texttt{u64} : \texttt{Type}$  
$\Gamma \vdash \texttt{i8} : \texttt{Type}$   
$\Gamma \vdash \texttt{i16} : \texttt{Type}$  
$\Gamma \vdash \texttt{i32} : \texttt{Type}$  
$\Gamma \vdash \texttt{i64} : \texttt{Type}$  

$\Gamma \vdash \texttt{char} : \texttt{Type}$   
$\Gamma \vdash \texttt{bool} : \texttt{Type}$   
$\Gamma \vdash \texttt{float} : \texttt{Type}$  
$\Gamma \vdash \texttt{int} : \texttt{Type}$   
$\Gamma \vdash \texttt{double} : \texttt{Type}$  
$\Gamma \vdash \texttt{long} : \texttt{Type}$   
$\Gamma \vdash \texttt{string} : \texttt{Type}$    
$\Gamma \vdash \texttt{void} : \texttt{Type}$   

$\Gamma \vdash \texttt{mutex} : \texttt{Type}$ 
$\Gamma \vdash \texttt{semaphore} : \texttt{Type}$ 
$\Gamma \vdash \texttt{type} : \texttt{Type}$ 

$$ 
\frac
{\Gamma \vdash \text{T} : \texttt{Type}}
{\Gamma \vdash \texttt{chan<} T \texttt{>}}
$$

$$
\frac
{\Gamma \vdash \text{T} : \texttt{Type} \qquad \Gamma \vdash \text{X} : \texttt{i64} \qquad \Gamma \vdash \text{Y}}
{\Gamma \vdash \texttt{mat<} T \textttt{,} X \texttt{,} Y \texttt{>} : \texttt{Type}}
$$  
  
$$
\frac
{\Gamma \vdash \text{T} : \texttt{Type} \qquad \Gamma \vdash \text{X} : \texttt{i64}}
{\Gamma \vdash \texttt{mat<} T \textttt{,} X \texttt{>} : \texttt{Type}}
$$  
  
$$
\frac
{\Gamma \vdash T_1 : \texttt{i32} \qquad ... \qquad \Gamma \vdash T_n : \texttt{i32}}
{\Gamma \vdash enum R(T_1,...T_n) : \texttt{Type} }
$$
  
$$
\frac
{\Gamma \vdash T : \texttt{Type} \qquad \Gamma \vdash T_1 : \texttt{Type} \qquad ... \qquad \Gamma \vdash T_n : \texttt{Type}}
{\Gamma \vdash fp T(T_1,...,T_n) : \texttt{Type} }
$$  

$$
\frac
{\Gamma \vdash T : \texttt{Type} \qquad \Gamma \vdash T_1 : \texttt{Type} \qquad ... \qquad \Gamma \vdash T_n : \texttt{Type}}
{\Gamma \vdash closure T(T_1,...,T_n) : \texttt{Type} }
$$  
  
$$\frac
{ \Gamma \vdash T : \texttt{Type} \qquad \text{Integral}(T) }
{ \Gamma \vdash \texttt{\%T} : \texttt{Type} }
$$  
  
# Classes  
$\text{Numeric}(T) \triangleq T \in \{\texttt{i8},\ \texttt{i16},\ \texttt{i32},\ \texttt{i64},\ \texttt{u8},\ \texttt{u16},\ \texttt{u32},\ \texttt{u64},\ \texttt{int},\ \texttt{float},\ \texttt{double},\ \texttt{long}, \texttt{char}\}$
  
$\text{Integral}(T) \triangleq T \in \{\texttt{i8},\texttt{i16}, \texttt{i32}, \texttt{i64}, \texttt{u8}, \texttt{u16}, \texttt{u32}, \texttt{u64}, \texttt{int}, \texttt{long}, \texttt{char}\}$  
  
$\text{Signed}(T) \triangleq T \in \{\texttt{i8},\texttt{i16}, \texttt{i32}, \texttt{i64}, \texttt{float}, \texttt{double}, \texttt{int}, \texttt{long}\}$  
  
$\text{Unsigned}(T) \triangleq T \in \{\texttt{u8}, \texttt{u16}, \texttt{u32}, \texttt{u64}, \texttt{char}\}$  
  
$\text{Decimal}(T) \triangleq T \in \{\texttt{float}, \texttt{double}\}$  
  
$\text{Boolean}(T) \triangleq T \in \{\texttt{bool}\}$  
  
$\text{Pointer}(T) \triangleq \exists U. (T = *U) \lor (T = fp R(T_1,...,T_n)) \lor (T = closure R(T_1,...T_n))$
  
$\text{Truthy}(T) \triangleq \text{Integral}(T) \lor \text{Boolean}(T) \lor \text{Pointer}(T) \lor (T = \texttt{string})}$  
    
# Relations  
$\Gamma \vdash \texttt{char} \equiv \texttt{i8}$  
$\Gamma \vdash \texttt{bool} \equiv \texttt{u8}$  
$\Gamma \vdash \texttt{int} \equiv \texttt{i32}$  
$\Gamma \vdash \texttt{long} \equiv \texttt{i64}$  
$\Gamma \vdash \texttt{string} \equiv \texttt{(pointer, i8)}$  
$$  
\frac
{\Gamma \vdash t : \texttt{string}}
{\Gamma \vdash t : pointer \times i64}
$$  
  
# Operations  
$\text{Operator}(ASGNOP) \triangleq ASGNOP \in \{\texttt{=}, \texttt{+=}, \texttt{-=}, \texttt{*=}, \texttt{/=}, \texttt{\%=}, \texttt{&=}, \texttt{|=}, \texttt{\&\&=}, \texttt{||=}, \texttt{\~=}, \texttt{^=}, \texttt{>>=}, \texttt{<<=}, \texttt{++}, \texttt{--}\}$  
$\text{Operator}(BINOP) \triangleq BINOP \in \{\texttt{+}, \texttt{-}, \texttt{*}, \texttt{/}, \texttt{\%}\}}$  
$\text{Operator}(LOGOP) \triangleq LOGOP \in \{\texttt{==}, \texttt{!=}, \texttt{>=}, \texttt{<=}, \texttt{!}, \texttt{\&\&}, \texttt{||}, \texttt{>}, \texttt{<}\}$  
$\text{Operator}(BITOP) \triangleq BITOP \in \{\texttt{\~}, \texttt{^}, \texttt{|}, \texttt{&}, \texttt{<<}, \texttt{>>}\}$  
$\text{Operator}(STRCTOP) \triangleq STRCTOP \in \{\texttt{.}, \texttt{.?}, \texttt{->}, \texttt{->?}\}$  
$\text{Operator}(VECOP) \triangleq VECOP \in \{\texttt{+}, \texttt{-}, \texttt{*}, \texttt{/}, \texttt{@}\}$  
$\Gamma \vdash \text{Operator}(PTROP) \equiv \texttt{[]}$  

$$
\frac
{\Gamma \vdash \text{T} : Type} 
{\Gamma \vdash \text{Operator}(SIZEOP) \equiv \texttt{sizeof(} T \texttt{)}}
$$

### Don't forget about ternary and send / receive

$$
\frac
{\Gamma \vdash a : \text{Integral} \qquad \Gamma \vdash b : \text{Integral}}
{\Gamma \vdash a + b : \text{Integral}}
$$

### Widening
### Using categories for now, but may have to specify which size decima
$$
\frac 
{\Gamma \vdash a : \text{Integral} \qquad \Gamma \vdash b : \text{Decimal}}
{\Gamma \vdash \exists OP \in {a + b : \text{Decimal}}
$$
  
### Notes  

* Strings are truthy since they are true if non empty and false if empty  
* Matrix and vector operations get their own semantics, but not category  
* Pointer arithmetic WILL be allowed since it is necessary, though it is the source of safety concerns  
* Truthy is also a common source of bugs, but I believe removing these convenient statements removes power from the user. At the end of the day you can do it either way and BrIan will not force a particular coding convention upon its users. BrIan prevents obvious footguns and traps, however, it will not restrict its users freedom. That being said, BrIan IS a multi-paradigm langauge so it needs to balance its features with safety. Despite this, BrIan also primarily targets embedded systems, so the user should discern the best cases to use particular conventions.  
* Widening IS implicitly allowed (warned), however, narrowing is explicity forbidden unless a prior cast has been made.

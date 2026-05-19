# Types  

$$\Gamma \vdash \texttt{u8} : \texttt{Type}$$ u8 : Type  
$$\Gamma \vdash \texttt{u16} : \texttt{Type}$$ u16 : Type  
$$\Gamma \vdash \texttt{u32} : \texttt{Type}$$ u32 : Type  
$$\Gamma \vdash \texttt{u64} : \texttt{Type}$$ u64 : Type  
$$\Gamma \vdash \texttt{i8} : \texttt{Type}$$ i8 : Type  
$$\Gamma \vdash \texttt{i16} : \texttt{Type}$$ i16 : Type  
$$\Gamma \vdash \texttt{i32} : \texttt{Type}$$ i32 : Type  
$$\Gamma \vdash \texttt{i64} : \texttt{Type}$$ i64 : Type  

$$\Gamma \vdash \texttt{char} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{bool} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{float} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{int} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{double} : \texttt{Type}$$  
$$\Gamma \vdash \texttt{long} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{string} : \texttt{Type}$$  
$$\Gamma \vdash \texttt{void} : \texttt{Type}$$ 

$$\Gamma \vdash \texttt{mutex} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{semaphore} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{type} : \texttt{Type}$$ 

$$\Gamma \vdash \texttt{chan<Type>} : \texttt{Type}$$  
$$\Gamma \vdash \texttt{mat<Type, i64, i64>} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{vec<Type, i64>} : \texttt{Type}$$  

$$\Gamma \vdash \texttt{fp} : \texttt{Type}$$ 
$$\Gamma \vdash \texttt{closure} : \texttt{Type}$$ 

# Classes  

$$\text{Numeric}(T) \triangleq T \in \{\texttt{i8},\ \texttt{i16},\ \texttt{i32},\ \texttt{i64},\ \texttt{u8},\ \texttt{u16},\ \texttt{u32},\ \texttt{u64},\ \texttt{int},\ \texttt{float},\ \texttt{double},\ \texttt{long}\}$$

# Relations  
$$\Gamma \vdash \texttt{char === i8}$$
$$\Gamma \vdash \texttt{bool === i8}$$
$$\Gamma \vdash \texttt{int === i32}$$

# Operations  
$$
\frac
{
$\Gamma$, x : i64 $$\vdash$$ b : i64 $$\vdash$$ a : i64
}
{
$\Gamma$ $$\vdash ($\lambda$x.a$\lambda$x.b) : i64
}
$$

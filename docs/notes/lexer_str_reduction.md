

the lexer should be able to reduce duplicate strings in memory.
this could be done efficiently with SIMD comparisons and a static buffer?

is it worth a potential slowdown in the lexer?

what is the memory overhead of using duplicate strings?
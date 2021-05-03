# Optimization Techniques



## tail calls

with tail call optimization we can replace a core loop with "recursion"

basically this replaces function calls with a jump, and reuses the stack
this makes recursion very similar to a loop on the intruction level

is this hard to get right? probably.
it it very cool? yes


going to try to change the lexetizer to follow this,
start: lexetizer takes ~0.000045 and uses ~2421 instructions
end: lexer takes ~11ms
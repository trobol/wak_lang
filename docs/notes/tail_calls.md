# tail calls

with tail call optimization we can replace a core loop with "recursion"

basically this replaces function calls with a jump, and reuses the stack
this makes recursion very similar to a loop on the intruction level


the lexer (after tail call) runs at about ~60MB per second. 
based on some informal tests, that is anywhere from 10 - 1000x.
some tests were hard to perform because older versions could not handle larger files.




### tips and things to watch out for:

the amount of work done by each loop should be minimized, this keeps the important values in register

if there is a function that does not tail call, have the callee branch at the end of its function. this prevents the callee from pushing the registers. e.g.

instead of:
```
int func() {
	if (something) {
		non_tail_call();
	}

	return tail_call();
}

```

```
int do_tail() {
	return tail_call();
}

int do_non_tail() {
	non_tail_call();

	return do_tail();
}

int func() {
	if (something) {
		return do_non_tail();
	} else {
		return do_tail();
	}
}
```


## references:
https://blog.reverberate.org/2021/04/21/musttail-efficient-interpreters.html
https://github.com/protocolbuffers/upb/pull/310
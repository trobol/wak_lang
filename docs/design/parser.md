# Parser Design


A program is made up of blocks.


A statement can be:
- return (`return 1;`)
- assignment (`a = 1;`)
- declaration (`a := 1;`)
- void function call (`something();`)
- block

## vocabulary

variable:
	a space of memory that has a type and holds a value
value:
	expression, literal or variable
expression:
	one or more values and an operation



## block
A block can be:
- function (`void something() {}`)
- if/else (`if (a) {}`)
- switch

- A block has a new variable context.
- ?All blocks start with `{` and end with `}`?


## output format

could the format have "implicit value selection"?

### implicit value selection:
when a value (expression, variable or literal) is needed for an instruction (e.g. `return (a + b - 1);`) can the statement that the instruction refers to be inferred?
if that could work it would allow faster parsing of any instruction that requires a value
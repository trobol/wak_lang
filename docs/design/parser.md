# Parser Design


A program is made up of blocks.


A statement can be:
- return (`return 1;`)
- assignment (`a = 1;`)
- declaration (`a := 1;`)
- void function call (`something();`)
- block



expressions:


## block
A block can be:
- function (`void something() {}`)
- if/else (`if (a) {}`)
- switch

- A block has a new variable context.
- ?All blocks start with `{` and end with `}`?

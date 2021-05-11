# lexer design


## high level

the lexer takes in the source code as text (a string buffer), and outputs a list of tokens.

a token can be one of three things

- value [ see below ]
- identifier [ a name like `foo`]
- literal [ `"hello"` or `0x14`]


### value token
a value token stores an enum value.

enum values 0-255 are reserved for ascii character codes
/ most characters are not valid value tokens, so should they be part of the enum? /

the others are for reserved keywords and combined operators ( like `+=` or `&&`)



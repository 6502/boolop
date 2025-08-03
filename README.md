# boolop
Pretty-print a truth table for a boolean expression of n inputs

This is a small exercise in that I found proposed by Dmitrii Kovanikov (@ChShersh) on X;
found it a nice idea so I implemented it in C.

Given a boolean expression of n variables as input, produces the truth table.
Variables are uppercase letters, `t` and `f` are constants and here are the operators
listed in order of precedence; binary ones are all left-associative.

!   unary negation
&   logical and
|   logical or
=>  implication
<=> equivalence

Grouping using `(` and `)` is possible.

Example run:
```
agriffini@hparchy:~/x$ ./boolop '(A & (A => B)) => B'
+---+---+---------------------+
| A | B | (A & (A => B)) => B |
+---+---+---------------------+
| f | f |          t          |
| f | t |          t          |
| t | f |          t          |
| t | t |          t          |
+---+---+---------------------+
```

# dis

This package provides a function that is able to peek into the bytecode of a
Lily function. This can be installed using Lily's `garden` via:

`garden install github FascinatedBox/dis`

## toplevel

### define dis`(f: Function(1)): String`

This receives a function that takes any number of input arguments, and returns a
`String` containing the disassembly of that function. If 'f' is not a native
function, then `"<foreign function>"` is returned instead.

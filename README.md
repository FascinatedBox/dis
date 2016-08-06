# dis

### Version: 0.1

This package provides functions for disassembling a native Lily function.

You can install this package via

`garden install github FascinatedBox/dis`

and then use it like so

`use dis`

This provides the following two functions:

`dis.dis( Function(1) ): String`

This function dumps any native function, regardless of how many arguments it
takes. However, the function is required to not return a value. The result of
this function is a `String` containing the disassembly. If the function provided
is a foreign function, then `"<foreign function>"` is returned.

`dis.dis_rt[A]( Function(1):A ):String`

This is a placeholder function, since Lily doesn't have a `Unit` type yet. It
handles disassembly for any function that returns a value.
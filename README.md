# dis

### Version: 0.1

This package provides functions for disassembling a native Lily function.

You can install this package via

`garden install github FascinatedBox/dis`

and then use it like so

`import dis`

This provides a single function:

`dis.dis( Function(1) ): String`

This function dumps any native function, regardless of how many arguments it
takes, or what the resulting type is. The result of this function is a `String`
containing the disassembly. If the function provided is a foreign function, then
`"<foreign function>"` is returned.

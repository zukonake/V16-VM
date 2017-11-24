# V16-ASM v1.1.0 by zukonake

See the general specifications for instructions list.
## Syntax
  * `ZZZ $A:XXXX $B:YYYY` where
    * `ZZZ` - base instruction (for example `ADD`)
    * `$` - if placed, means the mode is indirect
    * `AB` - modes (for example `R`)
    * `XY` - addresses (for example `0FA3`)
  * examples
    * `CPY  MFFFF $M0000` - copy the value `FFFF` to the memory at address `0000`
    * `JMP  R0002` - jump to the address stored in `R[2]`
    * `JMP $R0002` - jump to the address stored in memory at address stored in `R[2]`
  * `#` - comment lines
  * constants
    * `NAME:VALUE`
    * refering to constants
      * `!NAME`
    * example
    * names should be up to 5 letters of length (so that they don't break the indent)
      * although this is purely subjective as the indent is not enforced in any way
    * constants are compile-time abstraction only

```
CONST:0x0F0F
CPY  M:!CONST $M:0x0000
CPY $M:0x0000 $M:0x0000 #for indent comparison
```

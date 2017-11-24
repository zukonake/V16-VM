# V16-ASM v1.0.0 by zukonake

See the general specifications for instructions list.
# Syntax
	* `ZZZ $A:XXXX $B:YYYY` where
		* ZZZ - base instruction (for example ADD)
		* $ - if placed, means the mode is indirect
		* AB - modes (for example R)
		* XY - addresses (for example 0FA3)
	* examples
		* `CPY  MFFFF $M0000` - copy the value `FFFF` to the memory at address `0000`
		* `JMP  R0002` - jump to the address stored in R[2]
		* `JMP $R0002` - jump to the address stored in memory at address stored in R[2]

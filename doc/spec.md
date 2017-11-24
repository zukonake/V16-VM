# V16 v1.1.0 CPU by zukonake

## General
  * 16 bit word - W
  * 1W memory addresses (128KiB max)
  * 16x 1W registers (`R[0]` .. `R[F]`)
  * 1W program counter (`PC`)
  * 8 bit stack pointer (`SP`)
  * 4 bit flag (`F`) where
    * `OP--` where
      * `O` - overflow flag
      * `P` - panic flag
    * `F[O]` refers to overflow flag etc.
  * 256x 1W stack (`S[0]` .. `S[FF]`)
  * hardware modules
  * big endian

## Instructions
  * instructions of 1W - 3W
  * instruction is composed of (nibbles)
    * `IIABXXXXYYYY` where
      * `I` - instruction code
      * `A` - X mode
      * `B` - Y mode
      * `X` - X address
      * `Y` - Y address
    * if the A mode does not require an adress, Y address gets shifted to the X word

## A/B modes
  * `C` - cycles to fetch
  * `V` - value
  * `N` - nibbles needed
  * `I` - indirect? takes 2 more cycles
```
+---+----+---+------+-------------+
| C | V  | N | NAME | DESCRIPTION |
+---+----+---+------+-------------+
| 2 | I0 | 4 | M    | memory
+---+----+---+------+--------------
| 0 | I1 | 2 | R    | registry
+---+----+---+------+--------------
| 4 | I2 | 4 | H    | hardware
+---+----+---+------+--------------
| 1 | I3 | 0 | P    | program counter
+---+----+---+------+--------------
| 1 | I4 | 0 | T    | stack top
+---+----+---+------+--------------
| 1 | I5 | 0 | S    | stack pointer
+---+----+---+------+--------------
| 0 | I6 | 0 | F    | flags
+---+----+---+------+--------------
|   | .. |   |      |
+---+----+---+------+-------------+
```

## Instructions
```
+---+------+----+----------+----------+
| C | V    | AB?| NAME     | BEHAVIOR |
+---+------+----+----------+----------+
| SIMPLE                              |
+---+------+----+----------+----------+
| 1 | 0x00 | 00 | NOP      | //nothing
+---+------+----+----------+----------+
| 1 | 0x01 | 10 | JMP X    | PC = X;
+---+------+----+----------+-----------
| 2 | 0x02 | 10 | CLL X    | SP++;
|   |      |    |          | S[SP] = PC;
|   |      |    |          | PC = X;
+---+------+----+----------+-----------
| 1 | 0x03 | 00 | RET      | PC = S[SP];
|   |      |    |          | SP--;
+---+------+----+----------+-----------
|   | ..   |    |          |
+---+------+----+----------+-----------
| 1 | 0x0F | 00 | PNC      | F[P] = 1;
+---+------+----+----------+----------+
| READ/WRITE                          |
+---+------+----+----------+----------+
| 1 | 0x10 | 11 | MOV X, Y | //X moves to Y
|   |      |    |          | //X is undefined now
+---+------+----+----------+-----------
| 2 | 0x11 | 11 | CPY X, Y | Y = X;
+---+------+----+----------+-----------
| 2 | 0x12 | 11 | SWP X, Y | //X swaps with Y
+---+------+----+----------+----------+
| CONDITIONALS                        |
+---+------+----+----------+----------+
| 2 | 0x20 | 11 | IEQ X, Y | if( X == Y )
|   |      |    |          |     PC++;
+---+------+----+----------+-----------
| 2 | 0x21 | 11 | INQ X, Y | if( X != Y )
|   |      |    |          |     PC++;
+---+------+----+----------+-----------
| 2 | 0x22 | 11 | IGT X, Y | if( X > Y )
|   |      |    |          |     PC++;
+---+------+----+----------+-----------
| 2 | 0x23 | 11 | ILT X, Y | if( X < Y )
|   |      |    |          |     PC++;
+---+------+----+----------+-----------
| 2 | 0x24 | 11 | IGQ X, Y | if( X >= Y )
|   |      |    |          |     PC++;
+---+------+----+----------+-----------
| 2 | 0x25 | 11 | ILQ X, Y | if( X <= Y )
|   |      |    |          |     PC++;
+---+------+----+----------+----------+
| BINARY OPERATORS                    |
+---+------+----+----------+----------+
| 1 | 0x30 | 11 | NOT X, Y | Y = !X;
+---+------+----+----------+-----------
| 1 | 0x31 | 11 | OR  X, Y | Y |= X;
+---+------+----+----------+-----------
| 1 | 0x32 | 11 | AND X, Y | Y &= X;
+---+------+----+----------+-----------
| 1 | 0x33 | 11 | XOR X, Y | Y ^= X;
+---+------+----+----------+-----------
| 1 | 0x34 | 11 | RSF X, Y | Y >>= X;
+---+------+----+----------+-----------
| 1 | 0x35 | 11 | LSF X, Y | Y <<= X;
+---+------+----+----------+----------+
| ARITHMETIC OPERATORS                |
+---+------+----+----------+----------+
| 3 | 0x40 | 11 | ADD X, Y | Y += X;
|   |      |    |          | if( /* overflows */ )
|   |      |    |          |     F[O] = 1;
|   |      |    |          | else
|   |      |    |          |     F[O] = 0;
+---+------+----+----------+-----------
| 3 | 0x41 | 11 | SUB X, Y | Y -= X;
|   |      |    |          | if( /* underflows */ )
|   |      |    |          |     F[O] = 1;
|   |      |    |          | else
|   |      |    |          |     F[O] = 0;
+---+------+----+----------+-----------
| 6 | 0x42 | 11 | MUL X, Y | Y *= X;
|   |      |    |          | if( /* overflows */ )
|   |      |    |          |     F[O] = 1;
|   |      |    |          | else
|   |      |    |          |     F[O] = 0;
+---+------+----+----------+-----------
| 6 | 0x43 | 11 | DIV X, Y | Y /= X;
|   |      |    |          | if( /* rounds */ )
|   |      |    |          |     F[O] = 1;
|   |      |    |          | else
|   |      |    |          |     F[O] = 0;
+---+------+----+----------+-----------
| 8 | 0x44 | 11 | MOD X, Y | Y %= X;
+---+------+----+----------+----------+
| HARDWARE INTERACTION                |
+---+------+----+----------+----------+
| 4 | 0x50 | 11 | SND X, Y | //sends X to HW[Y]
|   |      |    |          | //HW[Y] will react immediately
+---+------+----+----------+----------+
```

## Hardware Modules
  * Hardware address is composed of (nibbles)
    * `CCAA` where
      * `C` - channel (hardware id)
      * `A` - databus address
    * ROM shall be connected as `HW[0xFF]`
      * whole ROM is copied to memory when the CPU starts
    * Memory shall be connected as `HW[0x00]`

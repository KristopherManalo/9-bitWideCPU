# Custom 9-bit wide cpu

***Written before I gained understanding of finer interactions of CPU and compiler design. Thus, has many architecture design decisions I do not agree with and would never make again.***



## Assembler
Compile:
```
make
```
Usage:
```
./fga [source] [options]
```
## Registers
R0 - hard coded to 0 \
R7 - result register of most instructions \
Register acceptable range for r<sub>i</sub> is from r<sub>i+1</sub> to r<sub>i+4</sub>, inclusive.
## Instructions

Instr | lhs | rhs | Notes
--------|--------|--------| -------- |
AND| r<sub>i</sub> | r<sub>j</sub> = [r<sub>i</sub>, r<sub>i+4</sub>] | Bitwise AND of r<sub>i</sub> and r<sub>j</sub>
ADD| r<sub>i</sub> | r<sub>j</sub> = [r<sub>i</sub>, r<sub>i+4</sub>] | Adds r<sub>i</sub> and r<sub>j</sub>
XOR| r<sub>i</sub> | r<sub>j</sub> = [r<sub>i</sub>, r<sub>i+4</sub>] | Bitwise XOR of r<sub>i</sub> and r<sub>j</sub>
LWD| r<sub>i</sub> | r<sub>j</sub> | r<sub>i</sub> = dat_mem[r<sub>j</sub>]
SWD| r<sub>i</sub> | r<sub>j</sub> | dat_mem[r<sub>j</sub>] = r<sub>i</sub>
BEQ| r<sub>i</sub> | 4-bit mapping | r<sub>i</sub> == r<sub>i+1</sub>
BLT| r<sub>i</sub> | 4-bit mapping | r<sub>i</sub> <  r<sub>i+1</sub>
BCO| null | 3-bit mapping | Bit mapping can be used with -r flag 
JMP| null | 6-bit buffer | Jumps into 1 of 32 subdivided space areas
ADI| r<sub>i</sub> | 3-bit immediate | Adds immediate as signed value [-4, 3]
LSL| r<sub>i</sub> | 2-bit immediate + 1 | Rolls value in register by immediate amount + 1
NOP| null | null | Assembler macro for CPU NOOP instruction
DON| null | null | Assembler macro for marking program as finished executing



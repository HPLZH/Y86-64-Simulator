# Y86-64 Simulator

## Simulator

This simulator can execute and debug Y86-64 programs in specific format.

Usage: `Y64 filename`

### Debugger commands
```
h        Display help message.
q        Quit.
s        Display current status.
a addr   Show the assembly command at given address.
r        Run program.
g        Run program to next breakpoint.
t addr   Run to given address.
b addr   Add breakpoint at given address.
d addr   Delete breakpoints at given address.
l        List all breakpoints.
m addr[63:32] addr[31:16]
         Export memory block to file.
v addr[63:32] addr[31:16]
         Export memory block to file and open it in VSCode.
```

### Y64 format

A Y64 file is a binary file.

```
[RIP : 8 bytes] [RSP : 8 bytes]
([Length : 8 bytes] [Start Address : 8 bytes] [Data : (Length) bytes])*n
[0xFFFFFFFFFFFFFFFF]
```

## Convert tools

### COE2Y64

This converter can convert COE files to Y64 files.

Usage: `python COE2Y64.py input_file output_file [initial_RIP] [initial_RSP]`

#!/bin/bash

FILE=regdump.txt

rm $FILE

# For CCM:
devregs 0x020C4060 >> $FILE
devregs 0x020C4064 >> $FILE
devregs 0x020C4068 >> $FILE
devregs 0x020C406C >> $FILE
devregs 0x020C4070 >> $FILE
devregs 0x020C4074 >> $FILE
devregs 0x020C4078 >> $FILE
devregs 0x020C407C >> $FILE
devregs 0x020C4080 >> $FILE
devregs 0x020C4088 >> $FILE

# For Power:
devregs 0x020C8110 >> $FILE
devregs 0x020C8114 >> $FILE
devregs 0x020C8118 >> $FILE
devregs 0x020C811C >> $FILE
devregs 0x020C8120 >> $FILE
devregs 0x020C8124 >> $FILE
devregs 0x020C8128 >> $FILE
devregs 0x020C812C >> $FILE
devregs 0x020C8130 >> $FILE
devregs 0x020C8134 >> $FILE
devregs 0x020C8138 >> $FILE
devregs 0x020C813C >> $FILE
devregs 0x020C8140 >> $FILE
devregs 0x020C8144 >> $FILE
devregs 0x020C8148 >> $FILE
devregs 0x020C814C >> $FILE
devregs 0x020C8150 >> $FILE
devregs 0x020C8154 >> $FILE
devregs 0x020C8158 >> $FILE
devregs 0x020C815C >> $FILE
devregs 0x020C8160 >> $FILE
devregs 0x020C8164 >> $FILE
devregs 0x020C8168 >> $FILE
devregs 0x020C816C >> $FILE
devregs 0x020C8170 >> $FILE
devregs 0x020C8174 >> $FILE
devregs 0x020C8178 >> $FILE
devregs 0x020C817C >> $FILE

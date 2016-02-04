<<<

Results:
  Starting Values                  ->  15AD    
  BRA  [!blueberry]                ->  25AD  1 cycle delay (expected this one)
  BRAE [!blueberry : F], r0 == r0  ->  26BD  3 cycle delay (no flush at all)
  BRAE [!blueberry : T], r0 == r0  ->  26BD  ? (there's an assembler bug, hint bit not set) 


Instruction Encodings: 
  BRA  [!blueberry]                 ->  5600...
  BRAE [!blueberry : F], r0 == r0   ->  1600...
  BRAE [!blueberry : T], r0 == r0   ->  1600...

  The encoding must be:

    [?][H][?][opcode x 5][rA x 8][rB x 8][rC x 8][imm x 32]
    
    The ? are P and R, I'm just not sure which is which. 
    How do I use predication?


Notes on FLUSH:
  This hint false business makes FLUSH a tricky bastard.

  Current implementation:
    FLUSH = BRAE [PC : F], r0 == r0 

  Original implementaion:
    FLUSH = BRANE [PC : T], r0 == r0 


  The current implementaion has a 3 cycle delay slot and it doesn't even flush the pipeline. 
  The original implementaion has a 1 cycle delay slot (which is noted in the thesis) but it does
  actually clear the pipeline. 

>>>

.constant TERM  0x4000
MOVI r41 = 0xD         #CR
MOVI r42 = 0xA         #LF


MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'
STOR [TERM] = r1
STOR [TERM] = r2
STOR [TERM] = r3
STOR [TERM] = r4
STOR [TERM] = r41
STOR [TERM] = r42




.mode NOP_DELAY off

# BRAE [!blueberry : F], r0 == r0
# BRAE [!blueberry : T], r0 == r0
# BRA [!blueberry]

ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1


!blueberry
  #gonna be hazards, but won't affect output.
  STOR [TERM] = r1
  STOR [TERM] = r2
  STOR [TERM] = r3
  STOR [TERM] = r4
  STOR [TERM] = r41
  STOR [TERM] = r42


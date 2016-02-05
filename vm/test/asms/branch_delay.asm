.mode NOP_DELAY off











<<<
  Test1: unconditional branch -- should have 1 delay slot.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'

BRA [!done_1]
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_1
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP











<<<
  Test2: Hint False (correctly) -- should fall through without branching at all.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'

BRANE [!done_2 : F], r0 != r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_2
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP











<<<
  Test3: Hint False (incorrectly) -- should have a 3 cycle delay slot.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'

BRAE [!done_3 : F], r0 == r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_3
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP











<<<
  Test4: Hint True (incorrectly) -- Flush should occur, leaving all registers untouched.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'

BRANE [!done_4 : T], r0 != r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_4
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP











<<<
  Test5: Hint true (correctly) -- should have 1 delay slot.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'

BRAE [!done_5 : T], r0 == r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_5
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
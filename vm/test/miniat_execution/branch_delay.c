#include <stdio.h>
#include <stdlib.h>

#include "miniat/miniat.h"
#include "miniat_priv_registers.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_hazards.h"

#include "CuTest.h"











void t_exec___branch_delay(CuTest *tc) {

  int i;
  FILE *bin_file;
  miniat *miniat;
  m_uword data;
  const char *log_name = __func__;


  /* Read in the binary testing file and instantiate a miniat. */
  char *path = OUTDIR"/bin/branch_delay.bin";
  bin_file = fopen(path, "rb");
  CuAssert(tc, "Should not be NULL", bin_file != NULL);

  miniat = miniat_new (bin_file);
  CuAssert(tc, "Should not be NULL", miniat != NULL);











  /* Process instructions until we reach the end of section 1. */
  while(true) {
    miniat_clock(miniat);
    if(miniat->pipeline.fetch.out.NPC == 0x201E) {
      break;
    }
  }

  /* 
   * Now lets check the registers to see how many delay slots we ran. 
   *
   * Instruction Sequence:                      (Hint True Correctly)
   *   BRA [!done_1]                
   *   r1 =r1 + 1
   *   r2 =r2 + 1
   *   r3 =r3 + 1
   *   r4 =r4 + 1
   *
   * Starting values:
   *   r1: '1'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   *
   * Expected ending values:
   *   r1: '2'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   */
  CuAssert(tc, "Register 1 should be 0x32 ('2')", miniat->reg[1] == 0x32); 
  CuAssert(tc, "Register 2 should be 0x35 ('5')", miniat->reg[2] == 0x35); 
  CuAssert(tc, "Register 3 should be 0x41 ('A')", miniat->reg[3] == 0x41); 
  CuAssert(tc, "Register 4 should be 0x44 ('D')", miniat->reg[4] == 0x44); 











  /* Process instructions until we reach the end of section 2. */
  while(true) {
    miniat_clock(miniat);
    if(miniat->pipeline.fetch.out.NPC == 0x203E) {
      break;
    }
  }

  /* 
   * Now lets check the registers to see how many delay slots we ran. 
   *
   * Instruction Sequence:                     (Hint False Correctly)
   *   BRANE [!done_2 : F], r0 != r0
   *   r1 =r1 + 1
   *   r2 =r2 + 1
   *   r3 =r3 + 1
   *   r4 =r4 + 1
   *
   * Starting values:            
   *   r1: '1'                     
   *   r2: '5'                     
   *   r3: 'A'
   *   r4: 'D'
   *
   * Expected ending values:
   *   r1: '2'
   *   r1: '6'
   *   r3: 'B'
   *   r4: 'E'
   */
  CuAssert(tc, "Register 1 should be 0x32 ('2')", miniat->reg[1] == 0x32); 
  CuAssert(tc, "Register 2 should be 0x36 ('6')", miniat->reg[2] == 0x36); 
  CuAssert(tc, "Register 3 should be 0x42 ('B')", miniat->reg[3] == 0x42); 
  CuAssert(tc, "Register 4 should be 0x45 ('E')", miniat->reg[4] == 0x45); 











  /* Process instructions until we reach the end of section 3. */
  while(true) {
    miniat_clock(miniat);
    if(miniat->pipeline.fetch.out.NPC == 0x205E) {
      break;
    }
  }

  /* 
   * Now lets check the registers to see how many delay slots we ran. 
   * 
   * Instruction Sequence:                   (Hint False Incorrectly) 
   *   BRAE [!done_3 : F], r0 == r0
   *   r1 =r1 + 1
   *   r2 =r2 + 1
   *   r3 =r3 + 1
   *   r4 =r4 + 1
   *
   * Starting values:
   *   r1: '1'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   *
   * Expected ending values:
   *   r1: '2'
   *   r2: '6'
   *   r3: 'B'
   *   r4: 'D'
   */
  CuAssert(tc, "Register 1 should be 0x32 ('2')", miniat->reg[1] == 0x32); 
  CuAssert(tc, "Register 2 should be 0x36 ('6')", miniat->reg[2] == 0x36); 
  CuAssert(tc, "Register 3 should be 0x42 ('B')", miniat->reg[3] == 0x42); 
  CuAssert(tc, "Register 4 should be 0x44 ('D')", miniat->reg[4] == 0x44); 
 










  /* Process instructions until we reach the end of section 4. */
  while(true) {
    miniat_clock(miniat);
    if(miniat->pipeline.fetch.out.NPC == 0x207E) {
      break;
    }
  }

  /* 
   * Now lets check the registers to see how many delay slots we ran. 
   * 
   * Instruction Sequence:                    (Hint True Incorrectly) 
   *   BRAE [!done_4 : T], r0 == r0
   *   r1 =r1 + 1
   *   r2 =r2 + 1
   *   r3 =r3 + 1
   *   r4 =r4 + 1
   *
   * Starting values:
   *   r1: '1'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   *
   * Expected ending values:
   *   r1: '1'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   */
  CuAssert(tc, "Register 1 should be 0x31 ('1')", miniat->reg[1] == 0x31); 
  CuAssert(tc, "Register 2 should be 0x35 ('5')", miniat->reg[2] == 0x35); 
  CuAssert(tc, "Register 3 should be 0x41 ('A')", miniat->reg[3] == 0x41); 
  CuAssert(tc, "Register 4 should be 0x44 ('D')", miniat->reg[4] == 0x44);  











  /* Process instructions until we reach the end of section 5. */
  while(true) {
    miniat_clock(miniat);
    if(miniat->pipeline.fetch.out.NPC == 0x209E) {
      break;
    }
  }

  /* 
   * Now lets check the registers to see how many delay slots we ran. 
   * 
   * Instruction Sequence:         ((Explicitly) Hint True Correctly) 
   *   BRAE [!done_5 : F], r0 == r0
   *   r1 =r1 + 1
   *   r2 =r2 + 1
   *   r3 =r3 + 1
   *   r4 =r4 + 1
   *
   * Starting values:
   *   r1: '1'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   *
   * Expected ending values:
   *   r1: '2'
   *   r2: '5'
   *   r3: 'A'
   *   r4: 'D'
   */
  CuAssert(tc, "Register 1 should be 0x31 ('2')", miniat->reg[1] == 0x32); 
  CuAssert(tc, "Register 2 should be 0x35 ('5')", miniat->reg[2] == 0x35); 
  CuAssert(tc, "Register 3 should be 0x41 ('A')", miniat->reg[3] == 0x41); 
  CuAssert(tc, "Register 4 should be 0x44 ('D')", miniat->reg[4] == 0x44);  











  miniat_free(miniat);
  return;
}


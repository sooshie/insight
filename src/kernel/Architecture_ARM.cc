/*-
 * Copyright (C) 2010-2014, Centre National de la Recherche Scientifique,
 *                          Institut Polytechnique de Bordeaux,
 *                          Universite de Bordeaux.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "Architecture_ARM.hh"

Architecture_ARM::Architecture_ARM (endianness_t endianness)
  : Architecture (ARM, endianness, 32, 32)
{
  /* Setting regular registers */
  add_register ("r0", 32);  /* Function call argument register 1 */
  add_register ("r1", 32);  /* Function call argument register 2 */
  add_register ("r2", 32);  /* Function call argument register 3 */
  add_register ("r3", 32);  /* Function call argument register 4 */
  add_register ("r4", 32);  /* Temporary register 1 */
  add_register ("r5", 32);  /* Temporary register 2 */
  add_register ("r6", 32);  /* Temporary register 3 */
  add_register ("r7", 32);  /* Temporary register 4 */
  add_register ("r8", 32);  /* Temporary register 5 */
  add_register ("r9", 32);  /* Temporary register 6 */
  add_register ("r10", 32); /* Temporary register 7 */
  add_register ("r11", 32); /* Temporary register 8 */
  add_register ("r12", 32); /* Intra-procedure-call scratch register */
  add_register ("r13", 32); /* Stack pointer */
  add_register ("r14", 32); /* Link register */
  add_register ("r15", 32); /* Program counter */

  /* Setting status flags */
  add_register ("z", 1); /* Zero flag */
  add_register ("n", 1); /* Negative flag */
  add_register ("c", 1); /* Carry flag */
  add_register ("v", 1); /* Overflow flag */
}

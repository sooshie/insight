/*-
 * Copyright (C) 2010-2012, Centre National de la Recherche Scientifique,
 *                          Institut Polytechnique de Bordeaux,
 *                          Universite Bordeaux 1.
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

#include "x86_32_translation_functions.hh"
 
using namespace std;

static void
s_jcc (MicrocodeAddress &from, x86_32::parser_data &data, 
       Expr *jmp, Expr *cond, MicrocodeAddress *to = NULL)
{
  MemCell *mc = dynamic_cast<MemCell *> (jmp);  
  assert (mc != NULL);
  Constant *cst = dynamic_cast<Constant *> (mc->get_addr ());  
  MicrocodeAddress last_addr = to ? *to : (from + 2);

  assert (cst != NULL);

  data.mc->add_skip (from, MicrocodeAddress (cst->get_val ()), cond);
  Expr *notcond = NULL;
  if (cond->is_UnaryApp ())
    {
      UnaryApp *ua = dynamic_cast<UnaryApp *> (cond);  
      if (ua->get_op () == LNOT || ua->get_op () == NOT)
	notcond = ua->get_arg1 ()->ref ();
    }

  if (notcond == NULL)
    notcond = UnaryApp::create (NOT, cond->ref (), 0, 1);

  data.mc->add_skip (from, last_addr, notcond);
  jmp->deref ();
}

#define X86_32_CC(cc, f) \
  X86_32_TRANSLATE_1_OP (J ## cc) \
  { s_jcc (data.start_ma, data, op1, \
	   data.condition_codes[x86_32::parser_data::X86_32_CC_ ## cc]->ref (), \
	   &data.next_ma); }

#include "x86_32_cc.def"
#undef X86_32_CC

X86_32_TRANSLATE_1_OP(JC)
{
  s_jcc (data.start_ma, data, op1, data.get_flag ("cf"), &data.next_ma); 
}

X86_32_TRANSLATE_1_OP(JCXZ)
{
  s_jcc (data.start_ma, data, op1, 
	 BinaryApp::create (EQ, data.get_register ("cx"), 
			    Constant::zero (16), 0, 1),
	 &data.next_ma); 
}

X86_32_TRANSLATE_1_OP(JECXZ)
{
  s_jcc (data.start_ma, data, op1, 
	 BinaryApp::create (EQ, data.get_register ("ecx"), 
			    Constant::zero (32), 0, 1),
	 &data.next_ma); 
}

X86_32_TRANSLATE_1_OP (JMP)
{
  MemCell *mc = dynamic_cast<MemCell *> (op1);  
  
  assert (mc != NULL);

  Expr *addr = mc->get_addr ();

  if (addr->is_Constant ())
    {
      Constant *cst = dynamic_cast<Constant *> (addr);
      data.mc->add_skip (data.start_ma, MicrocodeAddress (cst->get_val ()));
    }
  else 
    {
      data.mc->add_jump (data.start_ma, addr->ref ());
    }
  mc->deref ();
}

X86_32_TRANSLATE_1_OP (JMPW)
{
  x86_32_translate_with_size (data, op1, BV_DEFAULT_SIZE,
			      x86_32_translate<X86_32_TOKEN(JMP)>);
}

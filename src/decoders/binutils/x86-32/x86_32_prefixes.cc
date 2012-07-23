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

X86_32_TRANSLATE_PREFIX(ADDR16)
{
  if (start)
    {
      data.addr_size = data.addr16;
      data.addr16 = true;
    }
  else
    {
      data.addr16 = data.addr_size;
    }
}

X86_32_TRANSLATE_PREFIX(ADDR32)
{
  if (start)
    {
      data.addr_size = data.addr16;
      data.addr16 = false;
    }
  else
    {
      data.addr16 = data.addr_size;
    }
}

X86_32_TRANSLATE_PREFIX(DATA32)
{
  if (start)
    {
      data.data_size = data.data16;
      data.data16 = false;
    }
  else
    {
      data.data16 = data.data_size;
    }
}

X86_32_TRANSLATE_PREFIX(DATA16)
{
  if (start)
    {
      data.data_size = data.data16;
      data.data16 = true;
    }
  else
    {
      data.data16 = data.data_size;
    }
}

X86_32_TRANSLATE_PREFIX(LOCK)
{
  if (start)
    data.lock = true;
}

static void
s_start_rep (x86_32::parser_data &data)
{
  assert (! data.has_prefix);

  const char *regname = data.addr16 ? "cx" : "ecx";
  LValue *counter = data.get_register (regname);
  Expr *stopcond = BinaryApp::create (EQ, counter, Constant::zero ());
  data.has_prefix = true;

  data.mc->add_skip (data.start_ma, data.next_ma, stopcond);
  data.mc->add_skip (data.start_ma, data.start_ma + 1,
		     UnaryApp::create (LNOT, stopcond->ref ()));
  data.start_ma++;
}

static void
s_end_rep (x86_32::parser_data &data, Expr *cond)
{
  MicrocodeAddress start (data.start_ma);
  const char *regname = data.addr16 ? "cx" : "ecx";
  LValue *counter = data.get_register (regname);
  Expr *stopcond = BinaryApp::create (EQ, counter->ref (), Constant::zero ());

  data.mc->add_assignment (start, counter, 
			   BinaryApp::create (SUB, counter->ref (),
					      Constant::one ()));

  if (cond)    
    {
      cond = BinaryApp::create (EQ, data.get_register ("zf"), cond);
      stopcond = BinaryApp::create (OR, stopcond, cond);
    }

  data.mc->add_skip (start, data.next_ma, stopcond);
  data.mc->add_skip (start, MicrocodeAddress (data.start_ma.getGlobal ()),
		     UnaryApp::create (NOT, stopcond->ref ()));
  
  data.has_prefix = false;
}

static void
s_rep (x86_32::parser_data &data, bool start, Expr *zf_val)
{
  if (start)
    s_start_rep (data);
  else
    s_end_rep (data, zf_val);
}

X86_32_TRANSLATE_PREFIX(REP)
{  
  s_rep (data, start, NULL);
}

X86_32_TRANSLATE_PREFIX(REPE)
{
  s_rep (data, start, start ? NULL : Constant::zero ());
}

X86_32_TRANSLATE_PREFIX(REPZ)
{
  s_rep (data, start, start ? NULL : Constant::zero ());
}

X86_32_TRANSLATE_PREFIX(REPNE)
{
  s_rep (data, start, start ? NULL : Constant::one ());
}

X86_32_TRANSLATE_PREFIX(REPNZ)
{
  s_rep (data, start, start ? NULL : Constant::one ());
}
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

#include "x86_translate.hh"

using namespace std;

static void
s_cmps (x86::parser_data &data, int size)
{
  MicrocodeAddress from = data.start_ma;

  Expr *si = data.get_register ("esi");
  Expr *di = data.get_register ("edi");
  Expr *op1 = MemCell::create (si->ref (), 0, size);
  Expr *op2 = MemCell::create (di->ref (), 0, size);

  x86_cmpgen (from, data, op1, op2, NULL);

  MicrocodeAddress next = data.has_prefix ? from + 5 : data.next_ma;
  Expr *inc = Constant::create (size / 8, 0, si->get_bv_size ());

  x86_if_then_else (from, data, data.get_flag ("df"), from + 3, from + 1);
  from++;
  /* pc = from + 1 */
  data.mc->add_assignment (from, (LValue *) si->ref (),
			   BinaryApp::create (BV_OP_ADD, si->ref (),
					      inc->ref (),
					      0, si->get_bv_size ()));
  /* pc = from + 2 */
  data.mc->add_assignment (from, (LValue *) di->ref (),
			   BinaryApp::create (BV_OP_ADD, di->ref (),
					      inc->ref (),
					      0, di->get_bv_size ()),
			   next);
  from++;
  /* pc = from + 3 */
  data.mc->add_assignment (from, (LValue *) si->ref (),
			   BinaryApp::create (BV_OP_SUB, si->ref (),
					      inc->ref (),
					      0, si->get_bv_size ()));
  /* pc = from + 4 */
  data.mc->add_assignment (from, (LValue *) di->ref (),
			   BinaryApp::create (BV_OP_SUB, di->ref (),
					      inc->ref (),
					      0, di->get_bv_size ()),
			   next);

  if (data.has_prefix)
    data.start_ma = next;

  inc->deref ();
  si->deref ();
  di->deref ();
}

			/* --------------- */

X86_TRANSLATE_0_OP(CMPSB)
{
  s_cmps (data, 8);
}

X86_TRANSLATE_2_OP(CMPSB)
{
  x86_translate<X86_TOKEN(CMPSB)> (data);
  op1->deref ();
  op2->deref ();
}

X86_TRANSLATE_0_OP(CMPSW)
{
  s_cmps (data, 16);
}

X86_TRANSLATE_2_OP(CMPSW)
{
  x86_translate<X86_TOKEN(CMPSW)> (data);
  op1->deref ();
  op2->deref ();
}

X86_TRANSLATE_0_OP(CMPSD)
{
  s_cmps (data, 32);
}

X86_TRANSLATE_2_OP(CMPSD)
{
  x86_translate<X86_TOKEN(CMPSD)> (data);
  op1->deref ();
  op2->deref ();
}

X86_TRANSLATE_2_OP(LODS)
{
  op1->deref ();

  Expr *_si = NULL;
  switch (data.addr_mode)
    {
    case x86::parser_data::MODE_16:
      _si =  data.get_register ("si");
      break;

    case x86::parser_data::MODE_32:
      _si = data.get_register ("esi");
      break;

    case x86::parser_data::MODE_64:
      _si = data.get_register ("rsi");
      break;
    }

  assert(_si != NULL);

  Expr *dst = op2->ref ();
  Expr *src = MemCell::create (_si->ref (), 0, dst->get_bv_size ());
  Expr *inc =
    Constant::create (dst->get_bv_size ()  / 8, 0, _si->get_bv_size ());
  MicrocodeAddress from (data.start_ma);
  MicrocodeAddress next = data.has_prefix ? from + 4 : data.next_ma;

  /* pc = start */
  data.mc->add_assignment (from, (LValue *) dst->ref (), src->ref ());
  /* pc = start + 1*/
  x86_if_then_else (from, data, data.get_flag ("df"), from + 1, from + 2);
  from++;
  /* pc = start + 2*/
  data.mc->add_assignment (from, (LValue *) _si->ref (),
			   BinaryApp::create (BV_OP_SUB, _si->ref (),
					      inc->ref (),
					      0, _si->get_bv_size ()),
			   data.next_ma);
  from++;
  /* pc = start + 3*/
  data.mc->add_assignment (from, (LValue *) _si->ref (),
			   BinaryApp::create (BV_OP_ADD, _si->ref (),
					      inc->ref (),
					      0, _si->get_bv_size ()),
			   data.next_ma);

  if (data.has_prefix)
    data.start_ma = next;

  _si->deref ();
  inc->deref ();
  op2->deref ();
  src->deref ();
  dst->deref ();
}

X86_TRANSLATE_2_OP(STOS)
{
  op2->deref ();
  Expr *rdi = NULL;
  switch (data.addr_mode)
    {
    case x86::parser_data::MODE_16:
      rdi =  data.get_register ("di");
      break;

    case x86::parser_data::MODE_32:
      rdi = data.get_register ("edi");
      break;

    case x86::parser_data::MODE_64:
      rdi = data.get_register ("rdi");
      break;

      // TODO: Add an error case when 'default' is reached ?
    }

  Expr *src = op1->ref ();
  Expr *dst = MemCell::create (rdi->ref (), 0, src->get_bv_size ());
  Expr *inc =
    Constant::create (src->get_bv_size ()  / 8, 0, rdi->get_bv_size ());
  MicrocodeAddress from (data.start_ma);
  MicrocodeAddress next = data.has_prefix ? from + 4 : data.next_ma;

  /* pc = start */
  data.mc->add_assignment (from, (LValue *) dst->ref (), src->ref ());
  /* pc = start + 1*/
  x86_if_then_else (from, data, data.get_flag ("df"),
		       from + 1, from + 2);
  from++;
  /* pc = start + 2 */
  data.mc->add_assignment (from, (LValue *) rdi->ref (),
			   BinaryApp::create (BV_OP_SUB, rdi->ref (),
					      inc->ref (),
					      0, rdi->get_bv_size ()),
			   next);
  from++;
  /* pc = start + 3 */
  data.mc->add_assignment (from, (LValue *) rdi->ref (),
			   BinaryApp::create (BV_OP_ADD, rdi->ref (),
					      inc->ref (),
					      0, rdi->get_bv_size ()),
			   next);

  if (data.has_prefix)
    data.start_ma = next;

  rdi->deref ();
  inc->deref ();
  op1->deref ();
  src->deref ();
  dst->deref ();
}


static void
s_mov (x86::parser_data &data, int nb_bytes)
{
  MicrocodeAddress start (data.start_ma);
  MicrocodeAddress next = data.has_prefix ? start + 6 : data.next_ma;
  MicrocodeAddress ifaddr;

  LValue *_si = NULL;
  LValue *_di = NULL;
  switch (data.addr_mode)
    {
    case x86::parser_data::MODE_16:
      _si =  data.get_register ("si");
      _di =  data.get_register ("di");
      break;

    case x86::parser_data::MODE_32:
      _si = data.get_register ("esi");
      _di = data.get_register ("edi");
      break;

    case x86::parser_data::MODE_64:
      _si = data.get_register ("rsi");
      _di = data.get_register ("rdi");
      break;
    }

  assert(_si != NULL);
  assert(_di != NULL);

  int csize = _si->get_bv_size ();
  Expr *inc = Constant::create (nb_bytes, 0, _si->get_bv_size ());

  data.mc->add_assignment (start,
			   MemCell::create (_di, 0, 8 * nb_bytes),
			   MemCell::create (_si, 0, 8 * nb_bytes),
			   start + 1);

  x86_if_then_else (start + 1, data, data.get_flag ("df"),
		       start + 4, start + 2);

  data.mc->add_assignment (start + 2, (LValue *) _si->ref (),
			   BinaryApp::create (BV_OP_ADD, _si->ref (),
					      inc->ref (),
					      0, csize),
			   start + 3);

  data.mc->add_assignment (start + 3, (LValue *) _di->ref (),
			   BinaryApp::create (BV_OP_ADD, _di->ref (),
					      inc->ref (),
					      0, csize),
			   next);

  data.mc->add_assignment (start + 4, (LValue *) _si->ref (),
			   BinaryApp::create (BV_OP_SUB, _si->ref (),
					      inc->ref (),
					      0, csize),
			   start + 5);

  data.mc->add_assignment (start + 5, (LValue *) _di->ref (),
			   BinaryApp::create (BV_OP_SUB, _di->ref (),
					      inc->ref (),
					      0, csize),
			   next);
  if (data.has_prefix)
    data.start_ma = next;
  inc->deref ();
}

X86_TRANSLATE_0_OP(MOVSB)
{
  s_mov (data, 1);
}

X86_TRANSLATE_2_OP(MOVSB)
{
  x86_translate<X86_TOKEN(MOVSB)> (data);
  op1->deref ();
  op2->deref ();
}

X86_TRANSLATE_0_OP(MOVSW)
{
  s_mov (data, 2);
}

X86_TRANSLATE_2_OP(MOVSW)
{
  x86_translate<X86_TOKEN(MOVSW)> (data);
  op1->deref ();
  op2->deref ();
}

X86_TRANSLATE_0_OP(MOVSL)
{
  s_mov (data, 4);
}

X86_TRANSLATE_2_OP(MOVSL)
{
  x86_translate<X86_TOKEN(MOVSL)> (data);
  op1->deref ();
  op2->deref ();
}

X86_TRANSLATE_2_OP(SCAS)
{
  MicrocodeAddress from (data.start_ma);

  Expr::extract_bit_vector (op1, 0, op2->get_bv_size ());

  x86_cmpgen (from, data, op1, op2, NULL);
  MicrocodeAddress next = data.has_prefix ? from + 3 : data.next_ma;
  x86_if_then_else (from, data, data.get_register ("df"),
		       from + 1, from + 2);
  from++;

  LValue *_di = NULL;
  switch (data.addr_mode)
    {
    case x86::parser_data::MODE_16:
      _di =  data.get_register ("di");
      break;

    case x86::parser_data::MODE_32:
      _di = data.get_register ("edi");
      break;

    case x86::parser_data::MODE_64:
      _di = data.get_register ("rdi");
      break;
    }

  assert(_di != NULL);

  Expr *inc = Constant::create (op1->get_bv_size () / 8, 0, _di->get_bv_size ());
  data.mc->add_assignment (from, (LValue *) _di->ref (),
			   BinaryApp::create (BV_OP_SUB, _di->ref (),
					      inc->ref (), 0,
					      _di->get_bv_size ()),
			   next);
  from++;
  data.mc->add_assignment (from, (LValue *) _di->ref (),
			   BinaryApp::create (BV_OP_ADD, _di->ref (),
					      inc->ref (), 0,
					      _di->get_bv_size ()),
			   next);
  if (data.has_prefix)
    data.start_ma = next;
  _di->deref ();
  inc->deref ();
}

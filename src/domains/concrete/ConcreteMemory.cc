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

#include "ConcreteMemory.hh"

#include <cassert>
#include <inttypes.h>

#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>

#include <domains/concrete/ConcreteAddress.hh>

#include <utils/bv-manip.hh>


using namespace std;

/*****************************************************************************/
/* Constructors                                                              */
/*****************************************************************************/

ConcreteMemory::ConcreteMemory() :
  Memory<ConcreteAddress, ConcreteValue>(), RegisterMap<ConcreteValue>(),
  base (NULL), memory (), minaddr (MAX_ADDRESS), maxaddr (NULL_ADDRESS)
{
}

ConcreteMemory::ConcreteMemory(const ConcreteMemory &m) :
  Memory<ConcreteAddress,ConcreteValue> (m), RegisterMap<ConcreteValue> (m),
  base (m.base), memory (m.memory), minaddr (m.minaddr), maxaddr (m.maxaddr)
{
}

ConcreteMemory::ConcreteMemory(const ConcreteMemory *base) :
  Memory<ConcreteAddress,ConcreteValue> (), RegisterMap<ConcreteValue> (),
  base (base)
{
  if (base)
    base->get_address_range (minaddr, maxaddr);
  else
    {
      minaddr = MAX_ADDRESS;
      maxaddr = NULL_ADDRESS;
    }
}

ConcreteMemory::~ConcreteMemory()
{
  memory.clear ();
}

/*****************************************************************************/
/* Memory Access                                                             */
/*****************************************************************************/

ConcreteValue
ConcreteMemory::get(const ConcreteAddress &addr,
		    const int size,
		    const Architecture::endianness_t e) const
  throw (UndefinedValueException)
{
  word_t res = 0;
  address_t a = addr.get_address();

  for (int i = 0; i < size; i++)
    {
      address_t cur =
	(e == Architecture::LittleEndian ? a + size - i - 1 : a + i);
      word_t byte;

      if (!is_defined (ConcreteAddress (cur)))
	throw UndefinedValueException("at address " + addr.to_string ());
      MemoryMap::const_iterator ci = memory.find (cur);
      if (ci != memory.end ())
	byte = ci->second;
      else
	{
	  assert (base->is_defined (addr));
	  byte = base->get (addr, 1, e).get ();
	}

      res = (res << 8) | byte;
    }

  return ConcreteValue (8 * size, res);
}

void
ConcreteMemory::put(const ConcreteAddress &addr,
		    const ConcreteValue &value,
		    const Architecture::endianness_t e)
{
  word_t v = value.get();
  int size = value.get_size();
  address_t a = addr.get_address();

  if (size % 8)
    assert("cannot write value with non multiple of 8 size\n");

  size /= 8;

  for (int i = 0; i < size; i++)
    {
      address_t cur =
	(e == Architecture::BigEndian ? a + size - i - 1 : a + i);

      memory[cur] = v & 0xff;
      v >>= 8;
    }

  if (a < minaddr)
    minaddr = a;
  if (maxaddr < a)
    maxaddr = a;
}

bool
ConcreteMemory::is_defined(const ConcreteAddress &a) const
{
  return (memory.find(a.get_address()) != memory.end() ||
	  (base && base->is_defined (a)));
}

bool
ConcreteMemory::is_defined(const RegisterDesc *r) const
{
  return (RegisterMap<ConcreteValue>::is_defined (r) ||
	  (base && base->is_defined (r)));
}

ConcreteValue
ConcreteMemory::get(const RegisterDesc * r) const
    throw (UndefinedValueException)
{
  assert (! r->is_alias ());
  int offset = r->get_window_offset ();
  int size = r->get_window_size ();
  ConcreteValue regval;

  if (RegisterMap<ConcreteValue>::is_defined (r))
    regval = RegisterMap<ConcreteValue>::get (r);
  else if (base)
    regval = base->get (r);
  else
    throw UndefinedValueException ("for register " + r->to_string ());

  word_t val = BitVectorManip::extract_from_word (regval.get (), offset, size);
  return ConcreteValue (size, val);
}

/*****************************************************************************/
/* Utils                                                                     */
/*****************************************************************************/

bool
ConcreteMemory::equals (const ConcreteMemory &mem) const
{
  if (memory.size () != mem.memory.size ())
    return false;

  if (base != mem.base)
    return false;

  for (MemoryMap::const_iterator i = memory.begin (); i != memory.end (); i++)
    {
      if (! mem.is_defined (i->first) ||
	  ! (mem.get (i->first, 1,
		      Architecture::LittleEndian).get () == i->second))
	    return false;
    }

  for (RegisterMap<ConcreteValue>::const_reg_iterator i = mem.regs_begin ();
       i != mem.regs_end (); i++) {
    if (! is_defined (i->first) || ! (get (i->first).equals(i->second)))
      return false;
  }

  return true;
}

std::size_t
ConcreteMemory::hashcode () const
{
  std::size_t result = 0;

  for (MemoryMap::const_iterator i = memory.begin (); i != memory.end (); i++)
    result = ((result << 3) + 19 * i->second);

  for (RegisterMap<ConcreteValue>::const_reg_iterator i = regs_begin ();
       i != regs_end (); i++) {
    result = ((result << 3) + 19 * (intptr_t) i->second.get ());
  }

  return result;
}

void
ConcreteMemory::output_text(ostream &os) const
{
  os << "Memory: " << endl;
  for (MemoryMap::const_iterator mem = memory.begin(); mem != memory.end();
       mem++)
    os << "[ 0x" << hex << setfill('0')
       << nouppercase << setw(4) << (int) mem->first
       << " -> 0x" << hex << setfill('0')
       << nouppercase << setw(2) << (int) mem->second << "]" << endl;
  os << endl;

  os << "Registers: " << endl;
  RegisterMap<ConcreteValue>::output_text (os);
}

void
ConcreteMemory::get_address_range (address_t &min, address_t &max) const
{
  min = minaddr;
  max = maxaddr;
}

ConcreteMemory *
ConcreteMemory::clone () const
{
  ConcreteMemory *result = new ConcreteMemory (*this);
  return result;
}

ConcreteMemory::const_memcell_iterator
ConcreteMemory::begin () const
{
  return memory.begin ();
}

ConcreteMemory::const_memcell_iterator
ConcreteMemory::end () const
{
  return memory.end ();
}

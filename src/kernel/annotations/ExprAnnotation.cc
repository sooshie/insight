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

#include <kernel/annotations/ExprAnnotation.hh>

ExprAnnotation::ExprAnnotation()
  : Annotation (), expr (NULL)
{
}

ExprAnnotation::ExprAnnotation (const ExprAnnotation &other)
  : Annotation (other), expr (NULL)
{
  set_expr (other.expr);
}

ExprAnnotation::ExprAnnotation (const Expr *F)
  : Annotation (), expr (NULL)
{
  set_expr (F);
}

ExprAnnotation::~ExprAnnotation()
{
  if (expr != NULL)
    expr->deref ();
}

void
ExprAnnotation::output_text (std::ostream &out) const
{
  expr->output_text (out);
}

void *
ExprAnnotation::clone() const
{
  return new ExprAnnotation (expr);
}

void
ExprAnnotation::set_expr (const Expr *F)
{
  if (expr != NULL)
    expr->deref ();
  if (F != NULL)
    expr = F->ref ();
  else
    expr = NULL;
}

			/* --------------- */

const Expr *
ExprAnnotation::get_expr () const
{
  return expr;
}

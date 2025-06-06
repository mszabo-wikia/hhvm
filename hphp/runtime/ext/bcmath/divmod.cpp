/* divmod.c: bcmath library file. */
/*
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.
    Copyright (C) 2000 Philip A. Nelson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.  (COPYING.LIB)

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to:

      The Free Software Foundation, Inc.
      51 Franklin Street, Fifth Floor
      Boston, MA  02110-1301, USA

    You may contact the author by:
       e-mail:  philnelson@acm.org
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062

*************************************************************************/

#include <stdlib.h>
#include "hphp/runtime/ext/bcmath/bcmath.h"

#include <folly/ScopeGuard.h>

/* Division *and* modulo for numbers.  This computes both NUM1 / NUM2 and
   NUM1 % NUM2  and puts the results in QUOT and REM, except that if QUOT
   is NULL then that store will be omitted.
 */

int
bc_divmod (bc_num num1, bc_num num2, bc_num *quot, bc_num *rem, int scale TSRMLS_DC)
{
  bc_num quotient = NULL;
  bc_num temp;
  int rscale;

  /* Check for correct numbers. */
  if (bc_is_zero (num2 TSRMLS_CC)) return -1;

  /* Calculate final scale. */
  rscale = MAX (num1->n_scale, num2->n_scale+scale);
  bc_init_num(&temp TSRMLS_CC);
  SCOPE_EXIT { bc_free_num(&temp); };

  /* Calculate it. */
  bc_divide (num1, num2, &temp, scale TSRMLS_CC);
  if (quot)
    quotient = bc_copy_num (temp);
  bc_multiply (temp, num2, &temp, rscale TSRMLS_CC);
  bc_sub (num1, temp, rem, rscale);

  if (quot)
    {
      bc_free_num (quot);
      *quot = quotient;
    }

  return 0;	/* Everything is OK. */
}


/* Modulo for numbers.  This computes NUM1 % NUM2  and puts the
   result in RESULT.   */

int
bc_modulo (bc_num num1, bc_num num2, bc_num *result, int scale TSRMLS_DC)
{
  return bc_divmod (num1, num2, NULL, result, scale TSRMLS_CC);
}

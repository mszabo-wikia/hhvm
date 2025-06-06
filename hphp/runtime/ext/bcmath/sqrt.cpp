/* sqrt.c: bcmath library file. */
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

#include "hphp/runtime/ext/bcmath/bcmath.h"

#include <folly/ScopeGuard.h>

/* Take the square root NUM and return it in NUM with SCALE digits
   after the decimal place. */

int
bc_sqrt (bc_num *num, int scale TSRMLS_DC)
{
  int rscale, cmp_res, done;
  int cscale;
  bc_num guess, guess1, point5, diff;

  /* Initial checks. */
  cmp_res = bc_compare (*num, BCG(_zero_));
  if (cmp_res < 0)
    return 0;		/* error */
  else
    {
      if (cmp_res == 0)
	{
	  bc_free_num (num);
	  *num = bc_copy_num (BCG(_zero_));
	  return 1;
	}
    }
  cmp_res = bc_compare (*num, BCG(_one_));
  if (cmp_res == 0)
    {
      bc_free_num (num);
      *num = bc_copy_num (BCG(_one_));
      return 1;
    }

  /* Initialize the variables. */
  rscale = MAX (scale, (*num)->n_scale);
  bc_init_num(&guess TSRMLS_CC);
  SCOPE_EXIT { bc_free_num(&guess); };
  bc_init_num(&guess1 TSRMLS_CC);
  SCOPE_EXIT { bc_free_num(&guess1); };
  bc_init_num(&diff TSRMLS_CC);
  SCOPE_EXIT { bc_free_num(&diff); };
  point5 = bc_new_num (1,1);
  SCOPE_EXIT { bc_free_num(&point5); };
  point5->n_value[1] = 5;

  /* Calculate the initial guess. */
  if (cmp_res < 0)
    {
      /* The number is between 0 and 1.  Guess should start at 1. */
      guess = bc_copy_num (BCG(_one_));
      cscale = (*num)->n_scale;
    }
  else
    {
      /* The number is greater than 1.  Guess should start at 10^(exp/2). */
      bc_int2num (&guess,10);

      bc_int2num (&guess1,(*num)->n_len);
      bc_multiply (guess1, point5, &guess1, 0 TSRMLS_CC);
      guess1->n_scale = 0;
      bc_raise (guess, guess1, &guess, 0 TSRMLS_CC);
      bc_free_num (&guess1);
      cscale = 3;
    }

  /* Find the square root using Newton's algorithm. */
  done = FALSE;
  while (!done)
    {
      bc_free_num (&guess1);
      guess1 = bc_copy_num (guess);
      bc_divide (*num, guess, &guess, cscale TSRMLS_CC);
      bc_add (guess, guess1, &guess, 0);
      bc_multiply (guess, point5, &guess, cscale TSRMLS_CC);
      bc_sub (guess, guess1, &diff, cscale+1);
      if (bc_is_near_zero (diff, cscale))
	{
	  if (cscale < rscale+1)
	    cscale = MIN (cscale*3, rscale+1);
	  else
	    done = TRUE;
	}
    }

  /* Assign the number and clean up. */
  bc_free_num (num);
  bc_divide (guess,BCG(_one_),num,rscale TSRMLS_CC);
  return 1;
}

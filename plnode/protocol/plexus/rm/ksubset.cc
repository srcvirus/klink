/* subset.c
 *
 * By Sebastian Raaphorst, 2002
 * ID#: 1256241
 *
 * $Author: vorpal $
 * $Date: 2002/11/14 19:11:00 $
 */

#include <cstdlib>
#include "common.h"
#include "combination.h"
#include "ksubset.h"

using namespace std;

Set ksubset_init(int n)
{
  Set s;

  if (!(s = (Set) malloc(sizeof(struct _set))))
    return FALSE;

  s->n = n;
  if (!(s->nCr = combination_init(n))) {
    free(s);
    return FALSE;
  }

  return s;
}


void ksubset_free(Set s)
{
  combination_free(s->n, s->nCr);
  free(s);
}


int ksubset_lex_succ(Set s, int k, int *orig, int *succ)
{
  int i, j;

  for (i=0; i < k; ++i)
    succ[i] = orig[i];

  for (i=k-1; i >= 0 && orig[i] == s->n - k + i + 1; --i);

  if (i < 0)
    return FALSE;

  for (j=i; j < k; ++j)
    succ[j] = orig[i] + 1 + j - i;

  return TRUE;
}


long ksubset_lex_rank(Set s, int k, int *subset)
{
  int i, j;
  long r;

  for (i=0, r=0; i < k; ++i)
    if ((i ? subset[i-1] : 0) + 1 <= subset[i] - 1)
      for (j=(i ? subset[i-1] : 0) + 1; j < subset[i]; ++j)
	r += s->nCr[s->n - j][k - i - 1];

  return r;
}


void ksubset_lex_unrank(Set s, int k, long r, int *subset)
{
  int i;
  int x;

  for (x=1, i=0; i < k; ++i) {
    while (s->nCr[s->n - x][k - i - 1] <= r) {
      r -= s->nCr[s->n - x][k - i - 1];
      ++x;
    }

    subset[i] = x;
    ++x;
  }
}


/*
 * $Log: ksubset.c,v $
 * Revision 1.1  2002/11/14 19:11:00  vorpal
 * Initial checkin.
 *
 */

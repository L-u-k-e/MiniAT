#ifndef _MINIAT_PRIV_UTIL_H_
#define _MINIAT_PRIV_UTIL_H_

#include <stdbool.h>

#include "miniat/miniat.h"
#include "miniat_priv_typedefs.h"

/**
 * Returns a pointer to a newly allocated copy of s.
 * @param m
 *  A miniat
 * @param s
 *  The string to dup
 * @return
 *  NULL on error, or a pointer to the string on success
 */
extern char *m_util_str_duplicate(miniat *m, char *s);

#endif 

/*
 *  prothlp.c -- Some tools for protocol.c
 *
 *  prothlp.c is a part of binkd project
 *
 *  Copyright (C) 1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: prothlp.c,v 2.10.2.1 2014/08/09 15:17:43 gul Exp $
 *
 * $Log: prothlp.c,v $
 * Revision 2.10.2.1  2014/08/09 15:17:43  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.10  2008/04/17 15:19:44  gul
 * Fixed sending files with space or control chars in name
 *
 * Revision 2.9  2006/01/23 07:03:45  stas
 * Fix (null) suffix after ASO->BSO bundle name conversion
 *
 * Revision 2.8  2005/10/10 16:24:22  stas
 * Change method for generate 8+3 bundle name from ASO bundle name
 *
 * Revision 2.7  2005/09/22 12:11:13  gul
 * Dequote filenames for compare in M_GET processing
 *
 * Revision 2.6  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.5  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.4  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.3  2003/06/30 22:42:27  hbrew
 * Print only binkd name (without path) in error messages
 *
 * Revision 2.2  2003/03/01 18:29:52  gul
 * Change size_t to off_t for file sizes and offsets
 *
 * Revision 2.1  2003/02/22 21:32:46  gul
 * Amiga Style Outbound support
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.3  1997/11/05  01:11:29  mff
 * bt+ arcshield
 *
 * Revision 1.2  1997/10/23  04:00:17  mff
 * minor changes
 *
 * Revision 1.1  1997/03/28  06:49:13  mff
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sys.h"
#include "prothlp.h"
#include "tools.h"
#include "crypt.h"

int tfile_cmp (TFILE *a, char *netname, boff_t size, time_t time)
{
  int rc;
  char *anetname;

  anetname = strdequote(a->netname);
  netname = strdequote(netname);
  rc = strcmp (anetname, netname);
  free(anetname);
  free(netname);
  if (rc) return rc;
  if (a->size != size)
    return a->size > size ? 1 : -1;
  if (a->time != time)
    return a->time > time ? 1 : -1;
  return 0;
}

/* Adds a file to killlist */
void add_to_killlist (KILLLIST **killlist, int *n_killlist, char *name, int cond)
{
  *killlist = xrealloc (*killlist, sizeof (KILLLIST) * (*n_killlist + 1));
  memset (*killlist + *n_killlist, 0, sizeof (KILLLIST));
  strnzcpy ((*killlist)[*n_killlist].name, name, MAXPATHLEN);
  (*killlist)[*n_killlist].cond = cond;
  ++*n_killlist;
}

void q_to_killlist (KILLLIST **killlist, int *n_killlist, FTNQ *q)
{
  FTNQ *curr;

  for (curr = q; curr; curr = curr->next)
  {
    if (curr->action == 'a')
    {
      curr->action = 0;
      add_to_killlist (killlist, n_killlist, curr->path, 0);
    }
  }
}

void free_killlist (KILLLIST **killlist, int *n_killlist)
{
  free (*killlist);
  *killlist = 0;
  *n_killlist = 0;
}

/*
 * Process killlist. flag == 's': successful session
 */
void process_killlist (KILLLIST *killlist, int n_killlist, int flag)
{
  int i;

  Log (6, "processing kill list");
  for (i = 0; i < n_killlist; ++i)
    if (killlist[i].cond != 's' || (flag == 's' && killlist[i].cond == 's'))
      delete (killlist[i].name);
}

/* Adds a file to rcvdlist */
void add_to_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist, char *name)
{
  *rcvdlist = xrealloc (*rcvdlist, sizeof (RCVDLIST) * (*n_rcvdlist + 1));
  memset (*rcvdlist + *n_rcvdlist, 0, sizeof (RCVDLIST));
  strnzcpy ((*rcvdlist)[*n_rcvdlist].name, name, MAXPATHLEN);
  ++*n_rcvdlist;
}

void free_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist)
{
  free (*rcvdlist);
  *rcvdlist = 0;
  *n_rcvdlist = 0;
}

void netname_ (char *s, TFILE *q
#ifdef AMIGADOS_4D_OUTBOUND
		, int aso
#endif
		)
{
  static char *weekext[] = {"su", "mo", "tu", "we", "th", "fr", "sa"};

  if (q->type == 'm')
  {
    sprintf (s, "%08lx.pkt", rnd ());
  }
  else if (q->type == 'r')
  {
    sprintf (s, "%08lx.req", rnd ());
  }
  else
  {
    char *z = extract_filename(q->path);

    strnzcpy (s, z, MAX_NETNAME);

    /* gul: bt+ arcshield */
    if ((z = strrchr (s, '.')) != NULL)
      if (strlen (++z) == 3)
      {
	if ((*z >= '0') && (*z < '7') && (z[1] == '#'))
	  memcpy (z, weekext[*z - '0'], 2);
#ifdef AMIGADOS_4D_OUTBOUND
	if (aso && isarcmail(s))
	{ /* "2:2/0 aso name shorter then bso */
	  char ext[4];
	  unsigned long zone, net, node, p;
	  struct stat sb;

	  memset(&sb,0,sizeof(sb));
	  stat(q->path, &sb);
	  if (sscanf(s, "%lu.%lu.%lu.%lu.%3s", &zone, &net, &node, &p, ext) == 5) {
	    unsigned long C = CRC32((int)sb.st_size,CRC32(p,CRC32(node,CRC32(net,CRC32(zone,0)))));
	    sprintf(s, "%08lx.%s",  /* Convert long name to CRC32 from zone,net,node,point numbers */
	            C, ext);
	  }
	}
#endif
      }

    z = strquote(s, SQ_CNTRL | SQ_SPACE);
    strnzcpy (s, z, MAX_NETNAME);
    free (z);
  }
}

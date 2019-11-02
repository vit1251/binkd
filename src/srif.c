/*
 *  srif.c -- Create flags or run external programs on mail events
 *
 *  srif.c is a part of binkd project
 *
 *  Copyright (C) 1996-2002  Dima Maloff, 5047/13 and others
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: srif.c,v 2.21.2.1 2014/08/09 15:17:44 gul Exp $
 *
 * $Log: srif.c,v $
 * Revision 2.21.2.1  2014/08/09 15:17:44  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.21  2012/01/08 17:34:58  green
 * Avoid using MAXHOSTNAMELEN
 *
 * Revision 2.20  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.19  2008/08/05 06:05:17  gul
 * Optimize srif functions params
 *
 * Revision 2.18  2005/07/04 18:24:43  gul
 * Move events checking and running to inb_test() for reducing repeated code;
 * do not run immediate events twice;
 * fixed argus-style freqs (not tested).
 *
 * Revision 2.17  2004/06/07 10:47:12  gul
 * Touch flag if its already exists
 *
 * Revision 2.16  2004/02/29 08:51:13  gul
 * Fixed *A@ macro on event calls
 * (patch from Victor Levenets).
 *
 * Revision 2.15  2003/10/29 21:08:40  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.14  2003/09/08 17:05:42  stream
 * Big memory leak in evt_queue()
 *
 * Revision 2.13  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.12  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.11  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.10  2003/08/12 09:23:00  val
 * migrate from pmatch() to pmatch_ncase()
 *
 * Revision 2.9  2003/05/28 14:32:57  gul
 * new function q_add_last_file() - add file to the end of queue
 *
 * Revision 2.8  2003/05/27 16:11:17  gul
 * Improve logging
 *
 * Revision 2.7  2003/03/10 17:32:38  gul
 * Use socklen_t
 *
 * Revision 2.6  2003/03/05 13:21:51  gul
 * Fix warnings
 *
 * Revision 2.5  2003/02/23 12:49:00  gul
 * Added *A* and *A@ macros
 *
 * Revision 2.4  2002/03/14 12:26:24  gul
 * Creating flags bugfix
 *
 * Revision 2.3  2002/02/22 00:18:34  gul
 * Run by-file events with the same command-line once after session
 *
 * Revision 2.2  2001/10/27 08:07:19  gul
 * run and run_args returns exit code of calling process
 *
 * Revision 2.1  2001/10/27 07:53:46  gul
 * Unlink req-file after run freq-processor
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.4  1997/06/16  05:40:33  mff
 * Binkd will not complain about missing .rsp files when running
 * programms with "exec" statement without using of *S macro.
 *
 * Revision 1.3  1997/03/28  06:12:48  mff
 * Changes to support SRIF: + evt_run(), etc.
 *
 * Revision 1.2  1997/02/09  04:17:30  mff
 * Removed error msgs
 *
 * Revision 1.1  1997/01/08  03:57:54  mff
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sys.h"
#include "readcfg.h"
#include "protoco2.h"
#include "srif.h"
#include "ftnaddr.h"
#include "tools.h"
#include "run.h"
#include "ftnq.h"
#include "prothlp.h"
#include "protocol.h"
#include "rfc2553.h"

static EVTQ *evt_queue(EVTQ *eq, char evt_type, char *path)
{
  EVTQ *tmp;

  for (tmp=eq; tmp; tmp=tmp->next)
  { if (tmp->evt_type == evt_type && strcmp(tmp->path, path) == 0)
      return eq; /* already exists */
  }
  tmp = xalloc(sizeof(EVTQ));
  tmp->evt_type = evt_type;
  tmp->path = xstrdup(path);
  tmp->next = eq;
  return tmp;
}

/*
 * Tests if filename matches any of EVT_FLAG's patterns.
 */
int evt_test (EVTQ **eq, char *filename, EVT_FLAG *evt_flags)
{
  EVT_FLAG *curr;
  int rc=0;

  for (curr = evt_flags; curr; curr = curr->next)
  {
    if (pmatch_ncase(curr->pattern, filename))
    {
      if (curr->path)
      {
	Log (4, "got %s, %screating %s", curr->pattern, curr->imm ? "" : "delayed ", curr->path);
	if (curr->imm)
	{
	  if (create_empty_sem_file (curr->path) == 0)
	    touch (curr->path, time(NULL));
	}
	else
	  *eq = evt_queue(*eq, 'f', curr->path);
      }
      else if (curr->command)
      {
	Log (4, "got %s, %sstarting %s", curr->pattern, curr->imm ? "" : "delayed ", curr->command);
	rc=1;
      }
    }
  }
  return rc;
}

/*
 * Sets flags for all matched with evt_test events
 */
void evt_set (EVTQ *eq)
{
  EVTQ *curr;

  while (eq)
  {
    if (eq->evt_type == 'e')
    {
      Log (4, "Running %s", eq->path);
      run(eq->path);
    }
    else
    {
      Log (4, "Creating %s", eq->path);
      if (create_empty_sem_file(eq->path) == 0)
	touch(eq->path, time(NULL));
    }
    curr = eq->next;
    free(eq->path);
    free(eq);
    eq = curr;
  }
}

/* Makes SRIF request and SRIF response names from FTN address of a node */
static int mksrifpaths (FTN_ADDR *fa, char *srf, char *rsp, BINKD_CONFIG *config)
{
  ftnaddress_to_filename (srf, fa, config);
  if (*srf)
  {
    strnzcpy (rsp, srf, MAXPATHLEN);
    strnzcat (srf, ".srf", MAXPATHLEN);
    strnzcat (rsp, ".rsp", MAXPATHLEN);
    return 1;
  }
  else
    return 0;
}

static int srif_fill (char *path, FTN_ADDR *fa, int nfa,
		       char *req, char *rsp, int prot, int listed,
		       char *peer_name, char *peer_sysop)
{
  FILE *out;
  int i;

  if ((out = fopen (path, "w")) != 0)
  {
    fprintf (out, "Sysop %s\n", peer_sysop);
    for (i = 0; i < nfa; ++i)
    {
      char adr[FTN_ADDR_SZ + 1];

      ftnaddress_to_str (adr, fa + i);
      fprintf (out, "AKA %s\n", adr);
    }
    fprintf (out, "Baud 115200\n");
    fprintf (out, "Time -1\n");
    fprintf (out, "RequestList %s\n", req);
    fprintf (out, "ResponseList %s\n", rsp);
    fprintf (out, "RemoteStatus %s\n", prot ? "PROTECTED" : "UNPROTECTED");
    fprintf (out, "SystemStatus %s\n", listed ? "LISTED" : "UNLISTED");
    fprintf (out, "CallerID %s\n", peer_name);
    fprintf (out, "SessionType OTHER\n");
    fclose (out);
    return 1;
  }
  return 0;
}

static FTNQ *parse_response (FTNQ *q, char *rsp, FTN_ADDR *fa, BINKD_CONFIG *config)
{
  FILE *in = fopen (rsp, "r");

  if (in)
  {
    char buf[MAXPATHLEN + 1];
    int i;
    int haveSomething = 0;

    while (!feof (in))
    {
      if (!fgets (buf, MAXPATHLEN, in))
        break;
      haveSomething = 1;
      for (i = 0; i < sizeof (buf) - 1 && !isspace (buf[i]); ++i);
      buf[i] = 0;
      Log (4, "parse_response: add file `%s' to queue", buf + 1);
      switch (*buf)
	{
	  case '=':
	    q = q_add_last_file (q, buf + 1, fa, 'h', 'd', 0, config);
	    break;
	  case '+':
	    q = q_add_last_file (q, buf + 1, fa, 'h', 0, 0, config);
	    break;
	  case '-':
	    q = q_add_last_file (q, buf + 1, fa, 'h', 'a', 0, config);
	    break;
	  default:
	    Log (2, "parse_response: unknown predictor `%c', ignored response file `%s'", *buf, buf + 1);
	    break;
	}
    }
    if (!haveSomething)
        Log (3, "SRIF response file is empty");
    fclose (in);
  }
  else
    Log (1, "parse_response: %s: %s", rsp, strerror (errno));
  return q;
}

static char valid_filename_chars[]=".\\-_:/@";
static EVTQ *run_args(EVTQ *eq, char *cmd, char *filename0, STATE *st, int imm)
{
  char *sp, *w;
  char *fn=filename0;
  char *pn=st->peer_name ? st->peer_name : "";
  char ipaddr[BINKD_FQDNLEN + 1];
  char aka[4];
  char adr[FTN_ADDR_SZ + 2];
  int i;
  unsigned int l;
  size_t sw=1024;
  int use_fn=0;

  for (i=0; fn[i]; i++)
  if ((!isalnum(fn[i])) && (!strchr(valid_filename_chars, fn[i])))
  {
    fn="-";
    Log(4, "Invalid filename (%s) received!", filename0);
    break;
  }
  for (i=0; pn[i]; i++)
  if ((!isalnum(pn[i])) && (!strchr(valid_filename_chars, pn[i])))
  {
    i=0;
    break;
  }
  if (!i)
  {
    struct sockaddr_storage sin;
    socklen_t si = sizeof (sin);
    if ((!st) || (getpeername (st->s, (struct sockaddr *)&sin, &si) == -1))
      strcpy(ipaddr, "-");
    else
    {
      if (getnameinfo((struct sockaddr *)&sin, si, ipaddr, sizeof(ipaddr), 
                NULL, 0, NI_NUMERICHOST) != 0)
        strcpy(ipaddr, "-");
    }
    pn=ipaddr;
  }

  if (sw<=strlen(cmd)) sw=strlen(cmd)+1;
  w=(char*)xalloc(sw);
  strcpy(w, cmd);

  strcpy(aka, "*A0");

  for (l=0; (l<strlen(w)) && ((sp=strchr(w+l, '*'))!=NULL); l++)
  {
    l=sp-w;
    switch(toupper(sp[1]))
    {
      case 'N': /* short filename for win32 */
#ifdef WIN32
        {
          char ts[MAXPATHLEN+1];
          i=GetShortPathName(filename0, ts, sizeof(ts));
          if ((i<1) || (i>MAXPATHLEN))
          {
            Log(2, "GetShortPathName() fails %d", GetLastError());
            use_fn=1;
            strcpy(ts, fn);
          }
          w = ed (w, "*N", ts, &sw);
          break;
        }
#endif
        use_fn=1;
        w = ed (w, "*N", fn, &sw);
        break;
      case 'F': /* filename */
        use_fn=1;
        w = ed (w, "*F", fn, &sw);
        break;
      case 'A': /* AKA */
	if (sp[2] == '@' || sp[2] == '*')
	{
	  char *pa, *addrlist;
	  pa = addrlist = malloc(st->nfa*(FTN_ADDR_SZ+1l));
	  if (addrlist == NULL)
	  { Log(1, "Not enough memory, %ld bytes needed", st->nfa*(FTN_ADDR_SZ+1l));
	    break;
	  }
	  for (i=0; i<st->nfa; i++)
	  { if (pa != addrlist) *pa++=' ';
	    ftnaddress_to_str (pa, st->fa+i);
	    pa += strlen(pa);
	  }
	  aka[2]=sp[2];
	  w = ed (w, aka, addrlist, &sw);
	  free(addrlist);
	  break;
	}
        i=isdigit(sp[2])?(sp[2]-'0'):0;
        if (i<st->nfa)
          ftnaddress_to_str (adr, st->fa+i);
        else
          strcpy(adr, "-");
        aka[2]=sp[2];
        if (!isdigit(sp[2]))  /* If you will remove it, *A *A1 may not work */
        {
          i=strlen(adr);
          adr[i++]=sp[2];
          adr[i]=0;
        }
        w = ed (w, aka, adr, &sw);
        break;
      case 'P': /* protected ? */
        w = ed (w, "*P", st->state == P_SECURE ? "1" : "0", &sw);
        break;
      case 'L': /* listed ? */
        w = ed (w, "*L", st->listed_flag ? "1" : "0", &sw);
        break;
      case 'H': /* hostname or IP */
        w = ed (w, "*H", pn, &sw);
        break;
    }
  }

  if ((fn!=filename0) && (use_fn))
    Log (1, "Security problem. Execution aborted...");
  else if (imm) /* immediate event */
    run(w);
  else
    eq = evt_queue(eq, 'e', w);
  free(w);
  return eq;
}

/*
 * Runs external programs using S.R.I.F. interface
 * if the name matches one of our "exec"'s
 */
FTNQ *evt_run (FTNQ *q, char *filename, int imm_freq,
               STATE *st, BINKD_CONFIG *config)
{
  EVT_FLAG *curr;

  for (curr = config->evt_flags.first; curr; curr = curr->next)
  {
    if (curr->command && pmatch_ncase(curr->pattern, filename))
    {
      if (strstr (curr->command, "*S") || strstr (curr->command, "*s"))
      {
	if (imm_freq)
	{
	  char srf[MAXPATHLEN + 1], rsp[MAXPATHLEN + 1];
	  if (mksrifpaths (st->fa, srf, rsp, config))
	  {
	    char *w = ed (curr->command, "*S", srf, NULL);

	    if (srif_fill (srf, st->fa, st->nfa, filename, rsp,
	                   st->state == P_SECURE, st->listed_flag,
	                   st->peer_name, st->sysop))
	    {
	      if (!run_args (NULL, w, filename, st, 1))
	        delete(filename);
	    }
	    else
	      Log (1, "srif_fill: error");
	    free (w);
	    q = parse_response (q, rsp, st->fa, config);
	    delete (srf);
	    delete (rsp);
	  }
	  else
	    Log (1, "mksrifpaths: error");
	} else
	  add_to_rcvdlist (&st->rcvdlist, &st->n_rcvdlist, filename);
      }
      else
	st->evt_queue = run_args(st->evt_queue, curr->command, filename, st, curr->imm);
    }
  }
  return q;
}

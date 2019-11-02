/*
 *  breaksig.c -- SIGBREAK, etc. signal handlers
 *
 *  breaksig.c is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: breaksig.c,v 2.5 2003/10/29 21:08:38 gul Exp $
 *
 * $Log: breaksig.c,v $
 * Revision 2.5  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.4  2003/08/26 21:01:09  gul
 * Fix compilation under unix
 *
 * Revision 2.3  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.1  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 1.3  1997/10/23  04:18:08  mff
 * exitfunc() moved to exitproc.c, minor changes
 *
 * Revision 1.2  1997/03/09  07:17:16  mff
 * Support for pid_file
 *
 * Revision 1.1  1996/12/14  07:02:20  mff
 * Initial revision
 *
 */

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "sys.h"
#include "common.h"
#include "tools.h"

static void exitsig (int arg)
{
  /* Log (0, ...) will call exit(), exit() will call exitlist */
#ifdef HAVE_FORK
  if (pidcmgr)
    Log (0, "got signal #%i. Killing %i and quitting...", arg, (int) pidcmgr);
  else
#endif
    Log (0, "got signal #%i.", arg);
}

/* Set up break handler, set up exit list if needed */
int set_break_handlers (void)
{
  atexit (exitfunc);

#ifdef SIGBREAK
  signal (SIGBREAK, exitsig);
#endif
#ifdef SIGHUP
  signal (SIGHUP, SIG_IGN);
#endif
#ifdef SIGINT
  signal (SIGINT, exitsig);
#endif
#ifdef SIGTERM
  signal (SIGTERM, exitsig);
#endif
  return 1;
}

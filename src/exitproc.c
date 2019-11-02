/*
 *  exitproc.c -- Actions to perform on exit()
 *
 *  exitproc.c is a part of binkd project
 *
 *  Copyright (C) 1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: exitproc.c,v 2.44.2.1 2014/08/09 15:17:42 gul Exp $
 *
 * $Log: exitproc.c,v $
 * Revision 2.44.2.1  2014/08/09 15:17:42  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.44  2012/01/03 17:25:31  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.43  2010/05/24 14:24:32  gul
 * Exit immediately after all jobs done in "-p" mode
 *
 * Revision 2.42  2009/06/02 17:09:35  gul
 * Build binkd for OS/2 with perl support
 *
 * Revision 2.41  2009/05/31 07:16:16  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.40  2004/10/18 15:22:19  gul
 * Change handle perl errors method
 *
 * Revision 2.39  2004/02/07 14:06:06  hbrew
 * Macros: RTLDLL-->RTLSTATIC, BINKDW9X-->BINKD9X
 *
 * Revision 2.38  2004/01/03 19:04:20  stas
 * New functions: public w32Init() and hidden w32exitfunc()
 *
 * Revision 2.37  2004/01/03 12:17:43  stas
 * Implement full icon support (winNT/2k/XP)
 *
 * Revision 2.36  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.35  2003/10/28 13:09:53  gul
 * Fix NT service semaphore usage in exitfunc()
 *
 * Revision 2.34  2003/10/18 06:45:23  stas
 * Fix a semaphore usage in exitfunc()
 *
 * Revision 2.33  2003/10/17 18:49:36  stas
 * Use a semaphore to prevent double run exitfunc()
 *
 * Revision 2.32  2003/10/13 08:48:09  stas
 * Implement true NT service stop sequence
 *
 * Revision 2.31  2003/10/08 10:30:34  gul
 * Fixed debug logging (thx to Alexander Reznikov)
 *
 * Revision 2.30  2003/10/07 20:50:07  gul
 * Wait for servmanager exit from exitproc()
 * (Patch from Alexander Reznikov)
 *
 * Revision 2.29  2003/10/07 17:57:09  gul
 * Some small changes in close threads function.
 * Inhibit errors "socket operation on non-socket" on break.
 *
 * Revision 2.28  2003/09/11 13:04:13  hbrew
 * Undo 'move binkd9x deinit to exitfunc()' patch
 *
 * Revision 2.27  2003/09/09 17:57:43  stream
 * Do not unload config on exit (considered useless and potentially unstable)
 *
 * Revision 2.26  2003/09/08 08:21:20  stream
 * Cleanup config semaphore, free memory of base config on exit.
 *
 * Revision 2.25  2003/09/08 06:36:51  val
 * (a) don't call exitfunc for perlhook fork'ed process
 * (b) many compilation warnings in perlhooks.c fixed
 *
 * Revision 2.24  2003/09/07 04:49:41  hbrew
 * Remove binkd9x restart-on-config-change code; move binkd9x deinit to exitfunc()
 *
 * Revision 2.23  2003/08/26 22:18:47  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.22  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.21  2003/08/17 10:38:55  gul
 * Return semaphoring for log and binlog
 *
 * Revision 2.20  2003/08/16 09:08:33  gul
 * Binlog semaphoring removed
 *
 * Revision 2.19  2003/08/16 06:21:12  gul
 * Log() semaphoring removed
 *
 * Revision 2.18  2003/08/14 12:56:29  gul
 * Make Log() thread-safe
 *
 * Revision 2.17  2003/08/14 11:43:19  val
 * free allocated log buffer in exitfunc()
 *
 * Revision 2.16  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.15  2003/06/04 10:36:58  stas
 * Thread-safety tcperr() implementation on Win32
 *
 * Revision 2.14  2003/06/02 08:26:00  gul
 * Fix hang on exit with big loglevel
 *
 * Revision 2.13  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.12  2003/03/31 19:53:08  gul
 * Close socket before exit
 *
 * Revision 2.11  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.10  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.9  2003/03/11 09:21:30  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.8  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.7  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.6  2003/03/09 18:19:32  gul
 * Bugfix
 *
 * Revision 2.5  2003/03/06 18:30:28  gul
 * A bit optimize
 *
 * Revision 2.4  2003/03/05 11:43:56  gul
 * Fix win32 compilation
 *
 * Revision 2.3  2003/03/05 11:40:12  gul
 * Fix win32 compilation
 *
 * Revision 2.2  2003/03/05 09:00:45  gul
 * Fix win32 compilation
 *
 * Revision 2.1  2003/03/03 23:41:20  gul
 * Try to resolve problem with active threads while exitproc running
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 1.2  1997/10/23  04:13:35  mff
 * pidfiles are now killed only by servmgrs, misc
 *
 * Revision 1.1  1997/08/12  21:42:54  mff
 * Initial revision
 */

#include <signal.h>

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "ftnnode.h"
#include "bsy.h"
#include "tools.h"
#include "sem.h"
#include "server.h"
#ifdef WITH_PERL
#include "perlhooks.h"
#endif
#ifdef BINKD9X
#include "nt/win9x.h"
#endif
#if defined(WIN32) && !defined(BINKD9X)
#include "nt/service.h"
#include "nt/w32tools.h"
#endif

int binkd_exit;

#ifdef HAVE_THREADS

static fd_set sockets;
static SOCKET max_socket;

int add_socket(SOCKET sockfd)
{
  threadsafe(
    FD_SET (sockfd, &sockets);
    if (sockfd >= max_socket)
      max_socket = sockfd + 1;
  );
  return 0;
}

int del_socket(SOCKET sockfd)
{
  threadsafe(FD_CLR (sockfd, &sockets));
  return 0;
}

#endif

void close_srvmgr_socket(void)
{
  int curfd;

  for (curfd=0; curfd<sockfd_used; curfd++)
  { Log (5, "Closing server socket # %i", sockfd[curfd]);
    soclose (sockfd[curfd]);
  }
  sockfd_used = 0;
}

void exitfunc (void)
{
  BINKD_CONFIG *config;
#if defined(WIN32) && !defined(BINKD9X)
  static int exitfunc_called_flag=0;

  if (IsNT() && isService()) {
    LockSem(&exitsem);
    if(exitfunc_called_flag)
    { /* prevent double call exitfunc() at NT service stop sequence */
      ReleaseSem(&exitsem);
      Log(10, "exitfunc() repeated call, return from exitfunc()");
      return;
    }
    exitfunc_called_flag=1;
    ReleaseSem(&exitsem);
  }
#endif

  Log(7, "exitfunc()");

#ifdef HAVE_FORK
  if (pidcmgr)
  { int i;
    i=pidcmgr, pidcmgr=0; /* prevent abort when cmgr exits */
    kill (i, SIGTERM);
    /* sleep (1); */
  }
  close_srvmgr_socket();
#elif defined(HAVE_THREADS)
  /* exit all threads */
  { SOCKET h;
    int timeout = 0;
    /* wait for threads exit */
    binkd_exit = 1;
    for (;;)
#ifdef OS2
      if (n_servers || n_clients || (pidcmgr && server_flag))
#else
      if (n_servers || n_clients || pidcmgr || pidsmgr)
#endif
      {
	close_srvmgr_socket();
	if (pidcmgr)
	  PostSem(&wakecmgr);
	/* close active sockets */
	for (h=0; h < max_socket; h++)
	  if (FD_ISSET(h, &sockets))
	    soclose (h);

	if (WaitSem (&eothread, 1))
	{
	  timeout++;
	  if (timeout == 4) /* 4 sec */
	  {
	    Log(5, "exitfunc(): warning, threads exit timeout (%i sec)!", timeout);
	    break;
	  }
	}
	else
	  timeout = 0;
      }
      else
      {
	Log(8, "exitfunc(): all threads finished");
	break;
      }
  }
#endif

  config = lock_current_config();
  if (config)
    bsy_remove_all (config);
  sock_deinit ();
  nodes_deinit ();
  if (config)
  {
    if (*config->pid_file && pidsmgr == (int) getpid ())
      delete (config->pid_file);
	/* completely unload config */
#ifdef HAVE_FORK
    unlock_config_structure(config, inetd_flag || (!pidsmgr && pidCmgr == (int) getpid()) || (pidsmgr == (int) getpid()));
#else
    unlock_config_structure(config, 1);
#endif
  }
  CleanSem (&config_sem);
  CleanSem (&hostsem);
  CleanSem (&resolvsem);
  CleanSem (&lsem);
  CleanSem (&blsem);
  CleanSem (&varsem);
  CleanEventSem (&eothread);
  CleanEventSem (&wakecmgr);
#ifdef OS2
  CleanSem (&fhsem);
#endif
#if defined(WITH_PERL) && defined(HAVE_THREADS) && defined(PERL_MULTITHREAD)
  CleanSem (&perlsem);
#endif
  ReleaseErrorList();
}

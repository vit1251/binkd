/*
 *  binkd.c -- binkd's main
 *
 *  binkd.c is a part of binkd project
 *
 *  Copyright (C) 1996-2009  Dima Maloff 5047/13 and others
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: binkd.c,v 2.112.2.2 2014/08/20 06:12:37 gul Exp $
 *
 * $Log: binkd.c,v $
 * Revision 2.112.2.2  2014/08/20 06:12:37  gul
 * Fixed 100% cpu load if called with poll flag,
 * backport many fixes related to compilation on win32 and os/2.
 *
 * Revision 2.112.2.1  2014/08/09 15:17:41  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.112  2012/05/14 06:14:57  gul
 * More safe signal handling
 *
 * Revision 2.111  2012/01/21 18:49:22  green
 * Make environment usage ISO C compliant (no 3-args main())
 *
 * Revision 2.110  2012/01/08 16:23:52  green
 * Fixed compilation in Cygwin/MinGW
 *
 * Revision 2.109  2012/01/07 13:16:17  green
 * Some more information in binkd -vv output
 *
 * Revision 2.108  2012/01/03 16:53:22  green
 * Correct PID for servmgr in logs (i.e. PID after fork, not before)
 *
 * Revision 2.107  2010/05/24 14:38:03  gul
 * Allow daemonize on client-only mode
 *
 * Revision 2.106  2010/05/24 14:24:32  gul
 * Exit immediately after all jobs done in "-p" mode
 *
 * Revision 2.105  2010/03/30 06:13:01  gul
 * Do not chdir to "/" on daemonize for use relative pathes on reload config
 *
 * Revision 2.104  2009/06/16 19:24:29  gul
 * Cosmetics around mkargv()
 *
 * Revision 2.103  2009/06/15 19:56:38  stas
 * Fix OS/2 Watcom build crash. Thanks to Alexey Korop 2:461/155
 *
 * Revision 2.102  2009/06/02 17:09:34  gul
 * Build binkd for OS/2 with perl support
 *
 * Revision 2.101  2009/05/31 07:16:16  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.100  2009/05/26 13:04:35  gul
 * New perl hooks:
 * need_reload() - is it needed to reload config
 * config_loaded() - after successful reading config
 *
 * Revision 2.99  2009/01/31 16:57:59  gul
 * Update year in copyright line
 *
 * Revision 2.98  2007/10/06 09:35:12  gul
 * Retranslate SIGHUP from servermgr to clientmgr
 *
 * Revision 2.97  2007/10/04 17:30:28  gul
 * SIGHUP handler (thx to Sergey Babitch)
 *
 * Revision 2.96  2006/07/24 21:00:32  gul
 * use MSG_NOSIGNAL in send()
 *
 * Revision 2.95  2005/11/03 13:15:46  stas
 * Option '-n' recognization
 *
 * Revision 2.94  2005/11/03 11:42:35  stas
 * New option '-n', may be used to config check with option '-d' or to make poll with '-P'
 *
 * Revision 2.93  2005/10/10 16:31:43  stas
 * Fix compiler warning 'string length is greater than the length 509  ISO C89 compilers are required to support'
 *
 * Revision 2.92  2004/11/12 14:40:06  gul
 * Also one syntax error
 *
 * Revision 2.91  2004/11/09 07:04:22  gul
 * Syntax error in previous patch
 *
 * Revision 2.90  2004/11/07 07:26:35  gul
 * New config options zlib-dll and bzlib2-dll
 *
 * Revision 2.89  2004/08/04 19:51:40  gul
 * Change SIGCHLD handling, make signal handler more clean,
 * prevent occasional hanging (mutex deadlock) under linux kernel 2.6.
 *
 * Revision 2.88  2004/02/07 14:06:06  hbrew
 * Macros: RTLDLL-->RTLSTATIC, BINKDW9X-->BINKD9X
 *
 * Revision 2.87  2004/01/08 12:48:16  val
 * add missing 'break' for -d option case
 *
 * Revision 2.86  2004/01/07 21:40:46  stas
 * Options -i and -u is marked deprecated for win32
 *
 * Revision 2.85  2004/01/07 13:12:29  gul
 * Update 2003->2004 in copyright notices
 *
 * Revision 2.84  2004/01/07 10:15:51  gul
 * Remove redundant preprocessor directive
 *
 * Revision 2.83  2004/01/07 10:13:26  gul
 * Fix usage output
 *
 * Revision 2.82  2004/01/03 19:33:42  stas
 * Implement install and uninstall win* services using -t option
 *
 * Revision 2.81  2004/01/03 19:04:20  stas
 * New functions: public w32Init() and hidden w32exitfunc()
 *
 * Revision 2.79  2004/01/03 15:38:50  stas
 * Implement service control option for Windows NT/2k/XP
 *
 * Revision 2.78  2004/01/03 12:38:26  stas
 * Small change (win32)
 *
 * Revision 2.77  2004/01/03 12:17:43  stas
 * Implement full icon support (winNT/2k/XP)
 *
 * Revision 2.76  2004/01/03 11:16:57  stas
 * Remove the obsoleted rerun variable
 *
 * Revision 2.75  2003/12/26 20:11:32  gul
 * Add -d commandline switch - dump parsed config and exit;
 * remove 'debugcfg' config token.
 *
 * Revision 2.74  2003/11/09 03:45:45  hbrew
 * Add -vv to help message
 *
 * Revision 2.73  2003/11/04 00:46:20  hbrew
 * confopt added.
 *
 * Revision 2.72  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.71  2003/10/20 20:27:06  gul
 * format output
 *
 * Revision 2.70  2003/10/20 19:17:52  gul
 * Print optional compiled-in extensions by -vv command-line switch
 *
 * Revision 2.69  2003/10/19 12:21:46  gul
 * Stream compression
 *
 * Revision 2.68  2003/10/18 18:50:47  stas
 * Move to new 'tray.c' file several functions when is related with 'minimize to tray' feature
 *
 * Revision 2.67  2003/10/18 15:59:07  stas
 * Improve logging (report about loading dlls)
 *
 * Revision 2.66  2003/10/05 09:37:42  stas
 * Optimize binkd/nt start: use hack to determine if we're running as a service without waiting for the service control manager to fail
 *
 * Revision 2.65  2003/09/24 07:32:16  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 * Revision 2.64  2003/09/21 17:51:08  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.63  2003/09/12 07:37:57  val
 * compression support via zlib (preliminary)
 *
 * Revision 2.62  2003/09/05 06:49:06  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.61  2003/08/28 01:28:52  hbrew
 * Fix typo
 *
 * Revision 2.60  2003/08/26 22:18:47  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.59  2003/08/26 21:01:09  gul
 * Fix compilation under unix
 *
 * Revision 2.58  2003/08/26 18:18:51  gul
 * Process "-?" commandline switch as "-h"
 *
 * Revision 2.57  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.56  2003/08/26 14:36:47  gul
 * Perl hooks in os2-emx
 *
 * Revision 2.55  2003/08/24 16:55:07  hbrew
 * Fix memory allocation for polls
 *
 * Revision 2.54  2003/08/21 15:40:34  gul
 * Change building commandline for service under win32
 * (patch by Alexander Reznikov)
 *
 * Revision 2.53  2003/08/17 10:38:55  gul
 * Return semaphoring for log and binlog
 *
 * Revision 2.52  2003/08/16 09:08:32  gul
 * Binlog semaphoring removed
 *
 * Revision 2.51  2003/08/11 08:33:16  val
 * better error handling in perl hooks
 *
 * Revision 2.50  2003/08/04 12:17:49  gul
 * Remove extra error message
 *
 * Revision 2.49  2003/07/19 06:59:34  hbrew
 * Complex patch:
 * * nt/w32tools.c: Fix warnings
 * * nt/w32tools.c: Fix typo in #ifdef
 * * nt/win9x.c: Fix type in #include
 * * Config.h, sys.h, branch.c, nt/service.c,
 *     nt/win9x.c, : _beginthread()-->BEGINTHREAD()
 * * binkd.c, common.h, mkfls/nt95-msvc/Makefile.dep,
 *     nt/service.c, nt/w32tools.c,nt/win9x.c: cosmitic code cleanup
 *
 * Revision 2.48  2003/07/17 04:32:16  hbrew
 * Fix "No prototype found for 'isdigit'" warning.
 *
 * Revision 2.47  2003/07/17 02:41:47  hbrew
 * Compability with nt/service.c & nt/win9x.c.
 * Usage "--service" options as win9x "run-as-service" flag.
 *
 * Revision 2.46  2003/07/16 15:50:44  stas
 * Fix: restore "Minimise to tray"
 *
 * Revision 2.45  2003/07/16 15:42:53  stas
 * Fix: restore -T option
 *
 * Revision 2.44  2003/07/16 15:08:49  stas
 * Fix NT services to use getopt(). Improve logging for service
 *
 * Revision 2.43  2003/07/07 10:39:24  gul
 * getopt() usage fix
 *
 * Revision 2.42  2003/07/07 10:13:49  gul
 * Use getopt() for commandline parse
 *
 * Revision 2.41  2003/07/07 08:33:25  val
 * `perl-hooks' config keyword to specify perl script
 *
 * Revision 2.40  2003/07/06 06:49:43  gul
 * Code cleanup
 *
 * Revision 2.39  2003/06/30 22:42:27  hbrew
 * Print only binkd name (without path) in error messages
 *
 * Revision 2.38  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.37  2003/06/13 03:10:07  hbrew
 * Correct standalone/service logging for binkd9x.
 *
 * Revision 2.36  2003/06/12 12:04:10  gul
 * Code cleanup
 *
 * Revision 2.35  2003/06/11 07:44:21  gul
 * Cosmetics
 *
 * Revision 2.34  2003/06/10 19:15:25  gul
 * Also one commandline parser fix
 *
 * Revision 2.32  2003/06/10 07:28:25  gul
 * Fix patch about commandline parsing
 *
 * Revision 2.31  2003/06/09 13:27:28  stas
 * New command line parser
 *
 * Revision 2.30  2003/05/10 00:30:36  hbrew
 * binkd9x: -u option now support '--all' service name (uninstall all services).
 * Unneeded spaces cleanup.
 *
 * Revision 2.29  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.28  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.27  2003/03/31 21:48:59  gul
 * Avoid infinite recursion
 *
 * Revision 2.26  2003/03/31 19:53:08  gul
 * Close socket before exit
 *
 * Revision 2.25  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.24  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.23  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.22  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.21  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.20  2003/03/05 13:21:50  gul
 * Fix warnings
 *
 * Revision 2.19  2003/03/04 07:27:11  gul
 * Put OS to log on start
 *
 * Revision 2.18  2003/03/03 09:43:07  gul
 * Cosmetics changes
 *
 * Revision 2.17  2003/03/01 17:42:32  gul
 * Added tzset() call to main() start
 *
 * Revision 2.16  2003/03/01 17:33:25  gul
 * Clean daemonize code
 *
 * Revision 2.15  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.14  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.13  2003/02/22 20:19:54  gul
 * Update copyrightes, 2002->2003
 *
 * Revision 2.12  2003/02/22 14:30:18  gul
 * Make nNod var static
 *
 * Revision 2.11  2003/02/21 16:47:57  gul
 * Remove <socks.h> file and USE_SOCKS macro
 *
 * Revision 2.10  2002/12/10 22:05:29  gul
 * binkd/win32 bugfix
 *
 * Revision 2.9  2002/11/13 07:49:27  gul
 * Fix typo
 *
 * Revision 2.8  2002/11/12 16:55:58  gul
 * Run as service under win9x
 *
 * Revision 2.7  2002/07/22 19:48:30  gul
 * Update usage
 *
 * Revision 2.6  2001/11/07 13:46:20  gul
 * bugfix (added saved_argc)
 *
 * Revision 2.5  2001/10/29 19:31:44  gul
 * Added -h option (print usage)
 *
 * Revision 2.4  2001/10/28 14:52:58  gul
 * Cyrillic filenames fix
 *
 * Revision 2.3  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.2  2001/05/23 15:44:09  gul
 * help-message fixed (-C option)
 *
 * Revision 2.1  2001/01/16 03:53:23  gul
 * Added -D switch (run as daemon)
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 * nsoveiko@doe.carleton.ca 1998/12/14
 * creation of pid_file is now logged and warnings produced
 *
 * Revision 1.15  1998/05/08  03:35:47  mff
 * Added -P switch, brushed up error msgs
 *
 * Revision 1.14  1997/11/03  06:10:39  mff
 * +nodes_init()
 *
 * Revision 1.13  1997/10/23  04:20:53  mff
 * pidfiles fixed, ...
 *
 * Revision 1.12  1997/06/16  05:52:07  mff
 * Added -C, copyright note
 *
 * Revision 1.10  1997/05/17  08:44:42  mff
 * Changed cmd line processing a bit
 *
 * Revision 1.9  1997/03/09  07:16:31  mff
 * Added command line parsing, support for inetd
 *
 * Revision 1.8  1997/02/07  06:42:59  mff
 * Under UNIXs SIGHUP forces binkd to restart
 *
 * Revision 1.7  1997/02/01  05:55:24  mff
 * Changed SIGCHLD support
 *
 * Revision 1.5  1996/12/14  07:00:32  mff
 * Now we use branch() and set_break_handlers()
 *
 * Revision 1.3  1996/12/07  12:26:37  mff
 * SOCKS support by msh
 *
 * Revision 1.1.1 1996/12/02  18:26:00  ufm
 *    Port to NT
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#ifdef HAVE_FORK
#include <signal.h>
#include <sys/wait.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "server.h"
#include "client.h"
#include "tools.h"
#include "bsy.h"
#include "protocol.h"
#include "setpttl.h"
#include "sem.h"
#include "ftnnode.h"
#include "rfc2553.h"
#include "srv_gai.h"

#ifdef HAVE_GETOPT
#include <unistd.h>
#else
#include "getopt.h"
#endif

#ifdef WITH_PERL
#include "perlhooks.h"
#endif

#ifdef ZLIBDL
#include "compress.h"
#endif

#ifdef UNIX
#include "unix/daemonize.h"
#endif

#ifdef WIN32
#include "nt/service.h"
#include "nt/w32tools.h"
#ifdef BINKD9X
#include "nt/win9x.h"
#endif
#endif

#include "confopt.h"

#ifdef HAVE_THREADS
MUTEXSEM hostsem = 0;
MUTEXSEM resolvsem = 0;
MUTEXSEM lsem = 0;
MUTEXSEM blsem = 0;
MUTEXSEM varsem = 0;
MUTEXSEM config_sem = 0;
EVENTSEM eothread = 0;
EVENTSEM wakecmgr = 0;
#ifdef OS2
MUTEXSEM fhsem = 0;
#endif
#endif

/*
 * Global variables
 */
int pidcmgr = 0;		       /* pid for clientmgr */
int pidCmgr = 0;		       /* real pid for clientmgr (not 0) */
int pidsmgr = 0;		       /* pid for server */
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
static SOCKET inetd_socket = 0;
#endif

char *configpath = NULL;               /* Config file name */
char **saved_envp;

#ifdef HAVE_FORK

int mypid, got_sighup, got_sigchld;

void chld (int *childcount)
{
  int status;
  int pid;
  extern int pidcmgr;

  got_sigchld = 0;
#ifdef HAVE_WAITPID
  while ((pid = waitpid (-1, &status, WNOHANG)) > 0)
#else
  if ((pid = (int) wait (&status)) > 0)
#endif
  {
    if (pidcmgr && pid == pidcmgr) {
      if (WIFSIGNALED(status))
        Log (0, "client manager (pid=%u) exited by signal %u", pid, WTERMSIG(status));
      else
        Log (0, "client manager (pid=%u) exited, retcode %u", pid, WEXITSTATUS(status));
      exit(4);
    }
    if (childcount)
      childcount[0]--;
    if (WIFSIGNALED(status))
      Log (2, "process %u exited by signal %u", pid, WTERMSIG(status));
    else
      Log (4, "rc(%u)=%u", pid, WEXITSTATUS(status));
  }
}

void sighandler(int signo)
{
  int old_errno = errno;

  switch (signo) {
    case SIGHUP:  got_sighup++;
                  if (pidcmgr) kill(pidcmgr, SIGHUP);
                  break;
    case SIGCHLD: got_sigchld++;
                  break;
  }

#ifdef SYS5SIGNALS
  signal(signo, sighandler);
#endif
#ifdef EMXSIGNALS
  signal(signo, SIG_ACK);
#endif

  errno = old_errno;
}
#endif

#if defined(BLOCK_SIG)
void switchsignal(int how)
{
  sigset_t sigset;
  int old_errno = errno;

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGCHLD);
  sigaddset(&sigset, SIGHUP);
  sigprocmask(how, &sigset, NULL);
  errno = old_errno;
}
#endif

void usage (void)
{
#if defined(BINKD9X)
  AllocTempConsole();
#endif

  printf ("usage: binkd [-Cc"
#if defined(BINKD_DAEMONIZE)
	  "D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "i"
#elif defined(WIN32) && !defined(BINKD9X)
	  "T"
#endif
	  "pqrsvmh] [-P node]"
#if defined(WIN32)
	  " [-S name] [-t cmd]"
#endif
	  " config"
#ifdef OS2
	  " [socket]"
#endif
	  "\n"
#ifdef BINKD_DAEMONIZE
	  "  -D       run as daemon\n"
#endif
	  "  -C       reload on config change\n"
	  "  -c       run client only\n"
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "  -i       run from inetd\n"
#elif defined(BINKD9X)
	  "  -t cmd   (start|stop|restart|status|install|uninstall) service(s)\n"
	  "  -S name  set Win9x service name, all - use all services\n"
#elif defined(WIN32)
	  "  -T       minimize to Tray\n"
	  "  -t cmd   (start|stop|restart|status|install|uninstall) service\n"
	  "  -S name  set WindowsNT service name\n"
#endif
	  );
  printf ("  -P node  poll a node\n"
	  "  -p       run client only, poll, quit\n"
	  "  -q       be quiet\n"
	  "  -r       disable crypt traffic\n"
	  "  -s       run server only\n"
	  "  -v       be verbose / dump version and quit\n"
	  "  -vv      dump version with compilation flags and quit\n"
	  "  -d       dump parsed config and exit\n"
	  "  -m       disable CRAM-MD5 authorization\n"
	  "  -n       don't run binkd-client and binkd-server (check config, make polls)\n"
	  "  -h       print this help\n"
	  "\n"
	  "Copyright (c) 1996-2009 Dima Maloff and others.\n"
	  );

  puts ("\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
	  "the Free Software Foundation. See COPYING.\n"
	  "\n"
	  "Report bugs to 2:463/68 or binkd-bugs@happy.kiev.ua.");
  exit (1);
}

/* Environment variables in POSIX compliant */
#ifndef environ
extern char **environ;
#endif

/* Command line flags */
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
int inetd_flag = 0;		       /* Run from inetd (-i) */
#endif
#ifdef BINKD_DAEMONIZE
int daemon_flag = 0;		       /* Run as daemon (-D) */
#endif
int server_flag = 0;		       /* Run servermgr (-s) */
int client_flag = 0;		       /* Run clientmgr (-c) */
int poll_flag = 0;		       /* Run clientmgr, make all jobs, quit
				        * (-p) */
int quiet_flag = 0;		       /* Be quiet (-q) */
int verbose_flag = 0;		       /* Be verbose / print version (-v) */
int dumpcfg_flag = 0;		       /* Dump parsed config */
int checkcfg_flag = 0;		       /* exit(3) on config change (-C) */
int no_MD5 = 0;			       /* disable MD5 flag (-m) */
int no_crypt = 0;		       /* disable CRYPT (-r) */
int no_flag = 0;                       /* don't run client and server */

static TYPE_LIST(maskchain) psPolls;   /* Create polls (-P) */

#ifdef WIN32
enum serviceflags service_flag = w32_noservice;  /* install, uninstall, start, stop, restart wnt/w9x service */
char *service_name = NULL;
#ifdef BINKD9X
extern const char *Win9xStartService;  /* 'Run as win9x service' option */
#endif
#ifndef BINKD9X
int tray_flag = 0;                     /* minimize to tray */
#endif
#endif

const char *optstring = "CchmP:pqrsvd-:?n"
#ifdef BINKD_DAEMONIZE
			"D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
			"i"
#endif
#if defined(WIN32)
#if !defined (BINKD9X)
			"T"
#endif
			"t:iuS:"
#endif
			;

/* Parse and check command line parameters. Return config file name or NULL
 * On error prints usage information and exit (see usage() function)
 */
char *parseargs (int argc, char *argv[])
{
  char *cfgfile=NULL;
  int i, curind;

  curind = 1;
  while ((i = getopt(argc, argv, optstring)) != -1)
  {
	switch (i)
	  {
	    case '-':
	      /* GNU-style options */
	      if (!strcmp (argv[curind], "--help"))
		usage ();
	      else
#if defined (BINKD9X)
	      if (!strcmp (argv[curind], Win9xStartService))
	        service_flag = w32_run_as_service;
	      else
#endif
		Log (0, "%s: %s: unknown command line switch", extract_filename(argv[0]), argv[curind]);
	      break;
	    case 'C':
	      checkcfg_flag = 1;
	      break;
	    case 'c':
	      client_flag = 1;
	      break;
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	    case 'i':
	      inetd_flag = 1;
	      break;
#endif
#if defined(WIN32)
#if !defined (BINKD9X)
	    case 'T':
	      tray_flag = 1;
	      break;
#endif

	    case 't': /* service control/query */
	      if (isService()) break;
	      if ((service_flag != w32_noservice)) {
	        Log (0, "%s: '-t' command line switch can't mixed with '-i', '-u' and other '-t'", extract_filename(argv[0]));
	      }
	      if (!strcmp (optarg, "status"))
	        service_flag = w32_queryservice;
	      else if (!strcmp (optarg, "start"))
	        service_flag = w32_startservice;
	      else if (!strcmp (optarg, "stop"))
	        service_flag = w32_stopservice;
	      else if (!strcmp (optarg, "restart"))
	        service_flag = w32_restartservice;
	      else if (!strcmp (optarg, "install"))
	        service_flag = w32_installservice;
	      else if (!strcmp (optarg, "uninstall"))
	        service_flag = w32_uninstallservice;
	      else
	        Log (0, "%s: '-t': invalid argument '%s'", extract_filename(argv[0]), optarg);
	      break;
	    case 'S':
	      if (service_name)
	        Log(0, "%s: '-S %s': service name specified before, can't overwrite!", extract_filename(argv[0]), optarg);
	      service_name = strdup (optarg);
	      break;
	    case 'i': /* install service */
	      Log(1,"Warning: switch \"-i\" is deprecated, use \"-t install\" instead");
	      if (isService()) break;
	      if ( service_flag==w32_installservice
	        || service_flag==w32_uninstallservice
	        || service_flag==w32_queryservice
	        || service_flag==w32_startservice
	        || service_flag==w32_restartservice
	        || service_flag==w32_stopservice
	         )
	        Log (0, "%s: '-i' command line switch can't mixed with '-u', '-t' and other '-i'", extract_filename(argv[0]));
	      if (!service_flag)
	        service_flag = w32_installservice;
	      break;

	    case 'u': /* uninstall service */
	      Log(1,"Warning: switch \"-u\" is deprecated, use \"-t uninstall\" instead");
	      if (isService()) break;
	      if ( service_flag==w32_installservice
	        || service_flag==w32_uninstallservice
	        || service_flag==w32_queryservice
	        || service_flag==w32_startservice
	        || service_flag==w32_restartservice
	        || service_flag==w32_stopservice
	         )
	        Log (0, "%s: '-u' command line switch can't mixed with '-i', '-t' and other '-u'", extract_filename(argv[0]));
	      if (!service_flag)
	        service_flag = w32_uninstallservice;
	      break;
#endif

	    case 'P': /* create poll to node */
	      {
	        struct maskchain new_entry;

	        new_entry.mask = xstrdup(optarg);
	        simplelist_add(&psPolls.linkpoint, &new_entry, sizeof(new_entry));
	      }
	      break;

	    case 'p': /* run clients and exit */
	      poll_flag = client_flag = 1;
	      break;

	    case 'q': /* quiet */
	      quiet_flag = 1;
	      break;

	    case 's': /* run server only */
	      server_flag = 1;
	      break;

	    case 'm': /* MD5 off */
	      no_MD5 = 1;
	      /* fallthrough */

	    case 'r': /* CRYPT off */
	      no_crypt = 1;
	      break;

	    case 'v': /* verbose */
	      ++verbose_flag;
	      break;

	    case 'd': /* dump cfg */
	      ++dumpcfg_flag;
	      break;

#ifdef BINKD_DAEMONIZE
	    case 'D': /* run as unix daemon */
	      daemon_flag = 1;
	      break;
#endif

	    default:  /* unknown parameter/option */
	      if (optopt != '?')
	      /* getopt() already print error message
	      Log (0, "%s: %s: unknown command line switch", extract_filename(argv[0]), argv[curind]);
	      */ exit(1);

	    case 'n':
	      no_flag = 1;
	      break;

	    case 'h': /* display command line help */
	      usage();

	  }
	curind = optind;
  }
  if (optind<argc)
    cfgfile = argv[optind++];
#ifdef OS2
  if (optind<argc)
  { if ((inetd_socket = atoi(argv[argc-1])) == 0 && !isdigit(argv[argc-1][0]))
      Log (0, "%s: bad socket number", argv[optind]);
#ifdef EMX
    if ((inetd_socket = _impsockhandle (inetd_socket, 0)) == -1)
      Log (0, "_impsockhandle: %s", strerror (errno));
#endif
  }
#endif
  if (optind<argc)
    Log (1, "Extra parameters ignored");

  return cfgfile;
}

#if defined(WIN32) && !defined(BINKD9X)
int binkd_main (int argc, char *argv[]);
int main (int argc, char *argv[])
{ int res=-1;

  if( argc!=1 || (tell_start_ntservice()) )  /* See nt/service.c */
    res=binkd_main(argc,argv); /* Running not as NT service */

  return res;
}
#endif

#if defined(WIN32)
int binkd_main (int argc, char *argv[])
#else
int main (int argc, char *argv[])
#endif
{
  char tmp[128];
#if defined(HAVE_FORK)
  char **saved_argv;

  mypid = getpid();
  /* save argv as setproctitle() under some systems will change it */
  saved_argv = mkargv (argc, argv);

  configpath = parseargs(argc, saved_argv);
#else
  configpath = parseargs(argc, argv);
#endif

  saved_envp = mkargv (-1, environ);

#ifdef WIN32
  if (service_flag==w32_installservice && !configpath)
    Log (0, "%s: invalid command line: config name must be specified", extract_filename(argv[0]));
  w32Init();
#ifdef BINKD9X
  {
    int win9x_rc;

    win9x_rc = win9x_process(argc, argv);
    if (win9x_rc != -1)
      return win9x_rc;
  }
#endif
#endif

  tzset();

  if (poll_flag && server_flag)
    Log (0, "-p and -s cannot be used together");

#if defined(WIN32) && !defined(BINKD9X)
  if (service_flag!=w32_noservice)
    if (service(argc, argv, environ) && service_flag!=w32_run_as_service) {
      Log(0, "Windows NT service error");
    }
  if (tray_flag)
     do_tray_flag();
  else
  {
    atexit(UnloadBinkdIcon);
    LoadBinkdIcon();
  }
#endif

  /* No command line options: run both client and server */
  if (!client_flag && !server_flag)
    client_flag = server_flag = 1;

  InitSem (&hostsem);
  InitSem (&resolvsem);
  InitSem (&lsem);
  InitSem (&blsem);
  InitSem (&varsem);
  InitSem (&config_sem);
  InitEventSem (&eothread);
  InitEventSem (&wakecmgr);
#ifdef OS2
  InitSem (&fhsem);
#endif

  /* Init for ftnnode.c */
  nodes_init ();

  if (configpath)
  {
    current_config = readcfg (configpath);
    if (!current_config)
      Log (0, "error in configuration, aborting");
    if (dumpcfg_flag)
    {
      debug_readcfg ();
      exit(0);
    }
    InitLog(current_config->loglevel, current_config->conlog,
            current_config->logpath, current_config->nolog.first);
  }
  else if (verbose_flag)
  {
#if defined(WIN32) && defined(BINKD9X)
    AllocTempConsole();
#endif

    printf ("Binkd " MYVER " (" __DATE__ " " __TIME__ "%s)\n", get_os_string ());
    if (verbose_flag>1)
    {
      printf ("Compilation flags: " _DBNKD ".\n");
      printf ("Facilities: "
#ifndef srv_getaddrinfo
              "fsp1035 "
#endif
#ifndef HAVE_GETADDRINFO
              "rfc2553emu "
#else
              "ipv6 "
#endif
              "\n");
    }
    exit (0);
  }
  else if (argc > 1)
    Log (0, "%s: invalid command line: config name must be specified", extract_filename(argv[0]));
  else
    usage ();

  print_args (tmp, sizeof (tmp), argv + 1);
#ifdef WIN32
  if (service_flag==w32_run_as_service)
    Log (4, "BEGIN service '%s', " MYNAME "/" MYVER "%s%s", service_name, get_os_string(), tmp);
  else
    Log (4, "BEGIN standalone, " MYNAME "/" MYVER "%s%s", get_os_string(), tmp);
#else
  Log (4, "BEGIN, " MYNAME "/" MYVER "%s%s", get_os_string(), tmp);
#endif
  if (sock_init ())
    Log (0, "sock_init: %s", TCPERR ());

  bsy_init ();
  rnd ();
  initsetproctitle (argc, argv, environ);
#ifdef WIN32
  SetFileApisToOEM();
#endif

  /* Set up break handler, set up exit list if needed */
  if (!set_break_handlers ())
    Log (0, "cannot install break handlers");

#if defined(SIGPIPE) && !defined(HAVE_MSG_NOSIGNAL)
  signal(SIGPIPE, SIG_IGN);
#endif

#if defined(WITH_ZLIB) && defined(ZLIBDL)
  if (current_config->zlib_dll[0]) {
    if (!zlib_init(current_config->zlib_dll))
      Log (2, "cannot load %s, GZ compression disabled", current_config->zlib_dll);
    else
      Log (6, "%s loaded successfully", current_config->zlib_dll);
  } else
    Log (current_config->zrules.first ? 3 : 5, "zlib-dll not defined, GZ compression disabled");
#endif
#if defined(WITH_BZLIB2) && defined(ZLIBDL)
  if (current_config->bzlib2_dll[0]) {
    if (!bzlib2_init(current_config->bzlib2_dll))
      Log (2, "cannot load %s, BZ2 compression disabled", current_config->bzlib2_dll);
    else
      Log (6, "%s loaded successfully", current_config->bzlib2_dll);
  } else
    Log (current_config->zrules.first
#ifdef WITH_ZLIB
         && !zlib_loaded
#endif
         ? 3 : 5, "bzlib2-dll not defined, BZ2 compression disabled");
#endif

#ifdef WITH_PERL
  if (current_config->perl_script[0]) {
    if (!perl_init(current_config->perl_script, current_config)) {
      if (current_config->perl_strict)
        Log (0, "error parsing Perl script %s", current_config->perl_script);
    } else {
      perl_on_start(current_config);
      perl_config_loaded(current_config);
    }
  }
#endif

#ifdef HAVE_FORK
  signal (SIGCHLD, sighandler);
#endif

  { /* Create polls and release polls list */
    struct maskchain *psP;
    for (psP = psPolls.first; psP; psP = psP->next)
      poll_node (psP->mask, current_config);
    simplelist_free(&psPolls.linkpoint, destroy_maskchain);
  }

  if (no_flag)
    Log (0, "Exit on option '-n'");

#if defined(UNIX) || defined(OS2) || defined(AMIGA)
  if (inetd_flag)
  {
    protocol (inetd_socket, 0, NULL, current_config);
    soclose (inetd_socket);
    exit (0);
  }
#endif

#ifdef BINKD_DAEMONIZE
  if (daemon_flag)
  {
    if (binkd_daemonize(1) < 0)
      Log (0, "Cannot daemonize");
    else
      mypid = getpid();
  }
#endif

#if defined(HAVE_FORK)
  signal (SIGHUP, sighandler);
#endif

  if (client_flag && !server_flag)
  {
    clientmgr (0);
    exit (0);
  }

  pidsmgr = (int) getpid ();
  if (client_flag && (pidcmgr = branch (clientmgr, 0, 0)) < 0)
  {
    Log (0, "cannot branch out");
  }

  if (*current_config->pid_file)
  {
    if ( unlink (current_config->pid_file) == 0 ) /* successfully unlinked, i.e.
	                                            an old pid_file was found */
	Log (1, "unexpected pid_file: %s: unlinked", current_config->pid_file);
    else
    {
	int current_log_level = 1;
	switch ( errno )
	{
	   case ENOENT :	/* file not found or null pathname */
		current_log_level = 8; /* it's ok */
		break;
	   default :
		break;
	}
	Log (current_log_level, "unlink_pid_file: %s: %s", current_config->pid_file, strerror (errno));
    }
    create_sem_file (current_config->pid_file, 1);
  }

  servmgr ();

  return 0;
}

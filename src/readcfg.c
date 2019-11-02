/*
 *  readcfg.c -- reads config
 *
 *  readcfg.c is a part of binkd project
 *
 *  Copyright (C) 1996-2004  Dima Maloff, 5047/13 and others
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: readcfg.c,v 2.104.2.2 2014/08/09 15:17:43 gul Exp $
 *
 * $Log: readcfg.c,v $
 * Revision 2.104.2.2  2014/08/09 15:17:43  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.104.2.1  2012/10/29 19:52:57  green
 * Corrected Segfault in config error reporting on 64bit architectures
 *
 * Revision 2.104  2012/06/17 00:21:40  green
 * Segmentation violation in config dump
 *
 * Revision 2.103  2012/05/14 06:14:58  gul
 * More safe signal handling
 *
 * Revision 2.102  2012/01/22 13:54:12  green
 * Allow limiting IPv4/6 usage per node using new flags -4/-6
 *
 * Revision 2.101  2012/01/08 17:34:57  green
 * Avoid using MAXHOSTNAMELEN
 *
 * Revision 2.100  2012/01/07 16:22:26  green
 * Fix some compiler warnings
 *
 * Revision 2.99  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 * Revision 2.98  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.97  2011/08/17 15:51:46  stas
 * move declaration of default value of root_domain to Config.h
 *
 * Revision 2.96  2011/08/17 09:02:56  gul
 * Default root-domain fidonet.net changed to binkp.net
 *
 * Revision 2.95  2011/01/25 15:33:49  stas
 * Prevent segfault if length of passwords exceed 40 chars (addition for previous patch
 *
 * Revision 2.94  2009/05/31 07:16:17  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.93  2009/05/27 09:49:52  gul
 * Free perl-vars on unloading config
 *
 * Revision 2.92  2009/05/27 09:33:52  gul
 * perl-var config keyword
 * update $hosts by on_call() perl hook not only if it returns 2
 *
 * Revision 2.91  2009/05/26 13:04:35  gul
 * New perl hooks:
 * need_reload() - is it needed to reload config
 * config_loaded() - after successful reading config
 *
 * Revision 2.90  2008/12/09 09:04:09  gul
 * Use binkp from /etc/services if exists as iport/oport by default
 *
 * Revision 2.89  2007/10/30 07:33:25  gul
 * New config option dont-send-empty
 *
 * Revision 2.88  2007/10/06 10:20:04  gul
 * more accurate checkcfg()
 *
 * Revision 2.87  2007/10/04 17:30:28  gul
 * SIGHUP handler (thx to Sergey Babitch)
 *
 * Revision 2.86  2005/12/20 16:55:52  gul
 * Added '\n' after diag message
 *
 * Revision 2.85  2005/09/28 20:40:45  gul
 * Optional parameter root-domain for domain config option.
 *
 * Revision 2.84  2005/09/26 19:01:03  gul
 * bw limits code partially rewrited (not tested)
 *
 * Revision 2.83  2005/04/18 16:46:41  stream
 * Double free of pNodArray caused mysterious crashes on config reload
 *
 * Revision 2.82  2004/11/22 15:56:41  stream
 * Errors in config were not logged to file (log file name was set only
 * when config was completely loaded, checked and accepted). Now log
 * settings are changed immediately after each log-related directive
 * (only for first-time config load)
 *
 * Revision 2.81  2004/11/22 15:51:14  stas
 * Change error level to -1 for the config file parameter error (binkd/w32 will display message box with error message)
 *
 * Revision 2.80  2004/11/21 11:55:24  val
 * remove old commented code for limit-rate debug output
 *
 * Revision 2.79  2004/11/07 07:26:36  gul
 * New config options zlib-dll and bzlib2-dll
 *
 * Revision 2.78  2004/11/05 07:45:12  gul
 * Bugfix in passwords and defnode processing logic
 *
 * Revision 2.77  2004/10/26 17:36:59  gul
 * Set time intervals in human-readable form (12h instead of 43200 etc.)
 *
 * Revision 2.76  2004/10/26 15:46:42  gul
 * Process option skipmask for backward compatibility
 *
 * Revision 2.75  2004/10/25 17:04:54  gul
 * Process passwords file after all, independent of its place in config.
 * Use first password for node if several specified.
 *
 * Revision 2.74  2004/10/19 17:09:04  gul
 * Remove duplicate addresses from config
 *
 * Revision 2.73  2004/09/21 08:27:49  val
 * distinguish nodes, listed in binkd config and passwords file - overwrite defnode parameters (e.g. host) for the later
 * (hope, it'll fix reported bug with not calling defnode)
 *
 * Revision 2.72  2004/09/11 14:37:08  gul
 * Fix erroneously applied previous patch
 *
 * Revision 2.70  2004/09/06 10:47:05  val
 * bandwidth limiting code advancements, `listed' session state fix
 *
 * Revision 2.69  2004/09/02 09:00:50  val
 * fix warnings
 *
 * Revision 2.68  2004/09/02 08:56:20  val
 * bandwidth limiting config parameter 'limit-rate'
 *
 * Revision 2.67  2004/02/29 08:51:52  gul
 * Bugfix in print_node_info
 * (patch from Victor Levenets).
 *
 * Revision 2.66  2004/01/08 12:57:18  val
 * * parse up to 3 comma-separated passwords (in,pkt,out)
 * * use out password for outgoing sessions if it's set
 *
 * Revision 2.65  2004/01/07 13:12:29  gul
 * Update 2003->2004 in copyright notices
 *
 * Revision 2.64  2004/01/07 12:23:40  gul
 * Remove zaccept keyword, receiving compressed files possibility
 * is always on now if binkd was compiled with zlib/bzip2 support.
 *
 * Revision 2.63  2004/01/07 12:07:47  gul
 * New function free_nodes()
 *
 * Revision 2.62  2003/12/26 20:11:32  gul
 * Add -d commandline switch - dump parsed config and exit;
 * remove 'debugcfg' config token.
 *
 * Revision 2.61  2003/12/26 07:51:11  val
 * up to 3 comma-separated passwords in <passwords> file are parsed
 *
 * Revision 2.60  2003/11/21 19:39:59  stream
 * Initial support for "-noproxy" node option
 *
 * Revision 2.59  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.58  2003/10/23 21:38:51  gul
 * Remove C++ style comments
 *
 * Revision 2.57  2003/10/19 12:21:47  gul
 * Stream compression
 *
 * Revision 2.56  2003/10/11 08:41:46  gul
 * stricmp() -> STRICMP()
 *
 * Revision 2.55  2003/10/11 06:54:55  stas
 * ifcico/qico passwords file support
 *
 * Revision 2.54  2003/09/24 07:32:16  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 * Revision 2.53  2003/09/15 06:57:09  val
 * compression support via zlib: config keywords, improvements, OS/2 code
 *
 * Revision 2.52  2003/09/12 07:37:57  val
 * compression support via zlib (preliminary)
 *
 * Revision 2.51  2003/09/08 16:39:39  stream
 * Fixed race conditions when accessing array of nodes in threaded environment
 * ("jumpimg node structures")
 *
 * Revision 2.50  2003/09/08 08:21:20  stream
 * Cleanup config semaphore, free memory of base config on exit.
 *
 * Revision 2.49  2003/08/26 22:18:48  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.48  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.47  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.46  2003/08/25 06:11:06  gul
 * Fix compilation with HAVE_FORK
 *
 * Revision 2.45  2003/08/25 05:39:26  stas
 * Bugfix: "readcfg.c:938: `fa\' undeclared" compilation error
 *
 * Revision 2.44  2003/08/24 19:42:08  gul
 * Get FTN-domain from matched zone in exp_ftnaddress()
 *
 * Revision 2.43  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.42  2003/08/19 19:41:39  gul
 * Fix warnings
 *
 * Revision 2.41  2003/08/19 18:01:08  stream
 * Fix unix compilation
 *
 * Revision 2.40  2003/08/18 17:19:13  stream
 * Partially implemented new configuration parser logic (required for config reload)
 *
 * Revision 2.39  2003/08/18 07:35:08  val
 * multiple changes:
 * - hide-aka/present-aka logic
 * - address mask matching via pmatch
 * - delay_ADR in STATE (define DELAY_ADR removed)
 * - ftnaddress_to_str changed to xftnaddress_to_str (old version #define'd)
 * - parse_ftnaddress now sets zone to domain default if it's omitted
 *
 * Revision 2.38  2003/08/16 09:47:25  gul
 * Autodetect tzoff if not specified
 *
 * Revision 2.37  2003/08/14 07:39:36  val
 * migrate from vfprintf() to vsnprintf() in Log(), new keyword `nolog'
 *
 * Revision 2.36  2003/08/12 09:31:46  val
 * don't strlower() mask in flag/exec since we now use pmatch_ncase()
 *
 * Revision 2.35  2003/08/12 09:23:00  val
 * migrate from pmatch() to pmatch_ncase()
 *
 * Revision 2.34  2003/08/11 08:33:16  val
 * better error handling in perl hooks
 *
 * Revision 2.33  2003/07/30 11:01:37  val
 * perl-dll keyword can be used even when PERLDL is not defined (does nothing)
 *
 * Revision 2.32  2003/07/28 10:23:33  val
 * Perl DLL dynamic load for Win32, config keyword perl-dll, nmake PERLDL=1
 *
 * Revision 2.31  2003/07/07 08:33:25  val
 * `perl-hooks' config keyword to specify perl script
 *
 * Revision 2.30  2003/06/30 22:48:36  hbrew
 * Allow to override -ip, -sip, -md, -nomd in add_node()
 *
 * Revision 2.29  2003/06/12 08:30:57  val
 * check pkt header feature, see keyword 'check-pkthdr'
 *
 * Revision 2.28  2003/06/12 08:21:43  val
 * 'skipmask' is replaced with 'skip', which allows more skipping features
 *
 * Revision 2.27  2003/06/07 08:46:25  gul
 * New feature added: shared aka
 *
 * Revision 2.26  2003/05/28 09:03:17  gul
 * Typo in prev patch
 *
 * Revision 2.25  2003/05/28 08:56:33  gul
 * Reread config if passwords file changed when -C switch specified
 *
 * Revision 2.24  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.23  2003/03/25 13:17:53  gul
 * Check if inbound and temp-inbound are in the same partition
 *
 * Revision 2.22  2003/03/11 00:04:26  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.21  2003/03/10 10:57:45  gul
 * Extern declarations moved to header files
 *
 * Revision 2.20  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.19  2003/03/01 15:00:17  gul
 * Join skipmask and overwrite into common maskchain
 *
 * Revision 2.18  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.17  2003/02/23 16:31:21  gul
 * Add "-sip" option in node string.
 * Change "-ip" check logic.
 *
 * Revision 2.16  2003/02/22 21:32:46  gul
 * Amiga Style Outbound support
 *
 * Revision 2.15  2003/02/22 20:19:54  gul
 * Update copyrightes, 2002->2003
 *
 * Revision 2.14  2003/02/13 19:18:11  gul
 * minor fix
 *
 * Revision 2.13  2003/01/29 19:32:03  gul
 * Code cleanup, prevent segfault on bad config
 *
 * Revision 2.12  2003/01/16 14:34:11  gul
 * Fix segfault under unix
 *
 * Revision 2.11  2002/12/17 14:02:22  gul
 * change strcasecmp -> STRICMP
 *
 * Revision 2.10  2002/12/17 13:00:44  gul
 * Fix previous patch
 *
 * Revision 2.9  2002/12/10 21:31:30  gul
 * Bugfix for check filebox and outbound
 *
 * Revision 2.8  2002/11/14 13:01:43  gul
 * Bugfix for previous patch
 *
 * Revision 2.7  2002/11/12 17:41:02  gul
 * Check for (personal) outbox pointed to (common) outbound
 *
 * Revision 2.6  2002/07/21 10:35:44  gul
 * overwrite option
 *
 * Revision 2.5  2002/05/11 08:37:32  gul
 * Added token deletedirs
 *
 * Revision 2.4  2002/05/06 19:25:39  gul
 * new keyword inboundCase in config
 *
 * Revision 2.3  2002/02/22 00:18:34  gul
 * Run by-file events with the same command-line once after session
 *
 * Revision 2.2  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.1  2001/02/15 11:03:18  gul
 * Added crypt traffic possibility
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.14  1997/10/23  03:45:34  mff
 * +fdinhist, +fdouthist, +root_domain, many fixes to hide pNod into
 * ftnnode.c
 *
 * Revision 1.13  1997/09/04  02:53:01  mff
 * Added fdinhist/fdouthist keywords to support FrontDoor-style history.
 * Added support for multiple hosts per node. Find_port() moved to
 * iptools.c
 *
 * Revision 1.12  1997/08/19  21:42:29  mff
 * Changes to support multiple hosts per node: in FTN_NODE
 * host/port pair replaced with asciiz string in ``hosts''
 *
 *
 * 1997/07/11  11:47:55  maxb
 * Added fdinhist and fdouthist keyword
 *
 * Revision 1.11  1997/06/16  05:42:30  mff
 * Added binlog and tzoff keywords.
 *
 * Revision 1.10  1997/05/17  08:43:23  mff
 * Flavours for fileboxes were ignored
 *
 * Revision 1.9  1997/03/28  06:36:28  mff
 * Added "exec" keyword
 *
 * Revision 1.8  1997/03/15  05:06:08  mff
 * Added -nr key to node statement
 *
 * Revision 1.6  1997/03/09  07:13:30  mff
 * getservbyname with iport/oport, added reading of syslog facility
 *
 * Revision 1.5  1997/02/07  06:55:11  mff
 * `include', extened `node', more?
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
#define SYSLOG_NAMES
#include <syslog.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "sem.h"
#include "tools.h"
#include "protoco2.h"
#include "srif.h"
#include "iptools.h"
#include "readflo.h"
#include "ftnaddr.h"
#include "ftnnode.h"
#include "ftndom.h"
#include "ftnq.h"

#ifdef WITH_PERL
#include "perlhooks.h"
#endif

/*
 * Pointer to actual config used by all processes
 */
BINKD_CONFIG  *current_config;

/*
 * Temporary static structure for configuration reading
 */
static BINKD_CONFIG  work_config;

static char *current_path = "<command line>";
static int   current_line;
static char  linebuf[MAXCFGLINE + 1];
static char  spaces[] = " \n\t";

/*
 * Add static object to list (allocate new entry and copy static data)
 */
void simplelist_add(struct list_linkpoint *lp, void *data, int size)
{
  DEFINE_LIST(__dummy__) *l = (void *) lp; /* linkpoint always at the beginning of list */

  void *p = xalloc(size);  /* new data element - variable size! */

  memcpy(p, data, size);
  ((struct list_itemlink *)p)->next = NULL;  /* item.next always at the beginning of structure */

  if (l->linkpoint.last == NULL)
  {
    l->first = p;
    l->linkpoint.last = p;
  }
  else
    l->linkpoint.last = l->linkpoint.last->next = p;
}

/*
 * Destroy list and all its entries
 */
void simplelist_free(struct list_linkpoint *lp, void (*destructor)(void *))
{
  DEFINE_LIST(__dummy__) *l = (void *) lp; /* linkpoint always at the beginning of list */
  void *p, *next_p;

  for (p = l->first; p; p = next_p)
  {
    if (destructor)
      destructor(p);
    next_p = ((struct list_itemlink *)p)->next;
    free(p);
  }
  l->first = NULL;
  l->linkpoint.last = NULL;
}

/*
 * Destructors for list entries
 */
static void destroy_configlist(void *p)
{
  struct conflist_type *pp = p;

  xfree(pp->path);
}

static void destroy_domains(void *p)
{
  FTN_DOMAIN *pp = p;

  xfree(pp->path);
  xfree(pp->dir);
  xfree(pp->idomain);
}

static void destroy_evtflags(void *p)
{
  EVT_FLAG *pp = p;

  xfree(pp->path);
  xfree(pp->command);
  xfree(pp->pattern);
}

static void destroy_rfrules(void *p)
{
  RF_RULE *pp = p;

  xfree(pp->from);
  xfree(pp->to);
}

void destroy_maskchain(void *p)
{
  struct maskchain *pp = p;

  xfree(pp->mask);
}

static void destroy_skipchain(void *p)
{
  struct skipchain *pp = p;

  xfree(pp->mask);
}

static void destroy_akachain(void *p)
{
  struct akachain *pp = p;

  xfree(pp->mask);
}

static void destroy_shares(void *p)
{
  SHARED_CHAIN *pp = p;

  simplelist_free(&pp->sfa.linkpoint, NULL);
}

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
static void destroy_zrule(void *p)
{
  struct zrule *pp = p;

  xfree(pp->mask);
}
#endif

#ifdef BW_LIM
static void destroy_rate(void *p)
{
  struct ratechain *pp = p;

  xfree(pp->mask);
}
#endif

#ifdef WITH_PERL
static void destroy_perlvars(void *p)
{
  struct perl_var *pp = p;

  xfree(pp->name);
  xfree(pp->val);
}
#endif

/*#ifdef HTTPS
static void destroy_proxy(void *p)
{
  struct proxy_info *pp = p;

  xfree(pp->host);
  xfree(pp->username);
  xfree(pp->password);
}
#endif*/

/*
 * Zero config data and set default values of other variables
 * Note: must be called on locked config
 */
void lock_config_structure(BINKD_CONFIG *c)
{
  if (++(c->usageCount) == 1)
  {
    /* First-time call: init default values */

    snprintf(c->iport, sizeof(c->iport), "%s", find_port(""));
    snprintf(c->oport, sizeof(c->oport), "%s", find_port(""));
    c->call_delay        = 60;
    c->rescan_delay      = 60;
    c->nettimeout        = DEF_TIMEOUT;
    c->oblksize          = DEF_BLKSIZE;
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
    c->zminsize          = 1024;
    c->zlevel            = 0;
#endif
    c->max_servers       = 100;
    c->max_clients       = 100;
    c->minfree           = -1;
    c->minfree_nonsecure = -1;
    c->loglevel          = 4;
    c->conlog            = 1;
    c->inboundcase       = INB_SAVE;
    c->hold_skipped      = 60 * 60;
    c->tzoff             = -1; /* autodetect */

    strcpy(c->inbound, ".");
    strcpy(c->root_domain, ROOT_DOMAIN);
  }
}

/*
 * Deregister config usage. When config is not used anymore,
 * free all dynamically allocated entries in configuration data
 */
void unlock_config_structure(BINKD_CONFIG *c, int on_exit)
{
  int  usage;

  LockSem(&config_sem);
  usage = --(c->usageCount);
  ReleaseSem(&config_sem);

  if (usage == 0)
  {
    /* Free all dynamic data here */

    if (c != &work_config && !binkd_exit)
      Log(4, "previous config is no longer in use, unloading");

#ifdef WITH_PERL
    if (c->perl)
      perl_done(c, on_exit);
#endif

    xfree(c->pAddr);

    free_nodes(c);
    xfree(c->pkthdr_bad);

    simplelist_free(&c->config_list.linkpoint, destroy_configlist);
    simplelist_free(&c->pDomains.linkpoint,    destroy_domains);
    simplelist_free(&c->overwrite.linkpoint,   destroy_maskchain);
    simplelist_free(&c->nolog.linkpoint,       destroy_maskchain);
    simplelist_free(&c->skipmask.linkpoint,    destroy_skipchain);
    simplelist_free(&c->rf_rules.linkpoint,    destroy_rfrules);
    simplelist_free(&c->evt_flags.linkpoint,   destroy_evtflags);
    simplelist_free(&c->akamask.linkpoint,     destroy_akachain);
    simplelist_free(&c->shares.linkpoint,      destroy_shares);
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
    simplelist_free(&c->zrules.linkpoint,      destroy_zrule);
#endif
#ifdef BW_LIM
    simplelist_free(&c->rates.linkpoint,       destroy_rate);
#endif
#ifdef WITH_PERL
    simplelist_free(&c->perl_vars.linkpoint,   destroy_perlvars);
#endif

/*
#ifdef HTTPS
    simplelist_free(&c->proxylist,   destroy_proxy);
#endif
*/

    if (c != &work_config && !binkd_exit)
      free(c);
  }
}

/*
 * Locks current config structure and return pointer to it
 * Can be called anychronously in new thread
 */
BINKD_CONFIG *lock_current_config(void)
{
  BINKD_CONFIG *ret;

  LockSem(&config_sem);

  ret = current_config;
  if (ret)
    lock_config_structure(ret);

  ReleaseSem(&config_sem);

  return ret;
}


#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
int syslog_facility = -1;
#endif

typedef struct _KEYWORD KEYWORD;
struct _KEYWORD
{
  const char *key;
  int  (*callback) (KEYWORD *key, int wordcount, char **words);
  void *var;
  long option1;
  long option2;
};

static int passwords (KEYWORD *key, int wordcount, char **words);
static int include (KEYWORD *key, int wordcount, char **words);
static int read_aka_list (KEYWORD *key, int wordcount, char **words);
static int read_domain_info (KEYWORD *key, int wordcount, char **words);
static int read_node_info (KEYWORD *key, int wordcount, char **words);
static int read_int (KEYWORD *key, int wordcount, char **words);
static int read_time (KEYWORD *key, int wordcount, char **words);
static int read_string (KEYWORD *key, int wordcount, char **words);
static int read_bool (KEYWORD *key, int wordcount, char **words);
static int read_log_string (KEYWORD *key, int wordcount, char **words);
static int read_log_int (KEYWORD *key, int wordcount, char **words);
static int read_flag_exec_info (KEYWORD *key, int wordcount, char **words);
static int read_rfrule (KEYWORD *key, int wordcount, char **words);
static int read_mask (KEYWORD *key, int wordcount, char **words);
static int read_akachain (KEYWORD *key, int wordcount, char **words);
static int read_inboundcase (KEYWORD *key, int wordcount, char **words);
static int read_dontsendempty (KEYWORD *key, int wordcount, char **words);
static int read_port (KEYWORD *key, int wordcount, char **words);
static int read_skip (KEYWORD *key, int wordcount, char **words);
static int read_check_pkthdr (KEYWORD *key, int wordcount, char **words);
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
static int read_zrule (KEYWORD *key, int wordcount, char **words);
#endif
#ifdef BW_LIM
static int read_rate (KEYWORD *key, int wordcount, char **words);
#endif
#ifdef WITH_PERL
static int read_perlvar (KEYWORD *key, int wordcount, char **words);
#endif

/* Helper functions for shared akas implementation */
static int read_shares (KEYWORD *key, int wordcount, char **words);

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
static int read_syslog_facility (KEYWORD *key, int wordcount, char **words);
#endif

#define DONT_CHECK 0x7fffffffl

static KEYWORD keywords[] =
{
  {"include", include, NULL, 0, 0},
  {"passwords", passwords, work_config.passwords, 'f', 0},
  {"log", read_log_string, work_config.logpath, 'f', 0},
  {"loglevel", read_log_int, &work_config.loglevel, 0, DONT_CHECK},
  {"conlog", read_log_int, &work_config.conlog, 0, DONT_CHECK},
  {"binlog", read_string, work_config.binlogpath, 'f', 0},
  {"fdinhist", read_string, work_config.fdinhist, 'f', 0},
  {"fdouthist", read_string, work_config.fdouthist, 'f', 0},
  {"tzoff", read_time, &work_config.tzoff, -12*60*60, 12*60*60},
  {"domain", read_domain_info, NULL, 0, 0},
  {"address", read_aka_list, NULL, 0, 0},
  {"sysname", read_string, work_config.sysname, 0, MAXSYSTEMNAME},
  {"bindaddr", read_string, work_config.bindaddr, 0, 16},
  {"sysop", read_string, work_config.sysop, 0, MAXSYSOPNAME},
  {"location", read_string, work_config.location, 0, MAXLOCATIONNAME},
  {"nodeinfo", read_string, work_config.nodeinfo, 0, MAXNODEINFO},
  {"iport", read_port, &work_config.iport, 0, 0},
  {"oport", read_port, &work_config.oport, 0, 0},
  {"rescan-delay", read_time, &work_config.rescan_delay, 1, DONT_CHECK},
  {"call-delay", read_time, &work_config.call_delay, 1, DONT_CHECK},
  {"timeout", read_time, &work_config.nettimeout, 1, DONT_CHECK},
  {"oblksize", read_int, &work_config.oblksize, MIN_BLKSIZE, MAX_BLKSIZE},
  {"maxservers", read_int, &work_config.max_servers, 0, DONT_CHECK},
  {"maxclients", read_int, &work_config.max_clients, 0, DONT_CHECK},
  {"inbound", read_string, work_config.inbound, 'd', 0},
  {"inbound-nonsecure", read_string, work_config.inbound_nonsecure, 'd', 0},
  {"temp-inbound", read_string, work_config.temp_inbound, 'd', 0},
  {"node", read_node_info, NULL, 0, 0},
  {"defnode", read_node_info, NULL, 1, 0},
  {"kill-dup-partial-files", read_bool, &work_config.kill_dup_partial_files, 0, 0},
  {"kill-old-partial-files", read_time, &work_config.kill_old_partial_files, 1, DONT_CHECK},
  {"kill-old-bsy", read_time, &work_config.kill_old_bsy, 1, DONT_CHECK},
  {"percents", read_bool, &work_config.percents, 0, 0},
  {"minfree", read_int, &work_config.minfree, 0, DONT_CHECK},
  {"minfree-nonsecure", read_int, &work_config.minfree_nonsecure, 0, DONT_CHECK},
  {"flag", read_flag_exec_info, NULL, 'f', 0},
  {"exec", read_flag_exec_info, NULL, 'e', 0},
  {"printq", read_bool, &work_config.printq, 0, 0},
  {"try", read_int, &work_config.tries, 0, 0xffff},
  {"hold", read_time, &work_config.hold, 0, DONT_CHECK},
  {"hold-skipped", read_time, &work_config.hold_skipped, 0, DONT_CHECK},
  {"backresolv", read_bool, &work_config.backresolv, 0, 0},
  {"pid-file", read_string, work_config.pid_file, 'f', 0},
#ifdef HTTPS
  {"proxy", read_string, work_config.proxy, 0, BINKD_FQDNLEN + 40},
  {"socks", read_string, work_config.socks, 0, BINKD_FQDNLEN + 40},
#endif
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
  {"syslog", read_syslog_facility, &syslog_facility, 0, 0},
#endif
  {"ftrans", read_rfrule, NULL, 0, 0},
  {"send-if-pwd", read_bool, &work_config.send_if_pwd, 0, 0},
  {"root-domain", read_string, work_config.root_domain, 0, BINKD_FQDNLEN},
  {"prescan", read_bool, &work_config.prescan, 0, 0},
  {"connect-timeout", read_time, &work_config.connect_timeout, 0, DONT_CHECK},
#ifdef MAILBOX
  {"filebox", read_string, work_config.tfilebox, 'd', 0},
  {"brakebox", read_string, work_config.bfilebox, 'd', 0},
  {"deletebox", read_bool, &work_config.deleteablebox, 0, 0},
#endif
  {"skip", read_skip, NULL, 0, 0},
  {"skipmask", read_skip, NULL, 1, 0},
  {"inboundcase", read_inboundcase, &work_config.inboundcase, 0, 0},
  {"deletedirs", read_bool, &work_config.deletedirs, 0, 0},
  {"overwrite", read_mask, &work_config.overwrite, 0, 0},
  {"dont-send-empty", read_dontsendempty, &work_config.dontsendempty, 0, 0},

  /* shared akas definitions */
  {"share", read_shares, 0, 0, 0},
  /* check pkt header keyword */
  {"check-pkthdr", read_check_pkthdr, NULL, 0, 0},

#ifdef AMIGADOS_4D_OUTBOUND
  {"aso", read_bool, &work_config.aso, 0, 0},
#endif

#ifdef WITH_PERL
  {"perl-hooks", read_string, work_config.perl_script, 'f', 0},
  {"perl-dll", read_string, work_config.perl_dll, 'f', 0},
  {"perl-strict", read_bool, &work_config.perl_strict, 0, 0},
  {"perl-var", read_perlvar, &work_config.perl_vars, 0, 0},
#endif

  {"nolog", read_mask, &work_config.nolog, 0, 0},
  {"hide-aka", read_akachain, &work_config.akamask, ACT_HIDE, 0},
  {"present-aka", read_akachain, &work_config.akamask, ACT_PRESENT, 0},

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  {"zlevel", read_int, &work_config.zlevel, 0, 9},
  {"zminsize", read_int, &work_config.zminsize, 0, DONT_CHECK},
  {"zallow", read_zrule, &work_config.zrules, ZRULE_ALLOW, 0},
  {"zdeny", read_zrule, &work_config.zrules, ZRULE_DENY, 0},
#endif
#if defined (WITH_ZLIB) && defined (ZLIBDL)
  {"zlib-dll", read_string, work_config.zlib_dll, 'f', 0},
#endif
#if defined (WITH_BZLIB2) && defined (ZLIBDL)
  {"bzlib2-dll", read_string, work_config.bzlib2_dll, 'f', 0},
#endif

#ifdef BW_LIM
  {"limit-rate", read_rate, NULL, 0, 0},
#endif

  {NULL, NULL, NULL, 0, 0}
};

/* Check for (personal) outbox pointed to (common) outbound */
static int check_outbox(char *obox)
{
  FTN_DOMAIN *pd;
#ifndef UNIX
  char *OBOX, *PATH=NULL;
#endif
  if (obox == NULL)
    return 0;
#ifndef UNIX
  OBOX = strupper(xstrdup(obox));
#endif
  for (pd=work_config.pDomains.first; pd; pd=pd->next)
  {
    if (pd->alias4)
      continue;
    if (pd->path)
    {
      char *s;
#ifdef UNIX
      if (obox==strstr(obox, pd->path))
      {
        s = obox+strlen(pd->path);
        if ((*s == '\\' || *s == '/') && STRICMP(s+1, pd->dir) == 0)
          return 1;
      }
#else
      PATH = strupper(xstrdup(pd->path));
      if (OBOX==strstr(OBOX, PATH))
      {
        s = OBOX+strlen(PATH);
        if ((*s == '\\' || *s == '/') && STRICMP(s+1, pd->dir) == 0)
        {
          free(PATH);
          free(OBOX);
          return 1;
        }
      }
      free(PATH);
#endif
    }
  }
#ifndef UNIX
  free(OBOX);
#endif
  return 0;
}

static int ConfigError(char *format, ...)
{
  va_list args;

  va_start(args, format);
  Log(-1, "%s: line %d: error in configuration files\n", current_path, current_line);
  vLog(1, format, args);
  va_end(args);
  return 0;
}

static int ConfigNeedNumber(char *s)
{
  return ConfigError("%s: expecting a number", s);
}

static int ConfigNeedTime(char *s)
{
  return ConfigError("%s: expecting a time interval", s);
}

static int isDefined(char *value, char *name)
{
  if (*value == 0)
    return ConfigError("'%s' must be defined", name);
  return 1;
}

static void add_to_config_list(const char *path, FILE *f)
{
  struct  conflist_type new_entry;
  struct  stat sb;

  new_entry.path  = xstrdup(path);
  if (f != NULL && fstat(fileno(f), &sb) == 0)
    new_entry.mtime = sb.st_mtime;
  else
  {
    Log(2, "Cannot get modification time for %s: %s", path, strerror(errno));
    new_entry.mtime = 0;
  }
  simplelist_add(&work_config.config_list.linkpoint, &new_entry, sizeof(new_entry));
}

static int check_boxes(FTN_NODE *node, void *arg)
{
  struct stat st;
  char addr[FTN_ADDR_SZ + 1];

  ftnaddress_to_str(addr, &(node->fa));
  if (node->obox && node->obox[0])
  {
    if (stat(node->obox, &st) || (st.st_mode & S_IFDIR) == 0)
    {
      ConfigError("Outbox for %s does not exist (link %s)", node->obox, addr);
      return 1;
    }
    if (check_outbox(node->obox))
    {
      ConfigError("Outbox cannot point to outbound! (link %s)", addr);
      return 1;
    }
  }
  if (node->ibox && node->ibox[0])
  {
    if (stat(node->ibox, &st) || (st.st_mode & S_IFDIR) == 0)
    {
      ConfigError("Inbox for %s does not exist (link %s)", node->ibox, addr);
      return 1;
    }
    if (arg && st.st_dev != *(dev_t *)arg)
    {
      ConfigError("Inbox and temp-inbound must be in the same partition (link %s)", addr);
      return 1;
    }
  }
  return 0;
}

static int check_config(void)
{
  struct stat st, si;
  if (work_config.temp_inbound[0] && stat(work_config.temp_inbound, &st) == 0)
  {
    if (stat(work_config.inbound, &si) == 0 && st.st_dev != si.st_dev)
      return ConfigError("Inbound and temp-inbound must be in the same partition");
    if (stat(work_config.inbound_nonsecure, &si) == 0 && st.st_dev != si.st_dev)
      return ConfigError("Unsecure-inbound and temp-inbound must be in the same partition");
  }
  if (foreach_node(check_boxes, work_config.temp_inbound[0] ? &st.st_dev : NULL, &work_config))
    return 0;
  return 1;
}

static void split_passwords(char *password, char **pkt_pwd, char **out_pwd)
{
  if (!password) { *pkt_pwd = *out_pwd = NULL; return; }
  /* parse "in_pwd,pkt_pwd,out_pwd" to password,pkt_pwd,out_pwd */
  /* if any (pkt|out)_pwd is omitted, set pointer to NULL not to "" */
  *pkt_pwd = strchr(password, ',');
  *out_pwd = NULL;
  if (*pkt_pwd) {
    *((*pkt_pwd)++) = 0; 
    *out_pwd = strchr(*pkt_pwd, ',');
    if (*out_pwd) *((*out_pwd)++) = 0;
    if (!**pkt_pwd) *pkt_pwd = NULL;
    if (*out_pwd && !**out_pwd) *out_pwd = NULL;
  }
}

static int read_passwords(char *filename)
{
  FILE *in;
  FTN_ADDR fa;

  if ((in = fopen(filename, "rt")) == NULL)
    return ConfigError("unable to open password file (%s)", filename);

  add_to_config_list(filename, in);
  while (fgets (linebuf, sizeof (linebuf), in))
  {
    char *node, *password;

    node = strtok(linebuf, spaces);
    if(node && STRICMP(node,"password")==0 )
      node = strtok(NULL, spaces); /* ifcico/qico passwords file detected */

    if (node)
    {
      password = strtok(NULL, spaces);
      if (password && parse_ftnaddress (node, &fa, work_config.pDomains.first)) /* Do not process if any garbage found */
      {
        FTN_NODE *pn;
        char *pkt_pwd, *out_pwd;
        split_passwords(password, &pkt_pwd, &out_pwd);
        exp_ftnaddress (&fa, work_config.pAddr, work_config.nAddr, work_config.pDomains.first);
        pn = add_node (&fa, NULL, password, pkt_pwd, out_pwd, '-', NULL, NULL,
                  NR_USE_OLD, ND_USE_OLD, MD_USE_OLD, RIP_USE_OLD, 
		  HC_USE_OLD, NP_USE_OLD, AF_USE_OLD,
#ifdef BW_LIM
                  BW_DEF, BW_DEF,
#endif
                  &work_config);
        if (pn && !pn->listed) pn->listed = NL_PASSWORDS;
      }
    }
  }
  fclose(in);

  return 1;
}

/*
 * Parses one configuration file
 */
static int readcfg0 (char *path)
{
#define MAX_WORDS_ON_LINE 64

  FILE   *in;
  char   *words[MAX_WORDS_ON_LINE];
  int     success;

  if ((in = fopen (path, "r")) == 0)
    return ConfigError("cannot open %s: %s", path, strerror(errno));

  current_line = 0;
  current_path = path;

  add_to_config_list (path, in);

  for (success = 1; success && fgets (linebuf, sizeof (linebuf), in); )
  {
    int      wordcount;
    KEYWORD *k;

    ++current_line;

    /* Get array of all tokens on line */
    for (wordcount = 0; wordcount < MAX_WORDS_ON_LINE; wordcount++)
    {
      words[wordcount] = getword(linebuf, wordcount+1);
      if (words[wordcount] == NULL)
        break;
    }
    if (wordcount == MAX_WORDS_ON_LINE)
      ConfigError("warning: more then %d words on line will be ignored", MAX_WORDS_ON_LINE);

    if (wordcount != 0)
    {
      for (k = keywords; k->key; k++)
        if (!STRICMP (k->key, words[0]))
          break;
      if (k->key)
        success = k->callback(k, wordcount-1, words+1);
      else
        success = ConfigError("%s: unknown keyword", words[0]);
    }
    while (--wordcount > 0)
      free (words[wordcount]);
  }

  fclose (in);
  return success;
}

/*
 * Parses and reads _path as config.file
 */
BINKD_CONFIG *readcfg (char *path)
{
  BINKD_CONFIG *new_config = NULL;

  memset(&work_config, 0, sizeof(work_config));
  lock_config_structure(&work_config);

  if (
      readcfg0(path)                               &&
      isDefined(work_config.sysname,  "sysname")   &&
      isDefined(work_config.sysop,    "sysop")     &&
      isDefined(work_config.location, "location")  &&
      isDefined(work_config.nodeinfo, "nodeinfo")
     )
  {
    do
    {
      if (work_config.nAddr == 0)
      {
        ConfigError("your address should be defined");
        break;
      }
      if (work_config.pDomains.first == NULL)
      {
        ConfigError("at least one domain should be defined");
        break;
      }

      /* setup command-line overrides */

      if (quiet_flag)
      {
        work_config.percents = 0;
        work_config.conlog = 0;
        work_config.printq = 0;
      }
      switch (verbose_flag)
      {
      case 0:
        break;
      case 1:
        work_config.percents = work_config.printq = 1;
        work_config.loglevel = work_config.conlog = 4;
        break;
      case 2:
      case 3:
      default:
        work_config.percents = work_config.printq = 1;
        work_config.loglevel = work_config.conlog = 6;
        break;
      }

      if (!*work_config.inbound_nonsecure)
        strcpy (work_config.inbound_nonsecure, work_config.inbound);

      if (work_config.passwords[0])
        if (!read_passwords(work_config.passwords))
          break;

      if (!check_config())
        break;

      /* All checks passed! */

      new_config = xalloc(sizeof(work_config));
      memcpy(new_config, &work_config, sizeof(work_config));

    } while (0);
  }

  if (!new_config)
  {
    /* Config error. Abort or continue? */
    if (current_config)
    {
      Log(1, "error in configuration, using old config");
      unlock_config_structure(&work_config, 0);
    }
  }

  return new_config;
}

/*
 * Check for change in configuration files
 * !!! Must be called from "main" thread only !!!
 */

int checkcfg(void)
{
  struct stat sb;
  struct conflist_type *pc;
  int need_reload;
  BINKD_CONFIG *new_config, *old_config;

#ifdef HAVE_FORK
  if (got_sighup)
    Log (2, "got SIGHUP");
  need_reload = got_sighup;
  got_sighup = 0;
#else
  need_reload = 0;
#endif
  if (!checkcfg_flag && !need_reload)
    return 0;

  for (pc = current_config->config_list.first; pc; pc = pc->next)
  {
    if (stat (pc->path, &sb))
      continue;
    if (pc->mtime == 0)
      pc->mtime = sb.st_mtime;
    else if (pc->mtime != sb.st_mtime)
    {
      /* If reload failed, this will keep us from constant reload */
      pc->mtime = sb.st_mtime;

      Log(2, "%s changed!", pc->path);
      need_reload = 1;
    }
  }

#ifdef WITH_PERL
  switch (perl_need_reload(current_config, current_config->config_list.first, need_reload))
  { case 1: need_reload = 1; break;
    case 2: need_reload = 0; break;
  }
#endif

  if (!need_reload)
    return 0;
  /* Reload starting from first file in list */
  Log(2, "Reloading configuration...");
  pc = current_config->config_list.first;
  new_config = readcfg(pc->path);

#ifdef WITH_PERL
#if defined(HAVE_THREADS) || defined(PERL_MULTIPLICITY)
  if (new_config && new_config->perl_script[0])
  {
    if (!perl_init(new_config->perl_script, new_config))
      if (new_config->perl_strict)
      {
        Log (1, "error parsing Perl script %s, using old config", new_config->perl_script);
        unlock_config_structure(new_config, 0);
        new_config = NULL;
      }
  }
#else
  if (new_config)
  {
    if (current_config && current_config->perl)
      perl_done(current_config, 0);
    if (new_config->perl_script[0])
    {
      if (!perl_init(new_config->perl_script, new_config))
        if (new_config->perl_strict)
          Log (0, "error parsing Perl script %s", new_config->perl_script);
    }
  }
#endif
#endif

  if (new_config)
  {
    InitLog(new_config->loglevel, new_config->conlog,
            new_config->logpath, new_config->nolog.first);

#ifdef WITH_PERL
    /* before change current_config,
     * because clone to clientmanager should be after config_loaded() call
     */
    perl_config_loaded(new_config);
#endif

    LockSem(&config_sem);
    old_config = current_config;
    current_config = new_config;
    ReleaseSem(&config_sem);

    if (old_config)
      unlock_config_structure(old_config, 0);

  }

  return (new_config ? 1 : 0);
}

/*
 * Check number of arguments for command. Return 0 on error
 */
static int isArgCount(int expectedCount, int realCount)
{
  if (realCount != expectedCount)
    return ConfigError("%d argument(s) required", expectedCount);
  return 1;
}

static int SyntaxError(KEYWORD *k)
{
  return ConfigError("the syntax is incorrect for '%s'", k->key);
}

static void check_dir_path (char *s)
{
  if (s)
  {
    char *w = s + strlen (s) - 1;

    while (w >= s && (*w == '/' || *w == '\\'))
      *(w--) = 0;
  }
}

static int Config_ParseAddress(char *s, FTN_ADDR *a)
{
  if (!parse_ftnaddress (s, a, work_config.pDomains.first))
    return ConfigError("%s: the address cannot be parsed", s);
  return 1;
}


/*
 *  METHODS TO PROCESS KEYWORDS' ARGUMENTS
 */

static int include (KEYWORD *key, int wordcount, char **words)
{
  static int level;

  char *old_path;
  int   old_line;
  int   success;

  UNUSED_ARG(key);

  if (!isArgCount(1, wordcount))
    return 0;

  if (level == MAXINCLUDELEVEL)
    return ConfigError("too many nested include commands");

  ++level;
  old_path = current_path;
  old_line = current_line;
  success  = readcfg0(words[0]);
  current_path = old_path;
  current_line = old_line;
  --level;

  return success;
}

static int passwords (KEYWORD *key, int wordcount, char **words)
{
  if (!read_string(key, wordcount, words))
    return 0;
  return 1;
}

static int read_aka_list (KEYWORD *key, int wordcount, char **words)
{
  int       i, n;
  FTN_ADDR  a;

  if (wordcount == 0)
    return SyntaxError(key);

  for (i = 0; i < wordcount; i++)
  {
    if (!Config_ParseAddress(words[i], &a))
      return 0;
    if (!is4D (&a))
      return ConfigError("%s: must be at least a 4D address", words[i]);
    if (a.domain[0] == 0)
    {
      if (work_config.pDomains.first == NULL)
        return ConfigError("at least one domain must be defined first");
      strcpy (a.domain, get_matched_domain(a.z, work_config.pAddr, work_config.nAddr, work_config.pDomains.first));
    }
    for (n = 0; n < work_config.nAddr; n++)
      if (!ftnaddress_cmp (&a, work_config.pAddr + n))
        break;
    if (n < work_config.nAddr)
    {
      Log (2, "Duplicate address %s in config ignored", words[i]);
      continue;
    }
    work_config.pAddr = xrealloc (work_config.pAddr, sizeof (FTN_ADDR) * (work_config.nAddr+1));
    memcpy (work_config.pAddr + work_config.nAddr, &a, sizeof(FTN_ADDR));
    ++work_config.nAddr;
  }

  return 1;
}

static int read_domain_info (KEYWORD *key, int wordcount, char **words)
{
  FTN_DOMAIN new_domain, *tmp_domain;

  UNUSED_ARG(key);

  if (wordcount != 3 && wordcount != 4) {
    ConfigError("3 or 4 arguments required");
    return 0;
  }

  if (get_domain_info (words[0], work_config.pDomains.first))
    return ConfigError("%s: duplicate domain", words[0]);

  memset(&new_domain, 0, sizeof(new_domain));
  strnzcpy (new_domain.name, words[0], sizeof (new_domain.name));

  if (!STRICMP (words[1], "alias-for"))
  {
    if ((tmp_domain = get_domain_info (words[2], work_config.pDomains.first)) == 0)
      return ConfigError("%s: undefined domain", words[2]);
    if (!isArgCount(3, wordcount))
      return 0;
    new_domain.alias4 = tmp_domain;
  }
  else
  {
    char *s, *new_dir, *new_path;
    int   z;

    for (s = words[2]; *s && isdigit(*s); s++);
    if ((*s && !isspace(*s)) || (z = atoi (words[2])) <= 0)
      return ConfigError("%s: invalid zone", words[2]);

    new_domain.z[0] = z;
    new_domain.z[1] = 0;
    new_domain.alias4   = 0;

    check_dir_path(words[1]);
    if ((s = last_slash(words[1])) == 0)
    {
      new_dir  = words[1];
      new_path = ".";
    }
    else
    {
      *s = 0;
      new_dir  = s + 1;
      new_path = words[1];
      check_dir_path(new_path);
    }

    if (strchr (new_dir, '.'))
      return ConfigError("there should be no extension for the base outbound name");

    new_domain.dir  = xstrdup(new_dir);
    new_domain.path = xstrdup(new_path);
    if (wordcount == 4)
      new_domain.idomain = xstrdup(words[3]);
  }

  simplelist_add(&work_config.pDomains.linkpoint, &new_domain, sizeof(new_domain));

  return 1;
}

static int read_node_info (KEYWORD *key, int wordcount, char **words)
{
#define ARGNUM 6
  char *w[ARGNUM], *tmp, *pkt_pwd, *out_pwd;
  int   i, j;
  int   NR_flag = NR_USE_OLD, ND_flag = ND_USE_OLD, HC_flag = HC_USE_OLD,
        MD_flag = MD_USE_OLD, NP_flag = NP_USE_OLD, restrictIP = RIP_USE_OLD,
	IP_afamily = AF_USE_OLD;
#ifdef BW_LIM
  long bw_send = BW_DEF, bw_recv = BW_DEF;
#endif
  FTN_ADDR fa;
  FTN_NODE *pn;

  memset(w, 0, sizeof(w)); /* init by NULL's */
  i = 0;                   /* index in w[] */

  if (key->option1) /* defnode */
  {
    w[i++] = "0:0/0.0@defnode";
    work_config.havedefnode = 1;
  }

  for (j = 0; j < wordcount; j++)
  {
    tmp = words[j];

    if (tmp[0] == '-' && tmp[1] != 0)
    {
      if (STRICMP (tmp, "-md") == 0)
        MD_flag = MD_ON;
      else if (STRICMP (tmp, "-nomd") == 0)
        MD_flag = MD_OFF;
      else if (STRICMP (tmp, "-nr") == 0)
        NR_flag = NR_ON;
      else if (STRICMP (tmp, "-nd") == 0)
      {
        NR_flag = NR_ON;
        ND_flag = ND_ON;
      }
      else if (STRICMP (tmp, "-ip") == 0)
        restrictIP = RIP_ON;  /* allow matched or unresolvable */
      else if (STRICMP (tmp, "-sip") == 0)
        restrictIP = RIP_SIP; /* allow only resolved and matched */
      else if (STRICMP (tmp, "-crypt") == 0)
        ConfigError("obsolete %s option ignored", tmp);
      else if (STRICMP (tmp, "-hc") == 0)
        HC_flag = HC_ON;
      else if (STRICMP (tmp, "-nohc") == 0)
        HC_flag = HC_OFF;
      else if (STRICMP (tmp, "-noproxy") == 0)
        NP_flag = NP_ON;
#ifdef BW_LIM
      else if (STRICMP (tmp, "-bw") == 0) {
        char *s1, *s2, *ss;
        if (j == wordcount - 1) ConfigError("`-bw' option requires parameter");
        s1 = words[++j];
        s2 = strchr(s1, '/');
        if (s2) { *s2 = 0; s2++; }
        bw_send = bw_recv = parse_rate(s1, &ss);
        if (ss) ConfigError("syntax error near `%s'", ss);
        if (s2) {
          bw_recv = parse_rate(s2, &ss);
          if (ss) ConfigError("syntax error near `%s'", ss);
        }
      }
#endif
      else if (STRICMP (tmp, "-4") == 0)
	if (IP_afamily <= AF_UNSPEC)
	  IP_afamily = AF_INET;
	else
	  ConfigError("`-4' may only be used once and mutually exclusive with `-6'");
      else if (STRICMP (tmp, "-6") == 0)
#ifdef AF_INET6
	if (IP_afamily <= AF_UNSPEC)
	  IP_afamily = AF_INET6;
	else
	  ConfigError("`-6' may only be used once and mutually exclusive with `-4'");
#else
	ConfigError("IPv6 not supported in your version of BinkD");
#endif
      else
        return ConfigError("%s: unknown option for `node' keyword", tmp);
    }
    else if (i >= ARGNUM)
      return ConfigError("too many argumets for `node' keyword");
    else
    {   /* '-' will place default NULL */
      if (tmp[0] == '-')
        tmp = NULL;
      w[i++] = tmp;
    }
  }

  if (w[0] == NULL)
    return ConfigError("missing node address");
  if (!Config_ParseAddress (w[0], &fa))
    return 0;
  exp_ftnaddress (&fa, work_config.pAddr, work_config.nAddr, work_config.pDomains.first);

  if (w[2] && w[2][0] == 0)
    return ConfigError("empty password");
  if (w[3] && !isflvr (w[3][0]))
    return ConfigError("%s: incorrect flavour", w[3]);
  check_dir_path (w[4]);
  check_dir_path (w[5]);

  split_passwords(w[2], &pkt_pwd, &out_pwd);
  pn = add_node (&fa, w[1], w[2], pkt_pwd, out_pwd, (char)(w[3] ? w[3][0] : '-'), w[4], w[5],
            NR_flag, ND_flag, MD_flag, restrictIP, HC_flag, NP_flag, 
	    IP_afamily,
#ifdef BW_LIM
            bw_send, bw_recv,
#endif
            &work_config);
  if (pn) pn->listed = NL_NODE;

  return 1;
#undef ARGNUM
}

/*
 *  Gets hostname/portnumber for ``n''-th host in ``src'' string (1 ... N)
 *    <src> = <host> [ "," <src> ]
 *    <host> = "*"
 *    <host> = <hostname> [ ":" <service> ]
 *
 *  "*" will expand in corresponding domain name for ``fn''
 *                        (2:5047/13 --> "f13.n5047.z2.binkp.net.")
 *
 *  ``Host'' should contain at least BINKD_FQDNLEN bytes.
 *
 *  Returns 0 on error, -1 on EOF, 1 otherwise
 */
int get_host_and_port (int n, char *host, char **port, char *src, FTN_ADDR *fa, BINKD_CONFIG *config)
{
  int rc = 0;
  char *s = getwordx2 (src, n, 0, ",;", "");

  if (s)
  {
    /* support IPv6 address literals as in URL, i.e. [<ipv6 addr>]:<port> */
    int hostoff = 0;
    char *ipv6end = strchr (s, ']');
    char *t;

    if (ipv6end && *s == '[')
    {
      t = strchr (ipv6end, ':');
      *ipv6end = 0;
      hostoff = 1;
    }
    else
      t = strchr (s, ':');
       
    if (t)
      *t = 0;

    if (!strcmp (s, "*"))
      ftnaddress_to_domain (host, fa, config->pDomains.first, config->root_domain);
    else
      strnzcpy (host, s+hostoff, BINKD_FQDNLEN);

    if (!t)
    {
      *port = config->oport;
      rc = 1;
    }
    else if ((*port = find_port (t + 1)) != 0)
      rc = 1;

    free (s);
  }
  else
    rc = -1;
  return rc;
}

/*
 * Read a string (key->option1 == 0)
 * or a directory name (key->option1 == 'd')
 * or a file name (key->option1 == 'f')
 */
static int read_string (KEYWORD *key, int wordcount, char **words)
{
  struct stat sb;
  char *target = (char *) (key->var);
  char *w;

  if (!isArgCount(1, wordcount))
    return 0;

  strnzcpy (target, words[0], key->option1 == 0 ? key->option2 : MAXPATHLEN);

  if (key->option1 != 0)
  {
    w = target + strlen (target) - 1;
    while (w >= target && (*w == '/' || *w == '\\'))
    {
      if (key->option1 == 'f')
        return ConfigError("unexpected `%c' at the end of filename", *w);
      *(w--) = 0;
    }
    if (key->option1 == 'd' && (stat (target, &sb) == -1 ||
                                !(sb.st_mode & S_IFDIR)))
    {
      return ConfigError("%s: incorrect directory", target);
    }
  }

  return 1;
}

static int read_int (KEYWORD *key, int wordcount, char **words)
{
  int  *target = (int *) (key->var);
  char *s;

  if (!isArgCount(1, wordcount))
    return 0;

  for (s = words[0]; *s; s++)
  {
    if (isdigit(*s) || (s == words[0] && *s == '-'))
      continue;
    return ConfigNeedNumber(words[0]);
  }

  *target = atoi (words[0]);

  if ((key->option1 != DONT_CHECK && *target < key->option1) ||
      (key->option2 != DONT_CHECK && *target > key->option2))
    return ConfigError("%i: incorrect value", *target);

  return 1;
}

/*
 * When config is loaded for a first time, we have no log filename set yet
 * (it will be done only when config is checked and accepted).
 * It means that errors in configuration will be not logged even if
 * we have a 'log' directive in the beginning of configuration file.
 *
 * To avoid this, check: have active config ready or not? If not, 
 * reinit log immediately after any log-related directive has been processed.
 */

static void log_bootstrap(void)
{
  if (current_config == NULL)  /* do it only we have no active config */
  {
    /* 
     * Passing 'nolog' structures here can be dangerous, we still don't know
     * will config (and their memory) be accepted or free'd.
     * We can duplicate them in InitLog() but it'll be overkill.
     */
    InitLog(work_config.loglevel, work_config.conlog, work_config.logpath, NULL);
  }
}

static int read_log_string (KEYWORD *key, int wordcount, char **words)
{
  if (read_string(key, wordcount, words) == 0)
    return 0;
  log_bootstrap();
  return 1;
}

static int read_log_int (KEYWORD *key, int wordcount, char **words)
{
  if (read_int(key, wordcount, words) == 0)
    return 0;
  log_bootstrap();
  return 1;
}

static int read_bool (KEYWORD *key, int wordcount, char **words)
{
  if (wordcount != 0)
    return SyntaxError(key);

  (void) words;
  *(int *) (key->var) = 1;
  return 1;
}

static int read_time (KEYWORD *key, int wordcount, char **words)
{
  int *target = (int *) (key->var);
  char *s;
  int curnum = -1;

  if (!isArgCount(1, wordcount))
    return 0;

  *target = 0;
  s = words[0];
  if (*s == '-') s++;  /* tzoff can be negative */
  for (; *s; s++)
  {
    if (isdigit(*s))
    {
      if (curnum == -1) curnum = 0;
      else curnum *= 10;
      curnum += *s-'0';
      continue;
    }
    if (curnum == -1)
      return ConfigNeedTime(words[0]);
    switch (tolower(*s))
    {
      case 'w': curnum *= 7;
      case 'd': curnum *= 24;
      case 'h': curnum *= 60;
      case 'm': curnum *= 60;
      case 's': break;
      default:  return ConfigNeedTime(words[0]);
    }
    *target += curnum;
    curnum = -1;
  }
  if (curnum != -1)
    *target += curnum;
  if (words[0][0] == '-')
    *target = -*target;

  if ((key->option1 != DONT_CHECK && *target < key->option1) ||
      (key->option2 != DONT_CHECK && *target > key->option2))
    return ConfigError("%s: incorrect value", words[0]);

  return 1;
}

static int read_inboundcase (KEYWORD *key, int wordcount, char **words)
{
  enum inbcasetype *target = (enum inbcasetype *) (key->var);

  if (!isArgCount(1, wordcount))
    return 0;

  if (!STRICMP (words[0], "save"))
    *target = INB_SAVE;
  else if (!STRICMP (words[0], "upper"))
    *target = INB_UPPER;
  else if (!STRICMP (words[0], "lower"))
    *target = INB_LOWER;
  else if (!STRICMP (words[0], "mixed"))
    *target = INB_MIXED;
  else
    return SyntaxError(key);

  return 1;
}

static int read_dontsendempty (KEYWORD *key, int wordcount, char **words)
{
  enum dontsendemptytype *target = (enum dontsendemptytype *) (key->var);

  if (!isArgCount(1, wordcount))
    return 0;

  if (!STRICMP (words[0], "no"))
    *target = EMPTY_NO;
  else if (!STRICMP (words[0], "arcmail"))
    *target = EMPTY_ARCMAIL;
  else if (!STRICMP (words[0], "yes"))
    *target = EMPTY_ALL;
  else
    return SyntaxError(key);

  return 1;
}

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
static int read_syslog_facility (KEYWORD *key, int wordcount, char **words)
{
  int *target = (int *) (key->var);
  int i;

  if (isArgCount(1, wordcount))
    return 0;

  for (i = 0; facilitynames[i].c_name; ++i)
    if (!strcmp (facilitynames[i].c_name, words[0]))
    {
      *target = facilitynames[i].c_val;
      return 1;
    }
  return ConfigError("%s: incorrect facility name", words[0]);
}
#endif

static int read_rfrule (KEYWORD *key, int wordcount, char **words)
{
  RF_RULE new_entry;

  UNUSED_ARG(key);

  if (!isArgCount(2, wordcount))
    return 0;

  new_entry.from = xstrdup(words[0]);
  new_entry.to   = xstrdup(words[1]);
  simplelist_add(&work_config.rf_rules.linkpoint, &new_entry, sizeof(new_entry));

  return 1;
}

char *mask_test(char *s, struct maskchain *chain)
{
  struct maskchain *ps;

  for (ps = chain; ps; ps = ps->next)
    if (pmatch_ncase(ps->mask, s))
      return ps->mask;
  return NULL;
}

static int read_mask (KEYWORD *key, int wordcount, char **words)
{
  int i;
  struct maskchain new_entry;

  if (wordcount == 0)
    return SyntaxError(key);

  for (i = 0; i < wordcount; i++)
  {
    new_entry.mask = xstrdup(words[i]);
    simplelist_add(key->var, &new_entry, sizeof(new_entry));
  }

  return 1;
}

static int read_akachain (KEYWORD *key, int wordcount, char **words)
{
  struct akachain new_entry;
  int    type;
  char   *mask;

  if (!isArgCount(2, wordcount))
    return 0;

  if (!Config_ParseAddress(words[0], &new_entry.fa))  /* aka */
    return 0;
  exp_ftnaddress(&new_entry.fa, work_config.pAddr, work_config.nAddr, work_config.pDomains.first);

  mask = words[1];
  type = key->option1;
  if (*mask == '!')
  {
    mask++;
    type |= 0x80;
  }
  new_entry.type = type;
  new_entry.mask = xstrdup(mask);
  simplelist_add(key->var, &new_entry, sizeof(new_entry));

  return 1;
}

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
struct zrule *zrule_test(int type, char *s, struct zrule *root)
{
  struct zrule *ps = root;

  while (ps)
    if (s == NULL && type == ps->type) return ps;
    else if (pmatch_ncase(ps->mask, s)) return (type == ps->type ? ps : NULL);
    else ps = ps->next;
  return NULL;
}

static int read_zrule (KEYWORD *key, int wordcount, char **words)
{
  struct zrule new_entry;
  int    i;

  if (wordcount == 0) return ConfigError("at least one mask expected");

  new_entry.type = key->option1;
  for (i = 0; i < wordcount; i++) {
    new_entry.mask = xstrdup(words[i]);
    simplelist_add(key->var, &new_entry, sizeof(new_entry));
  }
  return 1;
}
#endif

static addrtype parse_addrtype(char *w)
{
  if (STRICMP (w, "all") == 0) return A_ALL;
  else if (STRICMP (w, "secure") == 0) return A_PROT;
  else if (STRICMP (w, "unsecure") == 0) return A_UNPROT;
  else if (STRICMP (w, "listed") == 0) return A_LST;
  else if (STRICMP (w, "unlisted") == 0) return A_UNLST;
  else return 0;
}

/* skip [all|listed|unlisted|secure|unsecure] [!]<size>|- <mask>... */
static int read_skip (KEYWORD *key, int wordcount, char **words)
{
  int i, destr, maskonly = 0;
  boff_t sz = 0;
  addrtype at = A_ALL;
  struct skipchain new_entry;

  if ((destr = key->option1) != 0)
    Log(1, "%s: line %d: warning: option skipmask is obsolete, use skip instead", current_path, current_line);
  for (i = 0; i < wordcount; i++)
  {
    char *w = words[i];

    if (i == 0 && isalpha(*w) && key->option1 == 0)
    {
      if ( !(at = parse_addrtype(w)) )
        return ConfigError("incorrect address type '%s'", w);
      continue;
    }
    if (i < 2 && maskonly == 0 && key->option1 == 0)
    {
      if (*w == '-' && w[1] == 0) { sz = -1; continue; }
      if (*w == '!') { destr = 1; w++; }
      if (!isdigit(*w))
        return ConfigNeedNumber(w);
      sz = (boff_t)strtoumax(w, NULL, 10);
      maskonly = 1; /* size detected, only masks are allowed further */
      continue;
    }
    /* Add new entry */
    new_entry.mask  = xstrdup(w);
    new_entry.size  = (sz > 0 ? sz << 10 : sz);
    new_entry.atype = at;
    new_entry.destr = destr;
    simplelist_add(&work_config.skipmask.linkpoint, &new_entry, sizeof(new_entry));

    maskonly = 2; /* at least one filemask present */
  }

  if (maskonly != 2)
    return ConfigError("expecting at least one filemask");

  return 1;
}

/* check-pkthdr [all|secure|unsecure|listed|unlisted] <ext> */
static int read_check_pkthdr (KEYWORD *key, int wordcount, char **words)
{
  char *w;

  if (wordcount == 0)
    return SyntaxError(key);

  w = words[0];
  if ((work_config.pkthdr_type = parse_addrtype(w)) != 0)
  {
    w = words[1];
    if (!isArgCount(2, wordcount))
      return 0;
  }
  else
  {
    work_config.pkthdr_type = A_ALL;
    if (!isArgCount(1, wordcount))
      return 0;
  }

  if (*w == '.')
    w++;
  xfree(work_config.pkthdr_bad);
  work_config.pkthdr_bad = xstrdup(w);

  return 1;
}
#ifdef BW_LIM
/* parse `<rate>[kM%]|-' string
   return in err pointer to error, NULL if no error */
long parse_rate (char *w, char **err)
{
  long rate;
  char *ss;

  if (err) *err = NULL;
  if (*w == '-' && w[1] == 0) return 0;
  rate = strtoul(w, &ss, 10);
  if (ss) {
    switch (*ss) {
      case 'k': case 'K': rate <<= 10; ss++; break;
      case 'm': case 'M': rate <<= 20; ss++; break;
      case '%': rate = -rate; ss++; break;
    }
    if (*ss) { if (err) *err = ss; return 0; }
  }
  return rate;
}
/* limit-rate [all|listed|unlisted|secure|unsecure] <rate>[kM%]|- <mask>... */
static int read_rate (KEYWORD *key, int wordcount, char **words)
{
  int i, rate = 0, maskonly = 0;
  char *ss;
  addrtype at = A_ALL;
  struct ratechain new_entry;

  UNUSED_ARG(key);

  for (i = 0; i < wordcount; i++)
  {
    char *w = words[i];

    if (i == 0 && isalpha(*w))
    {
      if ( !(at = parse_addrtype(w)) )
        return ConfigError("incorrect address type '%s'", w);
      continue;
    }
    if (i < 2 && maskonly == 0)
    {
      rate = parse_rate(w, &ss);
      if (ss) return ConfigError("syntax error near '%s'", ss);
/*
      if (*w == '-' && w[1] == 0) { rate = 0; continue; }
      if (!isdigit(*w))
        return ConfigNeedNumber(w);
      rate = strtoul(w, &ss, 10);
      if (ss) {
        switch (*ss) {
          case 'k': case 'K': rate <<= 10; ss++; break;
          case 'm': case 'M': rate <<= 20; ss++; break;
          case '%': rate = -rate; ss++; break;
        }
        if (*ss) return ConfigError("incorrect char in rate value '%c'", *ss);
      }
*/
      maskonly = 1; /* size detected, only masks are allowed further */
      continue;
    }
    /* Add new entry */
    new_entry.mask  = xstrdup(w);
    new_entry.rate  = rate;
    new_entry.atype = at;
    simplelist_add(&work_config.rates.linkpoint, &new_entry, sizeof(new_entry));

    maskonly = 2; /* at least one filemask present */
  }

  if (maskonly != 2)
    return ConfigError("expecting at least one filemask");

  return 1;
}
#endif

static int read_port (KEYWORD *key, int wordcount, char **words)
{
  char *target = (char *)(key->var);
  char *ps = NULL;

  if (!isArgCount(1, wordcount))
    return 0;

  if ((ps = find_port (words[0])) == NULL)
    return ConfigError("%s: bad port number", words[0]);

  snprintf(target, MAXSERVNAME+1, "%s", ps);

  return 1;
}

static int read_flag_exec_info (KEYWORD *key, int wordcount, char **words)
{
  EVT_FLAG new_event;
  char     *path;
  int      i;
  char     **body;

  if (wordcount < 2)
    return SyntaxError(key);

  path = words[0];
  for (i = 1; i < wordcount; i++)
  {
    memset(&new_event, 0, sizeof(new_event));

    if (key->option1 == 'f')
      body = &(new_event.path);
    else if (key->option1 == 'e')
      body = &(new_event.command);
    else
      continue; /* should never happens */
    if (*path == '!')
    {
      new_event.imm = 1;
      *body = xstrdup(path + 1);
    }
    else
      *body = xstrdup(path);
    new_event.pattern = xstrdup(words[i]);
    /* strlower (new_event->pattern); */

    simplelist_add(&work_config.evt_flags.linkpoint, &new_event, sizeof(new_event));
  }

  return 1;
}

#if 0 /* def HTTPS */
static int read_proxy (KEYWORD *key, int wordcount, char **words)
{
  char *p, *proxy_user, *proxy_password;
  struct proxy_info pro;

  if (!isArgCount(1, wordcount))
    return 0;

  /* by default, username and password are empty */
  proxy_user = proxy_password = "";
  if (key->option1 == 'p')
  {
    pro.method = CMETHOD_HTTP;
    pro.port   = 3128;
  }
  else
  {
    pro.method = CMETHOD_SOCKS4;
    pro.port   = 1080;
  }

  /*
   * !!! TODO !!! Get default port number from service name ("squid", "socks")
   */

  if ((p = strchr(words[0], '/')) != NULL)
  {
    /* Username/password? Set socks mode to socks5 */
    if (pro.method == CMETHOD_SOCKS4)
      pro.method = CMETHOD_SOCKS5;
    /* parse username and password */
    *p++ = 0;
    proxy_user = p;
    if ((p = strchr(p, '/')) != NULL)
    {
      *p++ = 0;
      proxy_password = p;
    }
    /* Note: NTLM auth kept as part of password and handled by connector */
  }
    /* port number */
  if ((p = strchr(words[0], ':')) != NULL)
  {
    *p++ = 0;
    if (*p)  /* avoid getting "default binkp" port */
    {
      pro.port = find_port(p); /* return 0 on error */
      if (pro.port == 0)
        return 0;
    }
  }

  pro.host     = xstrdup(words[0]);
  pro.username = xstrdup(proxy_user);
  pro.password = xstrdup(proxy_password);
  simplelist_add(&work_config.proxylist, &pro, sizeof(pro));
  return 1;
}
#endif

static int print_node_info_1 (FTN_NODE *fn, void *arg)
{
  char szfa[FTN_ADDR_SZ + 1];
  char *pwd;
  size_t pwd_len;

  UNUSED_ARG(arg);
  ftnaddress_to_str (szfa, &fn->fa);
  pwd_len = strlen(fn->pwd);
  if (fn->pkt_pwd) pwd_len += strlen(fn->pkt_pwd)+1; else pwd_len += 2;
  if (fn->out_pwd) pwd_len += strlen(fn->out_pwd)+1; else pwd_len += 2;
  pwd = calloc (1, pwd_len+1);
  strcpy(pwd, fn->pwd);
  if (fn->pkt_pwd != (char*)&(fn->pwd) || fn->out_pwd != (char*)&(fn->pwd)) {
    strcat(strcat(pwd, ","), (fn->pkt_pwd) ? fn->pkt_pwd : "-");
    strcat(strcat(pwd, ","), (fn->out_pwd) ? fn->out_pwd : "-");
  }
  printf("\n    %-20.20s %s %s %c %s %s%s%s%s%s%s%s%s%s%s",
         szfa, fn->hosts ? fn->hosts : "-", pwd,
         fn->obox_flvr, fn->obox ? fn->obox : "-",
         fn->ibox ? fn->ibox : "-",
         (fn->NR_flag == NR_ON)  ? " -nr" : "",
         (fn->ND_flag == ND_ON)  ? " -nd" : "",
         (fn->MD_flag == MD_ON)  ? " -md" : "",
         (fn->MD_flag == MD_OFF) ? " -nomd" : "",
         (fn->HC_flag == HC_ON)  ? " -hc" : "",
         (fn->HC_flag == HC_OFF) ? " -nohc" : "",
         (fn->NP_flag == NP_ON)  ? " -noproxy" : "",
         (fn->restrictIP == RIP_ON)  ? " -ip" : "",
         (fn->restrictIP == RIP_SIP) ? " -sip" : ""
        );
  free (pwd);
#ifdef BW_LIM
  if (fn->bw_send != BW_DEF || fn->bw_recv != BW_DEF) {
    printf(" -bw %s", describe_rate(fn->bw_send));
    printf("/%s", describe_rate(fn->bw_recv));
  }
#endif
  return 0;
}

static char *describe_addrtype(addrtype a)
{
  switch (a)
  {
  case A_ALL:    return "all";
  case A_PROT:   return "secure";
  case A_UNPROT: return "unsecure";
  case A_LST:    return "listed";
  case A_UNLST:  return "unlisted";
  }
  return "???";
}
#ifdef BW_LIM
char *describe_rate(long rate)
{
  static char buf[12];
  int c;
  if (rate == 0) return "-";
  else if (rate < 0) c = sprintf(buf, "%ld%%", -rate);
  else if (rate >= (1 << 20) && (rate & ((1 << 20) - 1)) == 0) c = sprintf(buf, "%ldM", rate >> 20);
  else if (rate >= (1 << 10) && (rate & ((1 << 10) - 1)) == 0) c = sprintf(buf, "%ldk", rate >> 10);
  else c = sprintf(buf, "%ld", rate);
  buf[c] = 0;
  return buf;
}
#endif
void debug_readcfg (void)
{
  KEYWORD *k;
  char szfa[FTN_ADDR_SZ + 1];


  for (k = keywords; k->key; k++)
  {
    printf("%-24s ", k->key);
    if (k->callback == read_string || k->callback == read_log_string)
      printf("\"%s\"", (char *)k->var);
    else if (k->callback == read_int || k->callback == read_port || k->callback == read_log_int)
      printf("%d", *(int *)(k->var));
    else if (k->callback == read_bool)
      printf(*(int *)(k->var) ? "[yes]" : "<not defined>");
    else if (k->callback == read_time)
    {
      int i = *(int *)(k->var);
      if (i<0)
      {
        printf("-");
        i = -i;
      }
      if (i >= 7*24*60*60)
      {
        printf("%dw", i/7*24*60*60);
        i %= 7*24*60*60;
      }
      if (i >= 24*60*60)
      {
        printf("%dd", i/24*60*60);
        i %= 24*60*60;
      }
      if (i >= 60*60)
      {
        printf("%dh", i/60*60);
        i %= 60*60;
      }
      if (i >= 60)
      {
        printf("%dm", i/60);
        i %= 60;
      }
      if (i > 0 || *(int *)(k->var) == 0)
        printf("%ds", i);
    }
    else if (k->callback == include)
    {
      struct conflist_type *c;
      for (c = work_config.config_list.first; c; c = c->next)
        printf("\n    %s", c->path);
    }
    else if (k->callback == read_domain_info)
    {
      FTN_DOMAIN *c;
      for (c = work_config.pDomains.first; c; c = c->next)
        if (c->alias4)
          printf("\n    %s alias-for %s", c->name, c->alias4->name);
        else
          printf("\n    %s %s/%s %d", c->name, c->path, c->dir, c->z[0]);
    }
    else if (k->callback == read_aka_list)
    {
      int  i;

      for (i = 0; i < work_config.nAddr; i++)
      {
        ftnaddress_to_str (szfa, work_config.pAddr + i);
        printf("\n    %s", szfa);
      }
    }
    else if (k->callback == read_node_info)
    {
      if (k->option1 == 0) /* not defnode */
        foreach_node (print_node_info_1, 0, &work_config);
      else
        printf(work_config.havedefnode ? "[see 0:0/0@defnode]" : "<not defined>");
    }
    else if (k->callback == read_flag_exec_info)
    {
      EVT_FLAG *c;

      for (c = work_config.evt_flags.first; c; c = c->next)
      {
        char *s = (k->option1 == 'f' ? c->path : c->command);
        if (s)
          printf("\n    %s\"%s\" on \"%s\"", (c->imm ? "[Imm] " : ""),
                 s, c->pattern);
      }
    }
    else if (k->callback == read_rfrule)
    {
      RF_RULE *c;

      for (c = work_config.rf_rules.first; c; c = c->next)
        printf("\n    \"%s\" => \"%s\"", c->from, c->to);
    }
    else if (k->callback == read_inboundcase)
    {
      switch (work_config.inboundcase)
      {
      case INB_SAVE:  printf("save");  break;
      case INB_UPPER: printf("upper"); break;
      case INB_LOWER: printf("lower"); break;
      case INB_MIXED: printf("mixed"); break;
      default:        printf("???");   break;
      }
    }
    else if (k->callback == read_skip)
    {
      struct skipchain *sk;
      for (sk = work_config.skipmask.first; sk; sk = sk->next)
        printf("\n    %s %c%ld \"%s\"", describe_addrtype(sk->atype),
               (sk->destr ? '!' : ' '),
               (long)(sk->size < 0 ? sk->size : sk->size >> 10),
               sk->mask);
    }
    else if (k->callback == read_mask)
    {
      struct maskchain *p;

      for (p = ((TYPE_LIST(maskchain) *)k->var)->first; p; p = p->next)
        printf("\n    \"%s\"", p->mask);
    }
    else if (k->callback == read_akachain)
    {
      if (k->option1 == ACT_HIDE)
        printf("\n    [see present-aka]");
      else
      {
        struct akachain *p;

        for (p = ((TYPE_LIST(akachain) *)k->var)->first; p; p = p->next)
        {
          ftnaddress_to_str(szfa, &p->fa);
          printf("\n    %s %s %c%s", ((p->type & 0x7F) == ACT_HIDE ? "hide-aka" : "present-aka"),
                 szfa, (p->type & 0x80 ? '!' : ' '), p->mask);
        }
      }
    }
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
    else if (k->callback == read_zrule)
    {
      if (k->option1 == ZRULE_DENY)
        printf("\n    [see zallow]");
      else
      {
        struct zrule *p;

        for (p = ((TYPE_LIST(zrule) *)k->var)->first; p; p = p->next)
        {
          printf("\n    %s %s", (p->type == ZRULE_DENY ? "zdeny" : "zallow"), p->mask);
        }
      }
    }
#endif
#ifdef BW_LIM
    else if (k->callback == read_rate)
    {
      struct ratechain *sk;
      for (sk = work_config.rates.first; sk; sk = sk->next) {
          printf("\n    %s %s \"%s\"", describe_addrtype(sk->atype), describe_rate(sk->rate), sk->mask);
      }
    }
#endif
    else if (k->callback == read_check_pkthdr)
    {
      if (work_config.pkthdr_type == 0)
        printf("<disabled>");
      else
        printf("%s %s", describe_addrtype(work_config.pkthdr_type), work_config.pkthdr_bad);
    }
    else if (k->callback == read_shares)
    {
      SHARED_CHAIN   *ch;
      FTN_ADDR_CHAIN *fch;

      for (ch = work_config.shares.first; ch; ch = ch->next)
      {
        ftnaddress_to_str(szfa, &ch->sha);
        printf("\n    %s", szfa);
        for (fch = ch->sfa.first; fch; fch = fch->next)
        {
          ftnaddress_to_str(szfa, &fch->fa);
          printf(" %s", szfa);
        }
      }
    }
#if 0 /* def HTTPS */
     else if (k->callback == read_proxy)
     {
     struct proxy_info *prox;

     if (k->option1 == 'p')
     for (prox = work_config.proxylist.first; prox; prox=prox->next)
     printf("\n    %s %s:%d user=`%s' password=`%s'",
     connect_method_name[prox->method],
     prox->host, prox->port, prox->username, prox->password);
     else
     printf("[see `proxy']");
     }
#endif
    else
      printf("[this item cannot be displayed]");
    printf("\n");
  }
}

/*
 * Read shared akas info:
 * share  share_address  node1 [node2 [...]]
 */
static int read_shares (KEYWORD *key, int wordcount, char **words)
{
  int i;
  SHARED_CHAIN   chn;
  FTN_ADDR_CHAIN fchn;
  KEYWORD        *k;

  if (wordcount < 2)
    return SyntaxError(key);

  if (!Config_ParseAddress(words[0], &chn.sha))
    return 0;
  exp_ftnaddress(&chn.sha, work_config.pAddr, work_config.nAddr, work_config.pDomains.first);

  /* To scan outgoing mails to shared node
   * `node share_addr' string is simulated
   */
  for (k = keywords; k->key; k++)
    if (!STRICMP (k->key, "node"))
    {
      /* Our first argument passed to read_node_info as-is, with argc = 1 */
      if (read_node_info(k, 1, words) == 0)
        return 0;
      break;
    }

  memset(&chn.sfa, 0, sizeof(chn.sfa)); /* clear list */
  for (i = 1; i < wordcount; ++i)
  {
    if (!Config_ParseAddress (words[i], &fchn.fa))
    {
      destroy_shares(&chn);
      return 0;
    }
    exp_ftnaddress(&fchn.fa, work_config.pAddr, work_config.nAddr, work_config.pDomains.first);
    simplelist_add(&chn.sfa.linkpoint, &fchn, sizeof(fchn));
  }

  simplelist_add(&work_config.shares.linkpoint, &chn, sizeof(chn));

  return 1;
}

#ifdef WITH_PERL
static int read_perlvar (KEYWORD *key, int wordcount, char **words)
{
  struct perl_var new_entry;

  if (wordcount != 2)
    return SyntaxError(key);

  new_entry.name = xstrdup(words[0]);
  new_entry.val  = xstrdup(words[1]);
  simplelist_add(key->var, &new_entry, sizeof(new_entry));
  return 1;
}
#endif


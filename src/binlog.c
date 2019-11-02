/*--------------------------------------------------------------------*/
/*       B i n L o g . c                                              */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Binary log implementation                                    */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*       Copyright (c) 1997 by Fydodor Ustinov                        */
/*                             FIDONet 2:5020/79                      */
/*       FrontDoor stat (c) 1997 A&T Software                         */
/*                             2:5023/8@fidonet                       */
/*                                                                    */
/*  This program is  free software;  you can  redistribute it and/or  */
/*  modify it  under  the terms of the GNU General Public License as  */ 
/*  published  by the  Free Software Foundation; either version 2 of  */
/*  the License, or (at your option) any later version. See COPYING.  */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          RCS Information                           */
/*--------------------------------------------------------------------*/

/*
 * $Id: binlog.c,v 2.13.2.1 2014/08/09 15:17:41 gul Exp $
 *
 * Revision history:
 * $Log: binlog.c,v $
 * Revision 2.13.2.1  2014/08/09 15:17:41  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.13  2010/05/22 08:11:30  gul
 * Call after_session() hook after removing bsy
 *
 * Revision 2.12  2004/10/08 08:30:30  gul
 * binlog rev.2.11 bugfix
 *
 * Revision 2.11  2004/08/04 13:06:58  gul
 * Binlogs writes correctly now independent on bytes order and alignment
 *
 * Revision 2.10  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.9  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.8  2003/08/17 19:07:10  gul
 * Fix typo
 *
 * Revision 2.7  2003/08/17 10:38:55  gul
 * Return semaphoring for log and binlog
 *
 * Revision 2.6  2003/08/16 09:47:25  gul
 * Autodetect tzoff if not specified
 *
 * Revision 2.5  2003/08/16 09:08:33  gul
 * Binlog semaphoring removed
 *
 * Revision 2.4  2003/04/02 13:12:57  gul
 * Try to use workaround for buggy windows time functions (timezone)
 *
 * Revision 2.3  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.2  2003/03/11 09:21:29  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.1  2003/03/05 13:21:50  gul
 * Fix warnings
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 0.02  1997/07/11  11:40:19  maxb
 *      Added FrontDoor aka Bink/+ statistics
 *
 * Revision 0.01  1997/05/25  18:36:00  ufm
 *      First revision
 *
 */

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <stdio.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "sys.h"
#include "readcfg.h"
#include "iphdr.h"
#include "protoco2.h"
#include "binlog.h"
#include "tools.h"
#include "sem.h"

/* Write 16-bit integer to file in intel bytes order */
static int fput16(u16 arg, FILE *file)
{
	if (fputc(arg & 0xff, file) == EOF) return EOF;
	return fputc(arg >> 8, file);
}

/* Write 32-bit integer to file in intel bytes order */
static int fput32(u32 arg, FILE *file)
{
	if (fput16((u16)(arg & 0xffff), file) == EOF) return EOF;
	return fput16((u16)(arg/0x10000), file);
}

/*--------------------------------------------------------------------*/
/*    static void TLogStat (char*, STATE*, char*, int)                */
/*                                                                    */
/*    Add record to T-Mail style binary log.                          */
/*--------------------------------------------------------------------*/

static void TLogStat (int status, STATE *state, char *binlogpath, int tzoff)
{
	struct {
		u16    fZone;
		u16    fNet;
		u16    fNode;
		u16    fPoint;
		u32    fSTime;
		u32    fLTime;
		u32    fBReceive;
		u32    fBSent;
		u8     fFReceive;
		u8     fFSent;
		u16    fStatus;
	} TS;

	FILE *fl;

	if (binlogpath[0]) {
		TS.fStatus = 0;

		if (state->to) {
			TS.fZone = (u16)state->to->fa.z;
			TS.fNet = (u16)state->to->fa.net;
			TS.fNode = (u16)state->to->fa.node;
			TS.fPoint = (u16)state->to->fa.p;
			TS.fStatus = 1;
		} else if (state->fa) {
			TS.fZone = (u16)state->fa->z;
			TS.fNet = (u16)state->fa->net;
			TS.fNode = (u16)state->fa->node;
			TS.fPoint = (u16)state->fa->p;
			TS.fStatus = 2;
		} else {
			TS.fZone = 0;
			TS.fNet = 0;
			TS.fNode = 0;
			TS.fPoint = 0;
			TS.fStatus = 0;
		}
		TS.fBReceive = (u32)state->bytes_rcvd;
		TS.fBSent = (u32)state->bytes_sent;
		TS.fFReceive = (u8)state->files_rcvd;
		TS.fFSent = (u8)state->files_sent;
		TS.fSTime = (u32)(state->start_time + tz_off(state->start_time, tzoff)*60);
		TS.fLTime = (u32)(safe_time() - state->start_time);
		if (status) {
			TS.fStatus |= 3;
		}
		LockSem(&blsem);
		if ((fl = fopen(binlogpath,"ab")) != NULL) {
			/* fwrite(&TS, sizeof(TS), 1, fl); */
			/* FIXME: check retcode and set original file size
			 * if write failed */
			fput16(TS.fZone,     fl);
			fput16(TS.fNet,      fl);
			fput16(TS.fNode,     fl);
			fput16(TS.fPoint,    fl);
			fput32(TS.fSTime,    fl);
			fput32(TS.fLTime,    fl);
			fput32(TS.fBReceive, fl);
			fput32(TS.fBSent,    fl);
			fputc (TS.fFReceive, fl);
			fputc (TS.fFSent,    fl);
			fput16(TS.fStatus,   fl);
			fclose(fl);
			ReleaseSem(&blsem);
		} else {
			ReleaseSem(&blsem);
			Log(1,"unable to open binary log file `%s'",binlogpath);
		}
	}

}

/*--------------------------------------------------------------------*/
/*    static void FDLogStat (STATE*, char*, char*, int)               */
/*                                                                    */
/*    Add record to FrontDoor-style binary log.                       */
/*--------------------------------------------------------------------*/

static void FDLogStat (STATE *state, char *fdinhist, char *fdouthist, int tzoff)
{
	struct
	{
		u16    Zone;
		u16    Net;
		u16    Node;
		u16    Point;
		char   Domain[16];
		u32    TimeStart;
		u32    TimeEnd;
		char   StationName[32];
		char   StationLoc[40];
		u32    Received;
		u32    Sent;
		u32    Cost;
	} std;

	FILE *fp;
	time_t t;

	if (!state->fa || ((state->to && !fdouthist[0]) || (!state->to && !fdinhist[0])))
            return; /* nothing to do */

	t = safe_time();
	std.TimeStart = (u32)(state->start_time + tz_off(state->start_time, tzoff)*60);
	std.TimeEnd = (u32)(t + tz_off(t, tzoff)*60);
	std.Zone = (u16)state->fa->z;
	std.Net = (u16)state->fa->net;
	std.Node = (u16)state->fa->node;
	std.Point = (u16)state->fa->p;
	strnzcpy (std.Domain, state->fa->domain, sizeof (std.Domain));
	strnzcpy (std.StationName, state->sysname, sizeof (std.StationName));
	strnzcpy (std.StationLoc, state->location, sizeof (std.StationLoc));
	std.Received = (u32)(state->bytes_rcvd);
	std.Sent = (u32)(state->bytes_sent);
	std.Cost = 0; /* Let it be free :) */

	LockSem(&blsem);
	if ((fp = fopen ( state->to ? fdouthist : fdinhist, "ab" )) != NULL)
	{
		/* fwrite ( &std, (size_t) sizeof(std), (size_t) 1, fp); */
		/* FIXME: check retcode and set original file size
		 * if write failed */
		fput16(std.Zone,       fp);
		fput16(std.Net,        fp);
		fput16(std.Node,       fp);
		fput16(std.Point,      fp);
		fwrite(std.Domain,     sizeof(std.Domain), 1, fp);
		fput32(std.TimeStart,  fp);
		fput32(std.TimeEnd,    fp);
		fwrite(std.StationName,sizeof(std.StationName), 1, fp);
		fwrite(std.StationLoc, sizeof(std.StationLoc),  1, fp);
		fput32(std.Received,   fp);
		fput32(std.Sent,       fp);
		fput32(std.Cost,       fp);
		fclose( fp );
		ReleaseSem(&blsem);
	}
	else
	{
		ReleaseSem(&blsem);
		Log (1, "failed to write to %s", (state->to ? fdouthist : fdinhist));
	}
}

/*--------------------------------------------------------------------*/
/*    void BinLogStat (char*, STATE*, BINKD_CONFIG*)                  */
/*                                                                    */
/*    Add record to binary logs.                                      */
/*--------------------------------------------------------------------*/

void BinLogStat (int status, STATE *state, BINKD_CONFIG *config)
{
	TLogStat (status, state, config->binlogpath, config->tzoff);
	FDLogStat (state, config->fdinhist, config->fdouthist, config->tzoff);
}


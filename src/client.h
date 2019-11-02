/*
 * $Id: client.h,v 2.0 2001/01/10 12:12:37 gul Exp $
 *
 * $Log: client.h,v $
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _client_h
#define _client_h

/*
 * Scans queue, makes outbound ``call'', than calls protocol()
 */
void clientmgr(void *arg);

#endif

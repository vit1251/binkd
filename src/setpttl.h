/*
 *  setpttl.h -- Interface to setproctitle() and initsetproctitle()
 *
 *  setpttl.h is a part of binkd project
 *
 *  Copyright (C) 1998  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */
/*
 * $Id: setpttl.h,v 2.0 2001/01/10 12:12:39 gul Exp $
 *
 * $Log: setpttl.h,v $
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 *
 */

#ifndef _setpttl_h
#define _setpttl_h

void initsetproctitle (int argc, char **argv, char **envp);
void setproctitle (const char *fmt,...);

#endif

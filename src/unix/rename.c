/*
 * $Id: rename.c,v 2.0 2001/01/10 12:12:40 gul Exp $
 *
 * Revision history:
 * $Log: rename.c,v $
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

extern void Log (int lev, char *s,...);

int o_rename (const char *from, const char *to)
{
  int h, saved_errno;

  if ((h = open (to, O_CREAT | O_EXCL, 0666)) == -1)
    return -1;
  close (h);
  if (rename (from, to) == -1)
  {
    saved_errno = errno;
    unlink (to);
    errno = saved_errno;
    return -1;
  }
  return 0;
}

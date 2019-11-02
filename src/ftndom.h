/*
 *  ftndom.h -- Source to handle FTN Domains
 *
 *  ftndom.h is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id: ftndom.h,v 2.6 2003/10/29 21:08:38 gul Exp $
 *
 * $Log: ftndom.h,v $
 * Revision 2.6  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.5  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.4  2003/08/24 19:42:08  gul
 * Get FTN-domain from matched zone in exp_ftnaddress()
 *
 * Revision 2.3  2003/08/18 17:19:13  stream
 * Partially implemented new configuration parser logic (required for config reload)
 *
 * Revision 2.2  2003/03/10 10:57:45  gul
 * Extern declarations moved to header files
 *
 * Revision 2.1  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1996/12/29  09:41:28  mff
 * Initial revision
 *
 */
#ifndef _ftndomain_h
#define _ftndomain_h

#include "btypes.h"

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name, FTN_DOMAIN *pDomains);

/*
 * Returns the matched domain by zone
 */
char *get_matched_domain (int zone, FTN_ADDR *pAddr, int nAddr, FTN_DOMAIN *pDomains);

#endif

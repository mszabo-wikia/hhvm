/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include <string.h>
#include "hphp/runtime/ext/gd/libgd/gd.h"

#define MAX_XBM_LINE_SIZE 255

/* {{{ gdImagePtr gdImageCreateFromXbm */
gdImagePtr gdImageCreateFromXbm(FILE * fd)
{
  char fline[MAX_XBM_LINE_SIZE];
  char iname[MAX_XBM_LINE_SIZE];
  char *type;
  int value;
  unsigned int width = 0, height = 0;
  int fail = 0;
  int max_bit = 0;

  gdImagePtr im;
  int bytes = 0, i;
  int bit, x = 0, y = 0;
  int ch;
  char h[8];
  unsigned int b;

  rewind(fd);
  while (fgets(fline, MAX_XBM_LINE_SIZE, fd)) {
    fline[MAX_XBM_LINE_SIZE-1] = '\0';
    if (strlen(fline) == MAX_XBM_LINE_SIZE-1) {
      return 0;
    }
    if (sscanf(fline, "#define %s %d", iname, &value) == 2) {
      if (!(type = strrchr(iname, '_'))) {
        type = iname;
      } else {
        type++;
      }

      if (!strcmp("width", type)) {
        width = (unsigned int) value;
      }
      if (!strcmp("height", type)) {
        height = (unsigned int) value;
      }
    } else {
      if ( sscanf(fline, "static unsigned char %s = {", iname) == 1
        || sscanf(fline, "static char %s = {", iname) == 1)
      {
        max_bit = 128;
      } else if (sscanf(fline, "static unsigned short %s = {", iname) == 1
          || sscanf(fline, "static short %s = {", iname) == 1)
      {
        max_bit = 32768;
      }
      if (max_bit) {
        bytes = (width * height / 8) + 1;
        if (!bytes) {
          return 0;
        }
        if (!(type = strrchr(iname, '_'))) {
          type = iname;
        } else {
          type++;
        }
        if (!strcmp("bits[]", type)) {
          break;
        }
      }
    }
  }
  if (!bytes || !max_bit) {
    return 0;
  }

  if(!(im = gdImageCreate(width, height))) {
    return 0;
  }
  gdImageColorAllocate(im, 255, 255, 255);
  gdImageColorAllocate(im, 0, 0, 0);
  h[2] = '\0';
  h[4] = '\0';
  for (i = 0; i < bytes; i++) {
    while (1) {
      if ((ch=getc(fd)) == EOF) {
        fail = 1;
        break;
      }
      if (ch == 'x') {
        break;
      }
    }
    if (fail) {
      break;
    }
    /* Get hex value */
    if ((ch=getc(fd)) == EOF) {
      break;
    }
    h[0] = ch;
    if ((ch=getc(fd)) == EOF) {
      break;
    }
    h[1] = ch;
    if (max_bit == 32768) {
      if ((ch=getc(fd)) == EOF) {
        break;
      }
      h[2] = ch;
      if ((ch=getc(fd)) == EOF) {
        break;
      }
      h[3] = ch;
    }
    if (sscanf(h, "%x", &b) != 1) {
      php_gd_error("invalid XBM");
      gdImageDestroy(im);
      return 0;
    }
    for (bit = 1; bit <= max_bit; bit = bit << 1) {
      gdImageSetPixel(im, x++, y, (b & bit) ? 1 : 0);
      if (x == im->sx) {
        x = 0;
        y++;
        if (y == im->sy) {
          return im;
        }
        break;
      }
    }
  }

  php_gd_error("EOF before image was complete");
  gdImageDestroy(im);
  return 0;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

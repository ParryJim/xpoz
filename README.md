# xpoz
Light weight graphics for cloud computing

A simple set of commands may be included in a useful ("scientific") program to
generate a display list that is easily available for interpretation by a simple
website to produce an html canvas display.

NOTE:  THERE IS NO SECURITY OR PRIVACY OFFERED


FILE ORGANIZATION:

This is one way to do things in a Google cloud linux instance.
<br/>(1) Put a copy of xpoz.c & xpoz.h in your home directory.
<br/>(2) In the directory with your scientific program include a make file
similar to the demo Makefile included in this repository.  Note that
the -iquotes option should be adjusted to indicate your home directory
or the location you selected to place xpoz.c & xpoz.h.  If you want to
keep everything in the same directory that will work too but it is less
flexible in terms of future uses.
<br/>(3) Create a mount point named "xpoz" and attach a bucket that you own to it
using gcsfuse.  (See https://cloud.google.com/storage/docs/gcs-fuse).
<br/>(4) If you copy the demo program into your "scientific" directory then you can
make it and run it.  
Use a browser to visit xpoz.org where you can enter your bucket name and
the title "Demo" to see your graphic results.
<br/>(5) In your own scientific code #include "xpoz.h" and use some of the graphical commands
including title() and view() or page() which will output the display list to the bucket
for any website to interpret (e.g. http://xpoz.org).
The simple php code for the web page is available in this repository.


NOTES ABOUT SIMPLE GRAPHICS:

Everything is pixel based.  The HTML canvas used by the http://xpoz.org website
is "first quadrant", i.e. (0, 0) is in the lower left corner.  The default
canvas size is 1001 by 1001 pixels.  The system is stateful with respect to
color and font, so specify those before drawing (or accept the defaults).
See xpoz.h for a little more explicit help.

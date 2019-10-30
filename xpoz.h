/* Tool to be inserted in scientific code to produce browser viewable graphics. */

/* A family of calls to simple graphics operations build up a
"display list" in a "bucket" which can be accessed separately by
a web page to display the built up image as an HTML canvas.

Security, Privacy, and error checking:
    ABSOLUTELY NONE.  USE AT YOUR OWN RISK.
    
Invoking (in a Google cloud environment):
    mount the public bucket "xpoz", or your own bucket, at the
        mount point "xpoz" (see
        https://cloud.google.com/compute/docs/disks/gcs-buckets)

    #include "~/xpoz.h"    assuming you have copied it to your home
    ...
    int main () {
        ... your useful code ...
        title("My_NewProgram");
        line(newx, somey); // and other routines to build up image
        ...
        view();    // xpoz will write the file & hang for ENTER key
          > OR <
        page(19);  // xpoz will write the file with page number
    }

Browsing:
    Open xpoz.org in a browser.  Fill in the title to see the image.

Impact:
    Some.
    If #define NOXPOZ precedes the #include "~/xpoz.h" then
        most of the routines compile as empty calls and your
        optimizer will probably omit them completely.  This means
        you probably do not need a bunch of #ifdef's in your code.
    If, instead, you execute xpoz_off() then almost nothing
        is done pending xpoz_on().

Coordinates:
    Everything is in units of pixels.
    Coordinate system is "first quadrant", i.e. 0, 0 is lower left.
    Screen size 1001 x 1001 (simple to change, could be function).
    Off screen coordinates are not checked.

Functions:
    System is stateful: remembers x, y, color, font, font height, title.
    setxy(x, y), setx(x), sety(y)   move without drawing, absolute coordinates
    incx(dx), incy(dy)              relative moves
    line(x, y), hline(x), vline(y)  absolute coordinates
    rline(dx), lline(dx), uline(dy), dline(dy) right/left/up/down delta 1..256
    rect(width, height)             draw filled rect (w & h can be negative)
    color(k)                        hex rrggbb, default black with white background
    font("name_of_font")            default "20px Courier New"
    texthi(h)                       default 24, delta y for writeln() and endl()
    write("text"), writeln("text")  draw text (writeln() = write() + endl())
    endl()                          down and back to left edge of previous text
    title(("file_name_compatible")
    page(n)                         output image as file_name.pnnn, n = 0..999
    char *view("msg")               output image as file_name.p000 and block for stdin
    boxGraph(int *data, int length, int width)  what to plot and pixel width of graph
    boxGraphBi(int *data, int length, int width,
        int color1, int cut_bin, int color2) colors up to/after of cut_bin
    hticks(every, totbins, totwidth)ticks on horizontal baseline
    getters                         for state variables
    xpoz_off(), xpoz_on()           de/activate functions

Printing:
    None.  Try using a Snipping Tool or other screen capture.
*/

#define RED   (0xFF0000)
#define GREEN (0x00ff00)
#define BLUE  (0x0000FF)
#define GRAY  (0xB0b0B0)
#define BLACK (0x000000)
#define CHARS 256

void xpoz_off();
void xpoz_on();
void title(char *c);
char *view(char *s);
void page(unsigned int);
void erase();
void setx(int x);
void sety(int y);
void incx(int x);
void incy(int y);
void setxy(int x, int y);
void color(int colr);
int getx();
int gety();
int getcolor();
char *getfont();
int getwidth();
int getheight();
void line(int x, int y);
void hline(int x);
void lline(int dx);
void rline(int dx);
void vline(int y);
void uline(int dy);
void dline(int dy);
void font(char *f);
void write(char * msg);
void writeln(char * msg);
void endl(); // end of text line
void texthi(int height);
void rect(int x, int y);
void boxGraph(int *data, int nbins, int pixWidth);
void boxGraphBi(int *data, int nbins, int pixWidth,
                int color1, int switchBin, int color2);
void hticks(int every, int length, int width);
/* Tool to be inserted in scientific code to produce browser viewable graphics. */

/* A family of calls to simple graphics operations build up an HTML page (currently in
a "bucket") which can be accessed separately by a browser to display the built up page.

Security, Privacy, and error checking:
    Absolutely none.  Use at your own risk.
    
Invoking:
    #include "endoscope.h"
    ...
    int main () {
        line(newx, somey); // and other routines to build up image
        ...
        view(); // endoscope will finish page & hang for ENTER key
    }
    
Browsing:
    //////Click to  http://storage.google.com/endoscope/codename.html which will
    In a browser type file:///C:/cygwin64/tmp/endoscope which will
    display the built up web page.    
    To continue the program press ENTER in window where scientific program is running.
    
Impact:
    Some.
    (NOT TRUE ->) If #define NOENDOSCOPE preceeds the #include endoscope.h then
        most of the routines compile as empty calls and your
        optimizer will probably omit them completely.  This
        means you probably do not need a bunch of #ifdef's in your code.
    If, instead, you execute endo_off() then almost nothing is done pending endo_on().
*/

#define RED   (0xFF0000)
#define GREEN (0x00ff00)
#define BLUE  (0x0000FF)
#define GRAY  (0xB0b0B0)
#define BLACK (0x000000)
#define CHARS 256

void endo_off();
void endo_on();
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
void vline(int y);
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
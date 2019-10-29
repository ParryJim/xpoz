/* Tool to be inserted into scientific code to produce browser
   viewable graphics. */

/* A family of calls to simple graphics operations build up
 a file (currently in a "bucket" if in a cloud context) with
 coded commands to draw on a canvas.  That file
 may subsequently be read by a web page (e.g. xpoz.org) to
 draw the canvas.  The command file is a base64 version of
 the graphic commands.

Security, Privacy, and error checking:
    Absolutely none.  Use at your own risk.
    
Invoking:
    #include "xpoz.h"
    ...
    int main () {
        line(newx, somey); // & other routines to build up image
        ...
        view(); // xpoz finishes page & hangs for ENTER key
    }
    
Browsing:
    In a Google cloud environment browse to
        xpoz.org  where you will be asked for the title of your output
        (Note: you can retain multiple displays by changing the "title"
    In a cygwin64 environment browse to
        file:///C:/cygwin64/tmp/xpo to display your program's output.
    
    To continue your scientific program press ENTER in window where
        scientific program is running.
    
Impact:
    Some.
    (NOT EXACTLY TRUE ->) If #define NOXPOZ preceeds the #include xpoz.h then
        most of the routines compile as empty calls and your
        optimizer will probably omit them completely.  This
        means you probably do not need a bunch of #ifdef's in your code.
    If, instead, you execute xpoz_off() then almost nothing is done pending xpoz_on().

Details:
 All drawing is based upon pixel counts.  The coordinate system is "first
 quadrant" meaning that (0, 0) is in the lower left corner.  Coordinate
 values may be 16 bits but the canvas size is up to the webpage.
 The commands are built up in a memory buffer then base64 encoded and
 written to the output file when the xpoz function view() is executed.

*/

// set for operation in Google cloud type environment
#define CLOUD_BUCKET 1

#ifdef NOXPOZ
void xpoz_off() {};
void xpoz_on() {};
void title(char *c) {};
char *view(char *s) { return("");};
void page(unsigned int n) {};
void erase() {};
void setx(int x) {};
void sety(int y) {};
void incx(int x) {};
void incy(int y) {};
void setxy(int x, int y) {};
void color(int colr) {};
int getx()      {return(0);}
int gety()      {return(0);}
int getcolor()  {return(0);}
char *getfont() {return("");}
int getwidth()  {return(0);}
int getheight() {return(0);}    
void line(int x, int y) {};
void hline(int x) {};
void vline(int y) {};
void font(char *f) {};
void write(char * msg) {};
//void writeln(char * msg) {};
void endl() {};
void texthi(int height) {};
void rect(int x, int y) {};
void boxGraph(int *data, int length, int width) {};
void hticks(int every, int length, int width) {};

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "xpoz.h"

#define MOVE_TO   (1)
#define LINE_TO   (MOVE_TO+1)
#define HLINE     (LINE_TO+1)
#define RLINE     (HLINE+1)
#define LLINE     (RLINE+1)
#define VLINE     (LLINE+1)
#define ULINE     (VLINE+1)
#define DLINE     (ULINE+1)
#define STROKE    (DLINE+1)
#define SET_COLOR (STROKE+1)
#define FILL_RECT (SET_COLOR+1)
#define FONT      (FILL_RECT+1)
#define WRITE     (FONT+1)
#define ENDL      (WRITE+1)

static int  w = 1001;  // Could add functions to get/set these
static int  h = 1001;
static int  off = 0;   // default to "on"
static int  ready = 0; // default to "not ready"
static int  ex, wy, colr;
static char *codename = "try_title";
static FILE *fo;       // html output file
static char *command;
static size_t nchars;
static char *fnt = "20px Courier New";
static int  txtheight = 24;
static uint8_t *out;  // command list output buffer
static int  nouts, outSize; // current number, buffer size
void xpoz_off() {off = 1;}
void xpoz_on() {off = 0;}

// prototypes
void prepare();
void font(char *fnt);
char *base64_encode(const unsigned char *data,
                    int input_length,
                    int *output_length);


void erase() { if (off) return;
    if (!ready) {prepare(); return;}
    ex = wy = colr = 0;
    nouts = 0;
}

void crash(char *reason) {
    printf("\n=== FAIL ===\nbecause %s.\n", reason);
    exit(1);
}

void emit(uint8_t theByte) {
    //printf("\nemit: nouts%5d, outSize%5d, out %p, theByte%3x  ",
    //   nouts, outSize, out, theByte);
    if (nouts == outSize) {
        if (!(out = realloc(out, 2 * outSize))) {
            crash("cannot increase output buffer size");
        }
        outSize *= 2;
    }
    out[nouts++] = theByte;
}

void emitxy(uint8_t theCommand) {
    emit(theCommand);       //printf("command");
    emit((ex >> 8) & 0xff); //printf("x high bits");
    emit(ex & 0xff);        //printf("x low bits");
    emit((wy >> 8) & 0xff); //printf("y high bits");
    emit(wy & 0xff);        //printf("y low bits");
}

void emitx(uint8_t theCommand) {
    emit(theCommand);       //printf("command");
    emit((ex >> 8) & 0xff); //printf("x high bits");
    emit(ex & 0xff);        //printf("x low bits");
}

void emity(uint8_t theCommand) {
    emit(theCommand);       //printf("command");
    emit((wy >> 8) & 0xff); //printf("y high bits");
    emit(wy & 0xff);        //printf("y low bits");
}

void emitd(uint8_t theCommand, int delta) { // 1 byte delta coord
    emit(theCommand);       //printf("command");
    emit(delta & 0xff);     //printf("delta"); // 00 will be interpreted as 256
}

void emitcoords(uint8_t theCommand, int width, int height) {
    emit(theCommand);
    emit((width >> 8) & 0xff);  //printf("width high bits");
    emit(width & 0xff);         //printf("width low bits");
    emit((height >> 8) & 0xff); //printf("height high bits");
    emit(height & 0xff);        //printf("height low bits");
}

void emit3(int theBytes) {
    emit((theBytes >> 16) & 0xff); //printf("triple high bits");
    emit((theBytes >> 8) & 0xff);
    emit(theBytes & 0xff);         //printf("triple low bits");
}

void emitstring(uint8_t theCommand, char *theString) {
    emit(theCommand);      //printf("command");
    do {emit(*theString);  /*printf("%c", *theString ? *theString : '0');*/}
    while (*theString++);
}

void prepare() { // executed once
    #define INIT_BUF (1024)
    if (!(out = malloc(INIT_BUF))) crash("allocating command output buffer");
    outSize = INIT_BUF;
    nouts = 0;           // no bytes emitted yet
    if(!(command = malloc(CHARS))) { // initial, getline may move it
        printf("xpoz unable to allocate buffer to receive command.\n");
        exit(1);
    }
    nchars = CHARS;  // initial value, getline could change it
    ready = 1;
    erase();
};

void title(char *c) { codename = c;};

void stroke() { emit(STROKE); }

void totempfile() {
    if ((fo = fopen("/tmp/xpoz.html", "w")) == NULL) {
        printf("xpoz unable to open output html file");
        exit(1);
    }
    int encode_length;
    char *char64 = base64_encode(out, nouts, &encode_length);
    fprintf(fo, char64);
    //printf("\nBase64 file size = %d\n", ftell(fo));
    //printf("%s\n", char64);
    free(char64);
    fclose(fo);
}


char *view(char *msg) {
    if (off) return(""); if (!ready) prepare();
    totempfile();
#if(CLOUD_BUCKET)
    // assume bucket "xpoz" is available
    char s[1024];
    sprintf(s, "cp -f /tmp/xpoz.html ~/xpoz/");
    if (codename) {strcat(s, codename); strcat(s, ".p000");}
    system(s);
    printf("Browse to http://xpoz.org and enter the title  %s\n",
        codename);
#else
    printf("Browse to file:///C:/cygwin64/tmp/xpoz.html\n");
#endif
    printf("Then (type optional command here and) press ENTER\n");
    printf("%s\n", msg);
    getline(&command, &nchars, stdin);
    erase();
    return(command);
}

void page(unsigned int pageNumb) {
    if (off) return; if (!ready) prepare();
    if (pageNumb > 999) crash("page number must be 0 to 999");
    totempfile();
#if(CLOUD_BUCKET)
    // assume bucket "xpoz" is available
    char s[1024];
    char p[1024];
    sprintf(p, ".p%03d", pageNumb);
    sprintf(s, "cp -f /tmp/xpoz.html ~/xpoz/");
    if (codename) {strcat(s, codename); strcat(s, p);}
    system(s);
#endif
    erase();
}

void setx(int x) { if (off) return; if (!ready) prepare();
    ex = x;
    emitxy(MOVE_TO);
}

void incx(int x) { setx(ex + x); }

void sety(int y) { if (off) return; if (!ready) prepare();
    wy = y;
    emitxy(MOVE_TO);
}

void incy(int y) { sety(wy + y); }

void setxy(int x, int y) { if (off) return; if (!ready) prepare();
    ex = x; wy = y;
    emitxy(MOVE_TO);
}

void color(int color) { if (off) return; if (!ready) prepare();
    colr = 0xFFffFF & color;
    //emitxy(SET_COLOR);
    emit(SET_COLOR);
    emit3(color);

/*    char s[64];
    printf("color: fix this\n"); return;
    printf(fp, s);
    // draw any existing subpaths & delete them
    sprintf(s, "ctx.stroke(); ctx.beginPath();\n");
    fprintf(fp, s);
    setxy(ex, wy);  // create new subpath
    sprintf(s, "ctx.fillStyle = \"#%06x\";\n", colr);
    fprintf(fp, s);
    sprintf(s, "ctx.strokeStyle = \"#%06x\";\n", colr);
    fprintf(fp, s);
*/
}

void line(int x, int y) { if (off) return; if (!ready) prepare();
    if (x == ex) {vline(y); return;}
    if (y == wy) {hline(x); return;}
    ex = x; wy = y;
    emitxy(LINE_TO);
}

void hline(int x) { if (off) return; if (!ready) prepare();
    int dx = x - ex;
    ex = x;
    if (dx > 0) {
        if (dx <= 256) emitd(RLINE, dx); // short right
        else emitx(HLINE);
    } else if (dx < 0) {
        dx = -dx;
        if (dx <= 256) emitd(LLINE, dx); // short left
        else emitx(HLINE);
    } 
}

void vline(int y) { if (off) return; if (!ready) prepare();
    int dy = y - wy;
    wy = y;
    if (dy > 0) {
        if (dy <= 256) emitd(DLINE, dy); // short down (y inverted)
        else emity(VLINE);
    } else if (dy < 0) {
        dy = - dy;
        if (dy <= 256) emitd(ULINE, dy); // short up
        else emity(VLINE);
    }
}

void font(char *font) { // e.g. "20px Arial"
    if (off) return;  if (!ready) prepare();
    fnt = font;
    //char s[1024];
    //sprintf(s, "ctx.font = \"%s\";\n", fnt);
    //fprintf(fp, s);
    emitstring(FONT, font);
}


// getters
int getx()      {return(ex);}
int gety()      {return(wy);}
int getcolor()  {return(colr);}
char *getfont() {return(fnt);}
int getwidth()  {return(w);}
int getheight() {return(h);}    

void write(char * msg) { // N.B.  Does not handle \n.
    if (off) return; if (!ready) prepare();
    emitstring(WRITE, msg);
}

void texthi(int txhi) { if (off) return; if (!ready) prepare();
    txtheight = txhi;
}

void endl() {
    // OK, this is awkward.  This code deals with the height
    // of the font (but not well yet) and the browser-based
    // interpreter deals with the width of the rendered text.
    emitd(ENDL, txtheight);
}

void writeln(char *msg) {if (off) return; if (!ready) prepare();
    write(msg);
    endl();
}

void rect(int ww, int hh) {if (off) return; if (!ready) prepare();
    emitcoords(FILL_RECT, ww, hh);
}
    
void boxGraph(int *data, int length, int width) {
    if (off) return; if (!ready) prepare();
    const int basey = wy;
    const int basex = ex;
    const double bin_width = width/(double)length;
    int ymax = basey;
    int ymin = ymax;
    for (int bin = 0; bin < length; ) {
        int y = basey + data[bin++];
        int x = basex + (int)(bin * bin_width);
        int dx = x - ex;   // width of this bin
        if (dx) {  // finish any multiples per v-line
            if (ymax != ymin) {vline(ymin); vline(ymax);}
            vline(y);
            hline(x);
            ymax = ymin = wy;
        } else {
            if (y > ymax) ymax = y;
            else if (y < ymin) ymin = y;
        }
    } // for (bin....)
    if (ymax != ymin) {vline(ymin); vline(ymax);}
    vline(basey);
    hline(basex);
}

    
void boxGraphBi(int *data, int length, int width, // what to plot and pixel width
                int color1, int cut_bin, int color2) {
    if (off) return; if (!ready) prepare();
    const int old_color = getcolor();
    color(color1);
    const int basey = wy;
    const int basex = ex;
    const double bin_width = width/(double)length;
    int ymax = basey;
    int ymin = ymax;
    for (int bin = 0; bin < length; ) {
        int y = basey + data[bin++];
        int x = basex + (int)(bin * bin_width);
        int dx = x - ex;        // width of this bin
        if (dx) { // finish any multiples per v-line
            if (ymax != ymin) {vline(ymin); vline(ymax);}
            vline(y);
            hline(x); // proceed to next visible bin
            ymax = ymin = wy;;
        } else {
            if (y > ymax) ymax = y;
            else if (y < ymin) ymin = y;
        }
        if ((bin >= cut_bin) && (getcolor() != color2)) {
            color(color2);
        }
    } // for (bin....)
    if (ymax != ymin) {vline(ymin); vline(ymax);}
    vline(basey);
    color(old_color);
    hline(basex);
} // boxGraphBi()

// ticks along horizontal baseline
#define TICKHALF (2)
void hticks(int every, int nbins, int totpixels) {
    if (off) return; if (!ready) prepare();
    const int basey = wy;
    const int basex = ex;
    const double bin_width = (double)totpixels / nbins;
    for (int bin = 0; bin < nbins; bin += every) {
        int y = basey - TICKHALF;
        int x = basex + (int)(bin * bin_width);
        if (x != ex) { // do not draw overlapping ticks
            setxy(x, y); // could reduce with square wave pattern
            vline(y + 2 * TICKHALF + 1);
        }
    }
    setxy(basex, basey);  // return to origin      
} // hticks()

// following adapted from
// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[] = {0, 2, 1}; 


char *base64_encode(const unsigned char *data,
                    int input_length,
                    int *output_length) { // not including null termination
    *output_length = 4 * ((input_length + 2) / 3); // 1 for string termination
    char *encoded_data = malloc(1 + *output_length);
    if (encoded_data == NULL) return NULL;
    for (int i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }   
    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';
    encoded_data[*output_length] = 0;
    return (encoded_data);
}

#endif // NOXPOZ
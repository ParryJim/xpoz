#include "xpoz.h"
#include <math.h>

void main() {
    title("demo_page1");
    setx(16);
    line(32, 256);
    color(0x0000Ff);
    line(64, 17);
    color(RED);
    rect(20, 10);
    font("20px Courier New");
    vline(300);
    writeln("Happy Boo.");
    //endl(); // new line
    write(" October's 'bout gone.");
    view("demo_page1 sent");

    color(GREEN);
    line(400, 276); //hline(400);
    line(400, 500); //vline(500);
    hline(64);
    #define BINS (100)
    #define SCALE (100)
    int gdata[BINS];
    for (int j = 0; j < BINS; j++) {
        gdata[j] = SCALE * sin((6.28 * j) / BINS);
    }
    setxy(100, 100);
    boxGraphBi(gdata, BINS, 600,
               RED, BINS/3, GREEN);
    title("demo_page2");
    view("demo_page2 sent");
}
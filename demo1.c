/* Quick & dirty demo of xpoz.h */

//#define NOXPOZ
#include "xpoz.h"
#include <stdio.h>
#include <stdlib.h>
int main () {
    title("Demo");
    setx(13);
    sety(600);
    color(0xff00ff); // purple?
    line(53, 620);
    hline(80);
    write("Silly demo!");
    int page_numb = 0;
    page(page_numb++);

    // overlapping rectangles
    int ww = 500, hh = 500;
    setx(ww); sety(hh);
    while (ww) {
        color(rand());
        rect((ww = -(7 * ww) / 8), hh);
        rect(ww, (hh = -(7 * hh) / 8));
    }
    page(page_numb++);

    // high science
    int bump[121];
    int bins = sizeof(bump)/sizeof(bump[0]);
    int trials = 10000;
    //xpoz_off();  // dynamically bypass xpoz routines
    while (trials <= 1000000) {
        for (int bin = bins; bin;) bump[--bin] = 0;
        for (int trial = trials; trial; --trial) {
            double avg12 = (drand48() + drand48() + drand48() + drand48() +
                            drand48() + drand48() + drand48() + drand48() +
                            drand48() + drand48() + drand48() + drand48()) / 12.0;
            bump[(int)(bins * avg12)]++;
        }
        setxy(100, 900);
        char writeThis[100];
        font("30px Comic");
        sprintf(writeThis, "For %d", trials);
        writeln(writeThis);
        font("20px Courier New");
        sprintf(writeThis, " trials");
        write(writeThis);
        setxy(0, 300);
        // scale to pixels for xpoz display
        for (int bin = bins; bin;) bump[bin--] *= 100 * bins / (double)trials;
        boxGraph(bump, bins, 600);
        // Following is example of interactive "view"
        //printf("Enter # trials for next run (zero quits)\n");
        //if (sscanf(view(), "%d", &trials) <= 0) break;
        // Or just use page()
        page(page_numb++);
        trials *= 10;
    } // while()
}


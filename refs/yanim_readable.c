#include <stdio.h>
#include <math.h>
#include <unistd.h>

char angleStr[] = "O:85!fI,wfO8!yZfO8!f*hXK3&fO;:O;#hP;\"i[b",
	 // a = (k["O:85!fI,wfO8!yZfO8!f*hXK3&fO;:O;#hP;\"i[by asloane"] - 79) / CharPerRad; // angle of the normal of lines
	 //         |-----|---|----|----|---|-----|---|----|----------, 49
	 intscStr[] = "<[\\]O=IKNAL;KNRbF8EbGEROQ@BSX""XtG!#t3!^";
	 //            |------|---|----|----|---|-----|---|---|-, 40
int frameWidth = 74, frameHeight = 24;

int c,p,i,j,n,F,k,m;
// Denote "CF" as "character in the frame", which consists of three pixels, stacked vertically.
// c: the last 3 bits of its value denote the visibility of the {upper, middle, lower} part (pixel) of the current CF (each CF corresponds to 3 pixels), in the order from the last to the 3rd-last digits of the value of c.
// p: the beginning index of next polygon, in the angle and intersection arrays defining the polygons.
// i: the horizontal position of the pixel. Right is larger.
// j: the vertical position of the pixel. Down is larger. Note it is not that of CF.
// n: "n=2" means currently the lower pixel of the current CF is considered. "n=1": the middle pixel of the current CF; "n=0": the upper pixel.
// F: frame time
// k: index scanning the angle and intersection arrays.
// m: the index of the polygon.
float a,x,y,S=0,V=0;
// a: angle of the considered line
// x,y: the scaled position of the pixel (not of the CF).
// S: scale variable.
// V: velocity variable.

/***
 * Data are stored by the printable characters, from ' ' (ascii value = 32) to '~' (ascii value = 126).
 * The median of this interval is 'O' (ascii value = 79), with both sides 47 characters available.
 * The entire interval is of length 95.
***/
int main()
{
	float CharPerRad = 14.64, // 46/pi,
		  CharPerInt = 1.16; // 79/68;
	for(F=39; F>-1; --F) { // time frame
		V += (1-S)/10-V/4;
		S += V;
		for(j=0;j<frameHeight*3;j+=3) { // for each y. Note that each CF represents 3 pixels: upper part, middle part, and the lower part. The frame height is 24.
			for(i=1; i<frameWidth; ++i) { // for each x
				x = S*(i-28); // 28 is the horizontal position of the center of the ellipse, so it is smaller than the horizontal position of the center of the frame.
				c = 0;
				for(n=2; n>-1; --n) { // for the visibility of the lower pixel (n=2), the middle pixel (n=1), and the upper pixel (n=0).
					y=S*(j+n-36); // verticle position of the current pixel
					k=0; p=6;
					c ^= (136*x*x + 84*y*y < 92033)<<n; // If the point is in the ellipse, the corresponding bit of c is set to 1.
					for(m=0; m<8;) { // for the m-th polygon
						a = (k[angleStr] - 79) / CharPerRad; // angle of the normal of lines
						if (k[intscStr]/CharPerInt - 68 > x*cos(a)+y*sin(a)) { // before > is the corresponding intersections of lines
							// outside the m-th polygon: move to the next polygon
							k=p;
							p="<AFJPTX"[m]-50; // 10 15 20 24 30 34 38, (p-1) is the last boundary line of the m-th polygon (m: after ++m)
							++m;
						} else {
							// may inside the m-th polygon
							++k;
							if(k == p) {
								c^=1<<n; m=8; // all the boundary lines of the m-th polygon checked and passed, in the polygon! terminate it.
								// If the point is in the polygon, alter the visibility of the corresponding pixel.
							}
						}
					}
				}
				putchar(c[" ''\".$u$"]); // c: 000-nothing- , 001-upper-', 010-middle-', 011-middle&upper-", 100-lower-., 101-lower&upper-$, 110-lower&middle-u, 111-lower&middle&upper-$
			}
			putchar('\n');
		}
		usleep(50000);
		if(F != 0) puts("\x1b[25A"); // "\x1b" is 16+11=27, the ASCII of "ESC". This command moves the cursor up 25 lines (1 more line than the height of the frame (24), since "puts" automatic does the line break).
	}
	return 0;
}

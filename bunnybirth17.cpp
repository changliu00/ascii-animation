#include <vector>
#include <cmath>
#include <cstdio> // putchar
#include <unistd.h> // usleep
#include "ascii-anim.h"

int main()
{
	int c,p,i,j,n,F,k,m;
	float a,x,y,S=0,V=0;
	float alpha = 10, beta = 4;
	bool val;
	int frameWid = 108, frameHt = 128, num_shapes = 9;
	float cenx = frameWid/2., ceny = frameHt/2., cscale = 1;
	float nosehw = 4, noseh = 10, noseuy = ceny+20, nosely = noseuy+noseh,
		  ulipw = 2, uliphh = 2, ulipcy = nosely+uliphh - .4,
		  llipcy = ulipcy+uliphh, llipcr = 4, llipw = 2, llipa = pi/4,
		  xdis = 25;

	std::vector<Shape*> bunny0;
	bunny0.reserve(num_shapes);
	bunny0.push_back(new Ellipse(cenx-8, ceny+12, 3.2, 4.8, 0, false));
	bunny0.push_back(new Ellipse(cenx+8, ceny+12, 3.2, 4.8, 0, false));
	bunny0.push_back(new Triangle(cenx-nosehw, noseuy, cenx+nosehw, noseuy, cenx, nosely, false));
	bunny0.push_back(new Rectangle(cenx, ulipcy, ulipw, uliphh*2, 0, false));
	bunny0.push_back(new EllArc(cenx-llipcr, llipcy, llipcr-llipw/2, llipcr, 0, llipw, 0, pi-llipa, false));
	bunny0.push_back(new EllArc(cenx+llipcr, llipcy, llipcr-llipw/2, llipcr, 0, llipw, llipa, pi-0.01, false));
	bunny0.push_back(new Ellipse(cenx, ceny+20, 20, 30, 0));
	bunny0.push_back(new Ellipse(cenx-10, ceny-25, 8, 30, -18./180*pi));
	bunny0.push_back(new Ellipse(cenx+10, ceny-25, 8, 30, +18./180*pi));
	for(int s=0; s<bunny0.size(); s++) bunny0[s]->displace(-xdis, 0);

	std::vector<Shape*> num22;
	float n22hdist = 15, n22hl = 30, n22ang = pi/12, n22w1 = 2.7, n22w2 = 7, ydis = 10,
		  n22hh = n22hl*cos(n22ang), n22hw = n22hl*sin(n22ang);
	num22.reserve(6);
	num22.push_back(new EllArc(cenx-n22hdist, ceny-n22hh, n22hw, n22hw*3/2, 0, n22w1, -pi+.01, n22ang));
	num22.push_back(new Rectangle(cenx-n22hdist, ceny, n22w2, 2*n22hh, n22ang+pi/35));
	num22.push_back(new Rectangle(cenx-n22hdist, ceny+n22hh, 2*(n22hw+n22w2), n22w2, 0));
	num22.push_back(new EllArc(cenx+n22hdist, ceny-n22hh, n22hw, n22hw*3/2, 0, n22w1, -pi+.01, n22ang));
	num22.push_back(new Rectangle(cenx+n22hdist, ceny, n22w2, 2*n22hh, n22ang+pi/35));
	num22.push_back(new Rectangle(cenx+n22hdist, ceny+n22hh, 2*(n22hw+n22w2), n22w2, 0));
	for(int s=0; s<num22.size(); s++) num22[s]->scale(0.8, cenx, ceny);
	for(int s=0; s<num22.size(); s++) num22[s]->displace(xdis, ydis);

	cscale = 1;
	for(F=39; F>-1; --F) {
		V += (1-S)/alpha-V/beta;
		S += V;
		cscale = cscale/S;
		for(int s=0; s<bunny0.size(); s++) bunny0[s]->scale(cscale, cenx-xdis, ceny);
		cscale = S;
		for(j=0;j<frameHt;j+=3) {
			for(i=0; i<frameWid; ++i) {
				x = i; c = 0;
				for(n=2; n>-1; --n) {
					y=j+n;
					for(int s=0; s<bunny0.size(); s++) if(bunny0[s]->judge(x, y, &val)) break;
					c ^= val<<n;
				}
				putchar(c[" ''\".$u$"]);
			}
			putchar('\n');
		}
		usleep(50000);
		char rewindstr[20]; sprintf(rewindstr, "\x1b[%dA", frameHt+1);
		if(F != 0) puts(rewindstr);
	}

	V=0; S=0;
	cscale = 1;
	for(F=39; F>-1; --F) {
		V += (1-S)/alpha-V/beta;
		S += V;
		cscale = cscale/S;
		for(int s=0; s<num22.size(); s++) num22[s]->scale(cscale, cenx+xdis, ceny);
		cscale = S;
		for(j=0;j<frameHt;j+=3) {
			for(i=0; i<frameWid; ++i) {
				x = i; c = 0;
				for(n=2; n>-1; --n) {
					y=j+n;
					bool brklabel = false;
					for(int s=0; s<num22.size(); s++) {
						if(num22[s]->judge(x, y, &val)) {
							brklabel = true; break;
						}
					}
					if(!brklabel) for(int s=0; s<bunny0.size(); s++) if(bunny0[s]->judge(x, y, &val)) break;
					c ^= val<<n;
				}
				putchar(c[" ''\".$u$"]);
			}
			putchar('\n');
		}
		usleep(50000);
		char rewindstr[20]; sprintf(rewindstr, "\x1b[%dA", frameHt+1);
		if(F != 0) puts(rewindstr);
	}

	return 0;
}


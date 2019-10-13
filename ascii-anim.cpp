#include <cmath>
#include "ascii-anim.h"

Line::Line(float angle, float intsc, float width, bool solid):
	Shape(solid), angle(angle), intsc(intsc), width(width), vx(cos(angle)), vy(sin(angle)) {
}
bool Line::judge(float x, float y, bool *val) {
	float dist = x*vx + y*vy;
	bool gov = (dist > intsc) && (dist <= intsc+width);
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}
void Line::displace(float dx, float dy) {
	intsc += dx*vx + dy*vy;
}
void Line::rotate(float dangle, float rox, float roy) {
	angle += dangle;
	dangle = rox*vx + roy*vy - intsc;
	vx = cos(angle); vy = sin(angle);
	intsc = rox*vx + roy*vy - dangle;
}
void Line::scale(float rate, float sox, float soy) {
	float dist = sox*vx + soy*vy;
	intsc = (1-rate)*dist + intsc*rate;
	width *= rate;
}
void Line::scale(float rate) { // expand/shrink wrt the centerline
	intsc -= (1-rate)/2*width;
	width *= rate;
}

Triangle::Triangle(float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, bool solid):
	Shape(solid), p0x(p0x), p0y(p0y), p1x(p1x), p1y(p1y), p2x(p2x), p2y(p2y),
	v1x(p1x-p0x), v1y(p1y-p0y), v2x(p2x-p0x), v2y(p2y-p0y), denom(v1x*v2y - v1y*v2x) {
}
bool Triangle::judge(float x, float y, bool *val) {
	float vx = x - p0x, vy = y - p0y,
		  a = vx*v2y - vy*v2x,
		  b = vy*v1x - vx*v1y;
	bool gov = ((a>0) && (b>0) && (a+b<denom));
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}
void Triangle::displace(float dx, float dy) {
	p0x += dx; p1x += dx; p2x += dx;
	p0y += dy; p1y += dy; p2y += dy;
}
void Triangle::rotate(float dangle, float rox, float roy) {
	float cosa = cos(dangle), sina = sin(dangle), dx, dy;
	dx = p0x-rox; dy = p0y-roy; p0x = dx*cosa - dy*sina + rox; p0y = dx*sina + dy*cosa + roy;
	dx = p1x-rox; dy = p1y-roy; p1x = dx*cosa - dy*sina + rox; p1y = dx*sina + dy*cosa + roy;
	dx = p2x-rox; dy = p2y-roy; p2x = dx*cosa - dy*sina + rox; p2y = dx*sina + dy*cosa + roy;
	v1x = p1x - p0x; v1y = p1y - p0y;
	v2x = p2x - p0x; v2y = p2y - p0y;
	// denom is unchanged
}
void Triangle::rotate(float dangle) { // rotate around the centroid
	float rox = (p0x+p1x+p2x)/3, roy = (p0y+p1y+p2y)/3;
	this->rotate(dangle, rox, roy);
}
void Triangle::scale(float rate, float sox, float soy) {
	sox *= (1-rate); soy *= (1-rate);
	p0x = rate*p0x + sox; p0y = rate*p0y + soy;
	p1x = rate*p1x + sox; p1y = rate*p1y + soy;
	p2x = rate*p2x + sox; p2y = rate*p2y + soy;
	v1x *= rate; v1y *= rate; v2x *= rate; v2y *= rate;
	denom *= (rate*rate);
}
void Triangle::scale(float rate) { // scale about the centroid
	float sox = (p0x+p1x+p2x)/3, soy = (p0y+p1y+p2y)/3;
	this->scale(rate, sox, soy);
}

Rectangle::Rectangle(float cx, float cy, float wid1, float wid2, float angle, bool solid):
	Shape(solid), cx(cx), cy(cy), wid1(wid1), wid2(wid2),
	l1(halfpi+angle, -cx*sin(angle)+cy*cos(angle)-wid2/2, wid2), l2(angle, cx*cos(angle)+cy*sin(angle)-wid1/2, wid1) {
}
bool Rectangle::judge(float x, float y, bool *val) {
	bool gov = (l1.judge(x,y) && l2.judge(x,y));
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}
void Rectangle::displace(float dx, float dy) {
	cx += dx; cy += dy;
	l1.displace(dx,dy); l2.displace(dx,dy);
}
void Rectangle::rotate(float dangle, float rox, float roy) {
	float sina = sin(dangle), cosa = cos(dangle), dx = cx - rox, dy = cy - roy;
	cx = dx*cosa - dy*sina + rox; cy = dx*sina + dy*cosa + roy;
	l1.rotate(dangle, rox, roy); l2.rotate(dangle, rox, roy);
}
void Rectangle::rotate(float dangle) {
	l1.rotate(dangle, cx, cy); l2.rotate(dangle, cx, cy);
}
void Rectangle::scale(float rate, float sox, float soy) {
	cx = rate*cx + (1-rate)*sox;
	cy = rate*cy + (1-rate)*soy;
	wid1 *= rate; wid2 *= rate;
	l1.scale(rate, sox, soy); l2.scale(rate, sox, soy);
}
void Rectangle::scale(float rate) { // scale wrt the centroid
	wid1 *= rate; wid2 *= rate;
	l1.scale(rate); l2.scale(rate);
}

Ellipse::Ellipse(float cx, float cy, float a, float b, float angle, bool solid):
	Shape(solid), cx(cx), cy(cy), asq(a*a), bsq(b*b), angle(angle),
	sina(sin(angle)), cosa(cos(angle)) {
}
bool Ellipse::judge(float x, float y, bool *val) {
	float dx = x-cx, dy = y-cy;
	x = dx*cosa + dy*sina; y = -dx*sina + dy*cosa;
	bool gov = (x*x/asq + y*y/bsq < 1);
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}
void Ellipse::displace(float dx, float dy) {
	cx += dx; cy += dy;
}
void Ellipse::rotate(float dangle, float rox, float roy) {
	float dx = cx-rox, dy = cy-roy, sinda = sin(dangle), cosda = cos(dangle);
	cx = dx*cosda - dy*sinda + rox; cy = dx*sinda + dy*cosda + roy;
	angle += dangle; sina = sin(angle); cosa = cos(angle);
}
void Ellipse::rotate(float dangle) {
	angle += dangle; sina = sin(angle); cosa = cos(angle);
}
void Ellipse::scale(float rate, float sox, float soy) {
	float rate2 = rate*rate;
	asq *= rate2; bsq *= rate2;
	cx = cx*rate + (1-rate)*sox; cy = cy*rate + (1-rate)*soy;
}
void Ellipse::scale(float rate) { // scale wrt the center
	float rate2 = rate*rate;
	asq *= rate2; bsq *= rate2;
}

EllArc::EllArc(float cx, float cy, float a, float b, float angle, float width, float anglim1, float anglim2, bool solid):
	Shape(solid), cx(cx), cy(cy), asq(a*a), bsq(b*b), angle(angle), width(width), anglim1(anglim1), anglim2(anglim2),
	sina(sin(angle)), cosa(cos(angle)), tan1(tan(anglim1)), tan2(tan(anglim2)),
	inright1(fabs(anglim1) < halfpi), inright2(fabs(anglim2) < halfpi) {
}
bool EllArc::judge(float x, float y, bool *val) {
	float dx = x-cx, dy = y-cy, dist, ratio;
	bool gov = false;
	x = dx*cosa + dy*sina; y = -dx*sina + dy*cosa;
	dist = x*x/asq + y*y/bsq;
	if(dist >= 1 && dist < 1+width) {
		// if(fabs(x) < 1e-20)
		if(x == 0)
			gov = ( (y>0) && (anglim1 <= halfpi) && (anglim2 > halfpi) )
				|| ( (y<0) && (anglim1 <= -halfpi) && (anglim2 > -halfpi) );
		else if(x>0) {
			ratio = y/x;
			if( (inright1 && (ratio>=tan1)) || (!inright1 && tan1>=0) )
				gov = ( (inright2 && (ratio<tan2)) || (!inright2 && tan2<=0) );
		} else {
			ratio = y/x;
			if( (inright1 && (y>=0)) || (!inright1 && (y>=0) && (ratio>=tan1)) || (!inright1 && (y<=0) && (ratio>=tan1) && (tan1>=0) ) )
				gov = ( (inright2 && (y<0)) || (!inright2 && (y>=0) && (ratio<tan2) && (tan2<=0)) || (!inright2 && (y<=0) && (ratio<tan2)) );
		}
	}
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}
void EllArc::displace(float dx, float dy) {
	cx += dx; cy += dy;
}
void EllArc::rotate(float dangle, float rox, float roy) {
	float dx = cx-rox, dy = cy-roy, sinda = sin(dangle), cosda = cos(dangle);
	cx = dx*cosda - dy*sinda + rox; cy = dx*sinda + dy*cosda + roy;
	angle += dangle; sina = sin(angle); cosa = cos(angle);
}
void EllArc::rotate(float dangle) {
	angle += dangle; sina = sin(angle); cosa = cos(angle);
}
void EllArc::scale(float rate, float sox, float soy) {
	float rate2 = rate*rate;
	asq *= rate2; bsq *= rate2;
	width *= rate;
	cx = cx*rate + (1-rate)*sox; cy = cy*rate + (1-rate)*soy;
}
void EllArc::scale(float rate) { // scale wrt the center
	float rate2 = rate*rate;
	asq *= rate2; bsq *= rate2;
	width *= rate;
}


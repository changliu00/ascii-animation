#pragma once
const float halfpi = 1.5707963267949, pi = 3.14159265358979;

class Shape
{
protected:
	bool solid; // If the governing region gets solid (not emptily printed).
public:
	Shape(bool solid): solid(solid) {};
	virtual bool judge(float x, float y, bool *val) = 0; // returned value: if (x,y) is in the governing region; val: if (x,y) gets solid printed.
	virtual void displace(float dx, float dy) = 0;
	virtual void rotate(float dangle, float rox, float roy) = 0;
	virtual void scale(float rate, float sox, float soy) = 0;
};

class Line: public Shape
{
	float angle, intsc, width, vx, vy; // angle: the angle from x-axis to the norm vector.
public:
	Line(float angle, float intsc, float width = 999, bool solid = true);
	bool judge(float x, float y, bool *val = nullptr);
	void displace(float dx, float dy);
	void rotate(float dangle, float rox = 0, float roy = 0);
	void scale(float rate, float sox, float soy);
	void scale(float rate); // expand/shrink wrt the centerline
};

class Triangle: public Shape
{
	float p0x, p0y, p1x, p1y, p2x, p2y,
		  v1x, v1y, v2x, v2y, denom;
public:
	Triangle(float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, bool solid = true);
	bool judge(float x, float y, bool *val = nullptr);
	void displace(float dx, float dy);
	void rotate(float dangle, float rox, float roy);
	void rotate(float dangle); // rotate around the centroid
	void scale(float rate, float sox, float soy);
	void scale(float rate); // scale about the centroid
};

class Rectangle: public Shape
{
	float cx, cy, wid1, wid2;
	Line l1, l2;
public:
	Rectangle(float cx, float cy, float wid1, float wid2, float angle, bool solid = true);
	bool judge(float x, float y, bool *val = nullptr);
	void displace(float dx, float dy);
	void rotate(float dangle, float rox, float roy);
	void rotate(float dangle);
	void scale(float rate, float sox, float soy);
	void scale(float rate); // scale wrt the centroid
};

class Ellipse: public Shape
{
	float cx, cy, asq, bsq, angle,
		  sina, cosa;
public:
	Ellipse(float cx, float cy, float a, float b, float angle, bool solid = true);
	bool judge(float x, float y, bool *val = nullptr);
	void displace(float dx, float dy);
	void rotate(float dangle, float rox, float roy);
	void rotate(float dangle);
	void scale(float rate, float sox, float soy);
	void scale(float rate); // scale wrt the center
};

class EllArc: public Shape
{
	float cx, cy, asq, bsq, angle, width, anglim1, anglim2,
		  sina, cosa, tan1, tan2;
	bool inright1, inright2;
public:
	EllArc(float cx, float cy, float a, float b, float angle, float width, float anglim1, float anglim2, bool solid = true);
	bool judge(float x, float y, bool *val = nullptr);
	void displace(float dx, float dy);
	void rotate(float dangle, float rox, float roy);
	void rotate(float dangle);
	void scale(float rate, float sox, float soy);
	void scale(float rate); // scale wrt the center
};


/***************************************************************************
  ========================================================================
  |                                                                      |
  |    C O N S O L E    A S C I I    A N I M A T I O N    S Y S T E M    |
  |                                                                      |
  |                 A gift for the 3rd anniversary with                  |
  |                              Suki H.                                 |
  |______________________________________________________________________|
  |                                                                      |
  |  Author:  Chang Liu,                                                 |
  |           Dept. of CS&T, Tsinghua University                         |
  |           <http://ml.cs.tsinghua.edu.cn/~changliu/index.html>        |
  |  Contact: <liuchangsmail@gmail.com>, <lctrumb@126.com>               |
  |  Date:    01/28/2018                                                 |
  |                                                                      |
  |  Notes:                                                              |
  |  * The frame is a right-handed system. The center is (0,0). The      |
  |    verticle bound is [-1, 1], and the horizontal bound is determined |
  |    by the aspect ratio (currently 1.856).                            |
  |  * Do not use any copy constructors except "new ClassName(*this)"!   |
  ========================================================================
***************************************************************************/

#include <iostream>
#include <utility> // pair
#include <initializer_list>
#include <cfloat> // FLT_MAX, FLT_MIN
#include <vector>
#include <string>
#include <functional> // std::function
#include <cmath> // sin, cos, atan2

using Fpair = std::pair<float, float>;
const float halfpi = 1.5707963267949, pi = 3.14159265358979;
static float TimeNow = 0;

/***************************************************************************
*************** Helper Functions for Point Manipulations ******************/

inline bool operator<(const Fpair& pt1, const Fpair& pt2) {
	return (pt1.first < pt2.first) && (pt1.second < pt2.second);
}
inline bool operator<=(const Fpair& pt1, const Fpair& pt2) {
	return (pt1.first <= pt2.first) && (pt1.second <= pt2.second);
}
inline Fpair& operator+=(Fpair& pt1, const Fpair& pt2) {
	pt1.first += pt2.first; pt1.second += pt2.second;
	return pt1;
}
inline Fpair& operator-=(Fpair& pt1, const Fpair& pt2) {
	pt1.first -= pt2.first; pt1.second -= pt2.second;
	return pt1;
}
inline Fpair operator+(const Fpair& pt1, const Fpair& pt2) {
	return {pt1.first + pt2.first, pt1.second + pt2.second};
}
inline Fpair operator-(const Fpair& pt1, const Fpair& pt2) {
	return {pt1.first - pt2.first, pt1.second - pt2.second};
}
inline Fpair operator-(const Fpair& pt) {
	return {-pt.first, -pt.second};
}
inline Fpair operator*(double c, const Fpair& pt) {
	return {c*pt.first, c*pt.second};
}
inline Fpair& operator*=(Fpair& pt, double c) {
	pt.first *= c; pt.second *= c;
	return pt;
}
inline float operator*(const Fpair& pt1, const Fpair& pt2) { // inner product
	return pt1.first * pt2.first + pt1.second * pt2.second;
}
inline float cross(const Fpair& pt1, const Fpair& pt2) {
	return pt1.first * pt2.second - pt1.second * pt2.first;
}
inline Fpair& rot_pt_pt(Fpair& pt, const Fpair& p0, float dangle) {
	Fpair pd = pt - p0, v = {cos(dangle), -sin(dangle)};
	return pt = Fpair{v * pd, cross(v, pd)} + p0;
}
inline void rot_pt_pt(std::initializer_list<Fpair*> il, const Fpair& p0, float dangle) {
	Fpair v = {cos(dangle), -sin(dangle)}, pd;
	for(auto ppt: il) {
		pd = (*ppt) - p0;
		(*ppt) = Fpair{v * pd, cross(v, pd)} + p0;
	}
}
inline Fpair& scl_pt_pt(Fpair& pt, const Fpair& p0, float rate) {
	return pt = rate*pt + (1-rate)*p0;
}
inline void scl_pt_pt(std::initializer_list<Fpair*> il, Fpair p0, float rate) {
	p0 *= (1-rate);
	for(auto ppt: il) (*ppt) = rate*(*ppt) + p0;
}

inline void union_pt(Fpair& lower1, Fpair& upper1, const Fpair& lower2, const Fpair& upper2) {
	if(lower2.first < lower1.first) lower1.first = lower2.first;
	if(lower2.second < lower1.second) lower1.second = lower2.second;
	if(upper1.first < upper2.first) upper1.first = upper2.first;
	if(upper1.second < upper2.second) upper1.second = upper2.second;
}

inline void intersect_pt(Fpair& lower1, Fpair& upper1, const Fpair& lower2, const Fpair& upper2) {
	if(lower1.first < lower2.first) lower1.first = lower2.first;
	if(lower1.second < lower2.second) lower1.second = lower2.second;
	if(upper2.first < upper1.first) upper1.first = upper2.first;
	if(upper2.second < upper1.second) upper1.second = upper2.second;
}

/*
After `ostringstream() << "aaa "`:
- On Mac, the type is still `ostringstream&&` and `<<` afterwards can only invoke API for `ostringstream&&` and cannot invoke API for `ostream&&`, unless explicitly `dynamic_cast<ostream&&>()`.
- On Ubuntu, `<<` afterwards invokes API for `ostream&` even when APIs for `ostream&&`, `ostringstream&`, `ostringstream&&` are implemented.
After `ss = ostringstream(); ss << "aaa "`:
- `<<` afterwards invokes API for `ostream&` on both systems.
*/
inline std::ostream& operator<<(std::ostream& out, const Fpair& pt) {
	out << "(" << pt.first << ", " << pt.second << ")";
	return out;
}
inline std::ostream&& operator<<(std::ostream&& out, const Fpair& pt) {
	out << pt;
	return std::move(out);
}
inline std::ostringstream& operator<<(std::ostringstream& out, const Fpair& pt) {
	dynamic_cast<std::ostream&>(out) << pt;
	return out;
}
inline std::ostringstream&& operator<<(std::ostringstream&& out, const Fpair& pt) {
	out << pt;
	return std::move(out);
}

/***************************************************************************
********************** Basic Classes: Do Not Alter ************************/
////// Motion Base Class //////
struct Obj;
struct Motion
{
	Fpair tlim;
public:
	enum class Type{incre, relpos, abspos};
	Motion(const Fpair& tlim): tlim(tlim) {};
	virtual ~Motion() {};
	virtual Motion* copy() const { return new Motion(*this); } // We need this since copy constructors cannot be virtual!
	virtual void act(Obj*) {};
};

////// Object Base Class: for layered union of objects //////
struct Obj
{
	static int FrameWidth, FrameHeight;
	static float wid_ht;

	const std::string name;
	Fpair pc, lower, upper, tlim;
	std::vector<Obj*> objs;
	std::vector<Motion*> motions;
public:
	static void set_frame(int fmwid, int fmht) {
		FrameWidth = fmwid; FrameHeight = fmht; wid_ht = static_cast<float>(fmwid)/fmht/1.856;
	}

	Obj(const std::string& name = "", const Fpair& tlim = {0,FLT_MAX}, const Fpair& pc = {0,0}, std::initializer_list<Obj*> il = {});
	virtual ~Obj();
	virtual Obj* copy() const;
	virtual Obj* copy(const std::string& name) const;
	Obj* add_objs(std::initializer_list<Obj*> il);
	// Motion* add_motions(std::initializer_list<Motion*> il);
	Obj* add_motions(std::initializer_list<Motion*> il);
	void erase_obj(const std::string& key);
	void erase_motion(int idx);
	Obj& operator[](const std::string& key);

	virtual bool judge(const Fpair& pt, bool *val = nullptr) const; // returned value: if pt=(x,y) is in the governing region; val: if pt=(x,y) gets solid printed.
	virtual void update(void);

	Obj* recenter(const Fpair& pc);
	Obj* adjcenter(const Fpair& pd);
	virtual Obj* displace(const Fpair& pd);
	virtual Obj* place(const Fpair& pp);
	virtual Obj* rotate(float dangle, const Fpair& pr);
	virtual Obj* rotate(float dangle);
	virtual Obj* scale(float rate, const Fpair& ps);
	virtual Obj* scale(float rate);
};

////// Wrapper Class for Intersection-based Objects //////
struct IntObj: public Obj
{
	bool solid; // If the governing region gets solid (not emptily printed).
public:
	IntObj(const std::string& name, bool solid = true, const Fpair& tlim = {0,FLT_MAX}, const Fpair& pc = {0,0}, std::initializer_list<Obj*> il = {});
	virtual Obj* copy() const;
	virtual bool judge(const Fpair& pt, bool *val = nullptr) const; // returned value: if pt=(x,y) is in the governing region; val: if pt=(x,y) gets solid printed.
	virtual void update(void);
};

////// The Frame Class for containing all the objects and output //////
struct Frame: public Obj
{
	float time_intv, x_intv, y_intv;
	const std::string rewindstr;
public:
	Frame(float fm_p_sec);
	void play(float time_end = 0);
};

/***************************************************************************
******** Specific Shape Objects: Customized New Ones Can Be Created *******/

struct Line: public Obj
{
	float angle, intsc, width; // angle: the angle from x-axis to the norm vector. intsc: the signed distance of the origin to the line, not the conventional intersection.
	Fpair v;
	bool solid; // If the governing region gets solid (not emptily printed).
public:
	Line(const std::string& name, float angle, float intsc, float width = 9999, bool solid = true, const Fpair& tlim = {0,FLT_MAX}, const Fpair& pc = {0,0});
	virtual Obj* copy() const;
	bool judge(const Fpair& pt, bool *val = nullptr) const;
	void update(void);
	Line* displace(const Fpair& pd);
	Line* rotate(float dangle, const Fpair& pr);
	Line* scale(float rate, const Fpair& ps);
	Line* scale(float rate); // expand/shrink wrt the centerline
};

struct Triangle: public Obj
{
	Fpair p0, p1, p2, v1, v2;
	float v1v2;
	bool solid; // If the governing region gets solid (not emptily printed).
	void set_lim(void);
public:
	Triangle(const std::string& name, const Fpair& p0, const Fpair& p1, const Fpair& p2, bool solid, const Fpair& tlim, const Fpair& pc);
	Triangle(const std::string& name, const Fpair& p0, const Fpair& p1, const Fpair& p2, bool solid = true, const Fpair& tlim = {0,FLT_MAX});
	virtual Obj* copy() const;
	virtual ~Triangle() {};
	bool judge(const Fpair& pt, bool *val = nullptr) const;
	void update(void);
	Triangle* displace(const Fpair& pd);
	Triangle* rotate(float dangle, const Fpair& pr);
	Triangle* scale(float rate, const Fpair& ps);
};

struct Triangle_isosceles: public Triangle
{
public:
	Triangle_isosceles(const std::string& name, const Fpair& pc0, float width, float height, float angle, bool solid, const Fpair& tlim, const Fpair& pc);
	Triangle_isosceles(const std::string& name, const Fpair& pc0, float width, float height, float angle, bool solid = true, const Fpair& tlim = {0,FLT_MAX});
	virtual Obj* copy() const;
};

struct Rectangle: public Obj
{
	Fpair pc0, v;
	float wid1, wid2, angle;
	Line l1, l2;
	bool solid; // If the governing region gets solid (not emptily printed).
	void set_lim(void);
public:
	Rectangle(const std::string& name, const Fpair& pc0, float wid1, float wid2, float angle, bool solid, const Fpair& tlim, const Fpair& pc);
	Rectangle(const std::string& name, const Fpair& pc0, float wid1, float wid2, float angle = 0, bool solid = true, const Fpair& tlim = {0,FLT_MAX});
	virtual Obj* copy() const;
	bool judge(const Fpair& pt, bool *val = nullptr) const;
	void update(void);
	Rectangle* displace(const Fpair& pd);
	Rectangle* rotate(float dangle, const Fpair& pr);
	Rectangle* scale(float rate, const Fpair& ps);
};

struct Ellipse: public Obj
{
	Fpair pc0, v;
	float asq, bsq, angle;
	bool solid; // If the governing region gets solid (not emptily printed).
	void set_lim(void);
public:
	Ellipse(const std::string& name, const Fpair& pc0, float a, float b, float angle, bool solid, const Fpair& tlim, const Fpair& pc);
	Ellipse(const std::string& name, const Fpair& pc0, float a, float b, float angle = 0, bool solid = true, const Fpair& tlim = {0,FLT_MAX});
	virtual Obj* copy() const;
	virtual ~Ellipse() {};
	virtual bool judge(const Fpair& pt, bool *val = nullptr) const;
	void update(void);
	Ellipse* displace(const Fpair& pd);
	Ellipse* rotate(float dangle, const Fpair& pr);
	Ellipse* scale(float rate, const Fpair& ps);
};

struct EllArc: public Ellipse
{
	float width, anglim1, anglim2; // anglim1 < anglim2 \in [-\pi, \pi], width \in (0,1]
public:
	EllArc(const std::string& name, const Fpair& pc0, float a, float b, float angle, float width, float anglim1, float anglim2, bool solid, const Fpair& tlim, const Fpair& pc);
	EllArc(const std::string& name, const Fpair& pc0, float a, float b, float angle, float width, float anglim1, float anglim2, bool solid = true, const Fpair& tlim = {0,FLT_MAX});
	virtual Obj* copy() const;
	bool judge(const Fpair& pt, bool *val = nullptr) const;
};

/***************************************************************************
******* Specific Motion Classes: Customized New Ones Can Be Created *******/

class Displace: public Motion
{
	using Motor = std::function<Fpair(float)>;
	const Motor motor;
	const Type motor_type;
	Fpair state1, state2;
public:
	Displace(const Fpair& tlim, Motor motor, Type motor_type = Type::relpos);
	virtual Motion* copy();
	void act(Obj* p);
};

class Rotate: public Motion
{
	using Motor = std::function<float(float)>;
	const Motor motor;
	const Type motor_type;
	float state1, state2;
public:
	Rotate(const Fpair& tlim, Motor motor, Type motor_type = Type::relpos);
	virtual Motion* copy();
	void act(Obj* p);
};

class Scale: public Motion
{
	using Motor = std::function<float(float)>;
	const Motor motor;
	const Type motor_type;
	float state1, state2;
public:
	Scale(const Fpair& tlim, Motor motor, Type motor_type = Type::relpos);
	virtual Motion* copy();
	void act(Obj* p);
};

/***************************************************************************
******* Specific Motor Classes: Customized New Ones Can be Created ********/

class Underdamp
{
	float alpha, beta, S, V;
//  larger alpha: smoother oscillation, and slower oscillation (stretched peaks)
//  larger beta: stronger oscillation (larger amptitude)
//  S0: the start position of the curve. Smaller positive S0 will induce alpha sharper and higher peak for the 1/S curve at the beginning.
//  V0: Larger (compared to 0) positive value will induce alpha lower peak of 1/S curve at beginning but will induce several severe peaks after the beginning.
public:
	Underdamp(float alpha = 10, float beta = 4);
	float operator()(float t);
};


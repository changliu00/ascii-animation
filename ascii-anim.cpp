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

#include <sstream> // stringstream
#include <stdexcept> // invalid_argument, out_of_range
#include <algorithm> // minmax
// #include <string>
// #include <functional>
#include <unistd.h> // usleep
#include "ascii-anim.h"

using namespace std;

int Obj::FrameWidth = 0, Obj::FrameHeight = 0;
float Obj::wid_ht = 0;

/***************************************************************************
********************** Basic Classes: Do Not Alter ************************/
////// Object Base Class: for layered union of objects //////
Obj::Obj(const string& name, const Fpair& tlim, const Fpair& pc, initializer_list<Obj*> il):
	name(name), pc(pc), lower(-wid_ht,-1), upper(wid_ht,1), tlim(tlim) {
	objs.insert(objs.end(), il);
}

Obj::~Obj() {
	for(auto p: objs) p->~Obj();
	for(auto p: motions) p->~Motion();
}

Obj* Obj::copy() const {
	Obj* res = new Obj(*this);
	for(int i=0; i<objs.size(); ++i) res->objs[i] = res->objs[i]->copy();
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

Obj* Obj::copy(const string& name) const {
	Obj* res = this->copy();
	const_cast<string&>(res->name) = name;
	return res;
}

Obj* Obj::add_objs(initializer_list<Obj*> il) {
	return *(objs.insert(objs.end(), il));
}

// Motion* Obj::add_motions(initializer_list<Motion*> il) {
// 	return *(motions.insert(motions.end(), il));
// }
Obj* Obj::add_motions(initializer_list<Motion*> il) {
	motions.insert(motions.end(), il);
	return this;
}

void Obj::erase_obj(const string& key) {
	for(auto it = objs.begin(); it != objs.end(); ++it)
		if((*it)->name == key) {
			(*it)->~Obj(); objs.erase(it); return;
		}
	throw out_of_range("Obj \""+key+"\" not found in Obj \""+name+"\"!");
}

void Obj::erase_motion(int idx) {
	motions.at(idx)->~Motion(); motions.erase(motions.begin() + idx);
}

Obj& Obj::operator[](const string& key) {
	for(auto p: objs) if(p->name == key) return (*p);
	throw out_of_range("Obj \""+key+"\" not found in Obj \""+name+"\"!");
}


bool Obj::judge(const Fpair& pt, bool *val) const { // returned value: if pt=(x,y) is in the governing region; val: if pt=(x,y) gets solid printed.
	if(tlim.first <= TimeNow && TimeNow < tlim.second)
		if(lower <= pt && pt <= upper) for(auto rit = objs.rbegin(); rit != objs.rend(); ++rit) if((*rit)->judge(pt, val)) return true;
	return false;
}

void Obj::update(void) {
	if(TimeNow < tlim.second) {
		for(auto p: motions) p->act(this);
		lower = {wid_ht,1}; upper = {-wid_ht,-1};
		for(auto p: objs) {
			p->update();
			union_pt(lower, upper, p->lower, p->upper);
		}
	}
}


Obj* Obj::recenter(const Fpair& pc) { this->pc = pc; return this; }

Obj* Obj::adjcenter(const Fpair& pd) { this->pc += pd; return this; }

Obj* Obj::displace(const Fpair& pd) {
	pc += pd; for(auto p: objs) p->displace(pd); return this;
}

Obj* Obj::place(const Fpair& pp) { return this->displace(pp - pc); }

Obj* Obj::rotate(float dangle, const Fpair& pr) {
	rot_pt_pt(pc, pr, dangle);
	for(auto p: objs) p->rotate(dangle, pr);
	return this;
}

Obj* Obj::rotate(float dangle) { return this->rotate(dangle, pc); }

Obj* Obj::scale(float rate, const Fpair& ps) {
	scl_pt_pt(pc, ps, rate);
	for(auto p: objs) p->scale(rate, ps);
	return this;
}

Obj* Obj::scale(float rate) { return this->scale(rate, pc); }

////// Wrapper Class for Intersection-based Objects //////
IntObj::IntObj(const string& name, bool solid, const Fpair& tlim, const Fpair& pc, initializer_list<Obj*> il):
	Obj(name, tlim, pc, il), solid(solid) {};

Obj* IntObj::copy() const {
	IntObj* res = new IntObj(*this);
	for(int i=0; i<objs.size(); ++i) res->objs[i] = res->objs[i]->copy();
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

bool IntObj::judge(const Fpair& pt, bool *val) const { // returned value: if pt=(x,y) is in the governing region; val: if pt=(x,y) gets solid printed.
	bool gov = false;
	if(tlim.first <= TimeNow && TimeNow < tlim.second)
		if(lower <= pt && pt <= upper) {
			auto rit = objs.rbegin();
			for(; rit != objs.rend(); ++rit) if(!((*rit)->judge(pt))) break;
			if(rit == objs.rend()) gov = true;
		}
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}

void IntObj::update(void) {
	if(TimeNow < tlim.second) {
		for(auto p: motions) p->act(this);
		lower = {-wid_ht,-1}; upper = {wid_ht,1};
		for(auto p: objs) {
			p->update();
			intersect_pt(lower, upper, p->lower, p->upper);
		}
	}
}

////// The Frame Class for containing all the objects and output //////
Frame::Frame(float fm_p_sec):
	Obj("Frame"), time_intv(1/fm_p_sec), x_intv(2.*wid_ht/(FrameWidth-1)),
	y_intv(2./(3*FrameHeight-1)), rewindstr("\033["+to_string(FrameHeight)+"A") {};

void Frame::play(float time_end) {
	int c; bool val; float x, y;
	do {
		TimeNow += time_intv;
		this->update();
		y = 1;
		for(int j=0; j<FrameHeight; ++j) {
			x = -wid_ht;
			for(int i=0; i<FrameWidth; ++i) {
				c = 0;
				for(int n=0; n<3; ++n) {
					if(!this->judge({x, y-n*y_intv}, &val)) val = false;
					c ^= val<<n;
				}
				cout << c[" ''*.'u$"]; // cout << c[" ''\".$u$"];
				x += x_intv;
			}
			cout << '\n';
			y -= 3*y_intv;
		}
		usleep(time_intv*1000000); cout << rewindstr;
	} while(TimeNow < time_end);
}

/***************************************************************************
******** Specific Shape Objects: Customized New Ones Can Be Created *******/
Line::Line(const string& name, float angle, float intsc, float width, bool solid, const Fpair& tlim, const Fpair& pc):
	Obj(name, tlim, pc), angle(angle), intsc(intsc), width(width),
	v(cos(angle), sin(angle)), solid(solid) {};

Obj* Line::copy() const {
	Obj* res = new Line(*this);
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

bool Line::judge(const Fpair& pt, bool *val) const {
	bool gov = false;
	if(tlim.first <= TimeNow && TimeNow < tlim.second) {
		float dist = v*pt;
		gov = (dist > intsc) && (dist <= intsc+width);
	}
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}

void Line::update(void) {
	if(TimeNow < tlim.second) for(auto p: motions) p->act(this);
}

Line* Line::displace(const Fpair& pd) {
	intsc += pd*v;
	pc += pd;
	return this;
}

Line* Line::rotate(float dangle, const Fpair& pr) {
	rot_pt_pt(pc, pr, dangle);
	angle += dangle;
	dangle = v*pr - intsc;
	v = {cos(angle), sin(angle)};
	intsc = v*pr - dangle;
	return this;
}

Line* Line::scale(float rate, const Fpair& ps) {
	scl_pt_pt(pc, ps, rate);
	intsc = (1-rate)*(v*ps) + intsc*rate;
	width *= rate;
	return this;
}

Line* Line::scale(float rate) { // expand/shrink wrt the centerline
	intsc += (1-rate)/2*width;
	width *= rate;
	return this;
}


void Triangle::set_lim(void) {
	Fpair xlim = minmax({p0.first, p1.first, p2.first}), ylim = minmax({p0.second, p1.second, p2.second});
	lower = {xlim.first, ylim.first};
	upper = {xlim.second, ylim.second};
}

Triangle::Triangle(const string& name, const Fpair& p0, const Fpair& p1, const Fpair& p2, bool solid, const Fpair& tlim, const Fpair& pc):
	Obj(name, tlim, pc), p0(p0), p1(p1), p2(p2), v1(p1-p0), v2(p2-p0), v1v2(cross(v1,v2)), solid(solid) {
	if(v1v2 == 0) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "degenerate triangle! (" << p0 << ", " << p1 << ", " << p2 << ")").str());
	this->set_lim();
}

Triangle::Triangle(const string& name, const Fpair& p0, const Fpair& p1, const Fpair& p2, bool solid, const Fpair& tlim):
	Triangle(name, p0, p1, p2, solid, tlim, (1./3)*(p0+p1+p2)) {};

Obj* Triangle::copy() const {
	Obj* res = new Triangle(*this);
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

bool Triangle::judge(const Fpair& pt, bool *val) const {
	bool gov = false;
	if(tlim.first <= TimeNow && TimeNow < tlim.second)
		if(lower <= pt && pt <= upper) {
			Fpair v = pt - p0;
			float v1v_v1v2 = cross(v1, v) / v1v2,
				  vv2_v1v2 = cross(v, v2) / v1v2;
			gov = ((v1v_v1v2 >= 0) && (vv2_v1v2 >= 0) && (v1v_v1v2 + vv2_v1v2 <= 1));
		}
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}

void Triangle::update(void) {
	if(TimeNow < tlim.second) for(auto p: motions) p->act(this);
}

Triangle* Triangle::displace(const Fpair& pd) {
	p0 += pd; p1 += pd; p2 += pd; pc += pd; lower += pd; upper += pd; return this;
}

Triangle* Triangle::rotate(float dangle, const Fpair& pr) {
	rot_pt_pt({&p0, &p1, &p2, &pc}, pr, dangle);
	v1 = p1 - p0; v2 = p2 - p0;
	// v1v2 is unchanged
	this->set_lim(); return this;
}

Triangle* Triangle::scale(float rate, const Fpair& ps) {
	scl_pt_pt({&p0, &p1, &p2, &pc, &lower, &upper}, ps, rate);
	if(rate < 0) swap(lower, upper);
	v1 *= rate; v2 *= rate; v1v2 *= (rate*rate);
	return this;
}


Triangle_isosceles::Triangle_isosceles(const string& name, const Fpair& pc0, float width, float height, float angle, bool solid, const Fpair& tlim, const Fpair& pc):
	Triangle(name, pc0 + Fpair{0, height*2/3}, pc0 + Fpair{-width/2, -height/3}, pc0 + Fpair{width/2, -height/3}, solid, tlim, pc) {
	this->Obj::rotate(angle);
}

Triangle_isosceles::Triangle_isosceles(const string& name, const Fpair& pc0, float width, float height, float angle, bool solid, const Fpair& tlim):
	Triangle(name, pc0 + Fpair{0, height*2/3}, pc0 + Fpair{-width/2, -height/3}, pc0 + Fpair{width/2, -height/3}, solid, tlim, pc0) {
	this->Obj::rotate(angle);
}

Obj* Triangle_isosceles::copy() const {
	Obj* res = new Triangle_isosceles(*this);
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}


void Rectangle::set_lim(void) {
	float dx1 = fabs(wid1/2*v.first-wid2/2*v.second), dx2 = fabs(wid1/2*v.first+wid2/2*v.second),
		  dy1 = fabs(wid1/2*v.second+wid2/2*v.first), dy2 = fabs(wid1/2*v.second-wid2/2*v.first);
	(dx1>dx2)? (lower=pc0-Fpair{dx1, dy2}, upper=pc0+Fpair{dx1, dy2}): (lower=pc0-Fpair{dx2, dy1}, upper=pc0+Fpair{dx2, dy1});
}

Rectangle::Rectangle(const string& name, const Fpair& pc0, float wid1, float wid2, float angle, bool solid, const Fpair& tlim, const Fpair& pc):
	Obj(name, tlim, pc), pc0(pc0), v(cos(angle), sin(angle)), wid1(wid1), wid2(wid2), angle(angle),
	l1("", halfpi+angle, cross(v, pc0) - wid2/2, wid2), l2("", angle, v * pc0 - wid1/2, wid1), solid(solid) {
	this->set_lim();
}

Rectangle::Rectangle(const string& name, const Fpair& pc0, float wid1, float wid2, float angle, bool solid, const Fpair& tlim):
	Rectangle(name, pc0, wid1, wid2, angle, solid, tlim, pc0) {};

Obj* Rectangle::copy() const {
	Obj* res = new Rectangle(*this);
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

bool Rectangle::judge(const Fpair& pt, bool *val) const {
	bool gov = false;
	if(tlim.first <= TimeNow && TimeNow < tlim.second)
		if(lower <= pt && pt <= upper) gov = (l1.judge(pt) && l2.judge(pt));
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}

void Rectangle::update(void) {
	if(TimeNow < tlim.second) for(auto p: motions) p->act(this);
}

Rectangle* Rectangle::displace(const Fpair& pd) {
	pc0 += pd; pc += pd; lower += pd; upper += pd;
	l1.displace(pd); l2.displace(pd);
	return this;
}

Rectangle* Rectangle::rotate(float dangle, const Fpair& pr) {
	rot_pt_pt({&pc0, &pc}, pr, dangle);
	l1.rotate(dangle, pr); l2.rotate(dangle, pr);
	angle += dangle;
	v = {cos(angle), sin(angle)};
	this->set_lim();
	return this;
}

Rectangle* Rectangle::scale(float rate, const Fpair& ps) {
	scl_pt_pt({&pc0, &pc, &lower, &upper}, ps, rate);
	if(rate < 0) swap(lower, upper);
	wid1 *= rate; wid2 *= rate;
	l1.scale(rate, ps); l2.scale(rate, ps);
	return this;
}


void Ellipse::set_lim(void) {
	float delta = sqrt(asq*v.first*v.first + bsq*v.second*v.second);
	lower.first = pc0.first - delta; upper.first = pc0.first + delta;
	delta = sqrt(asq*v.second*v.second + bsq*v.first*v.first);
	lower.second = pc0.second - delta; upper.second = pc0.second + delta;
}

Ellipse::Ellipse(const string& name, const Fpair& pc0, float a, float b, float angle, bool solid, const Fpair& tlim, const Fpair& pc):
	Obj(name, tlim, pc), pc0(pc0), v(cos(angle), sin(angle)), asq(a*a), bsq(b*b), angle(angle), solid(solid) {
	if(asq == 0 || bsq == 0) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "\"a\" and \"b\" cannot be zero!").str());
	this->set_lim();
}

Ellipse::Ellipse(const string& name, const Fpair& pc0, float a, float b, float angle, bool solid, const Fpair& tlim):
	Ellipse(name, pc0, a, b, angle, solid, tlim, pc0) {};

Obj* Ellipse::copy() const {
	Obj* res = new Ellipse(*this);
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

bool Ellipse::judge(const Fpair& pt, bool *val) const {
	bool gov = false;
	if(tlim.first <= TimeNow && TimeNow < tlim.second)
		if(lower <= pt && pt <= upper) {
			float dx = pt.first - pc0.first, dy = pt.second - pc0.second;
			gov = (pow(dx*v.first + dy*v.second, 2)/asq + pow(-dx*v.second + dy*v.first, 2)/bsq < 1);
		}
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}

void Ellipse::update(void) {
	if(TimeNow < tlim.second) for(auto p: motions) p->act(this);
}

Ellipse* Ellipse::displace(const Fpair& pd) {
	pc0 += pd; pc += pd; lower += pd; upper += pd; return this;
}

Ellipse* Ellipse::rotate(float dangle, const Fpair& pr) {
	rot_pt_pt({&pc0, &pc}, pr, dangle);
	angle += dangle;
	v = {cos(angle), sin(angle)};
	this->set_lim();
	return this;
}

Ellipse* Ellipse::scale(float rate, const Fpair& ps) {
	float rate2 = rate*rate;
	asq *= rate2; bsq *= rate2;
	scl_pt_pt({&pc0, &pc, &lower, &upper}, ps, rate);
	if(rate < 0) swap(lower, upper);
	return this;
}


EllArc::EllArc(const string& name, const Fpair& pc0, float a, float b, float angle, float width, float anglim1, float anglim2, bool solid, const Fpair& tlim, const Fpair& pc):
	Ellipse(name, pc0, a, b, angle, solid, tlim, pc), width(width), anglim1(anglim1), anglim2(anglim2) {
	if(width <= 0 || width > 1) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "\"width\" (" << width << ") should be in the range (0, 1]!").str());
	if(anglim1 >= anglim2) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "\"anglim1\" (" << anglim1 << ") should be less than \"anglim2\" (" << anglim2 << ")!").str());
	if(anglim1 > pi || anglim1 < -pi) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "\"anglim1\" (" << anglim1 << ") should be in the range [-\\pi, \\pi]!").str());
	if(anglim2 > pi || anglim2 < -pi) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "\"anglim2\" (" << anglim2 << ") should be in the range [-\\pi, \\pi]!").str());
}

EllArc::EllArc(const string& name, const Fpair& pc0, float a, float b, float angle, float width, float anglim1, float anglim2, bool solid, const Fpair& tlim):
	EllArc(name, pc0, a, b, angle, width, anglim1, anglim2, solid, tlim, pc0) {};

Obj* EllArc::copy() const {
	Obj* res = new EllArc(*this);
	for(int i=0; i<motions.size(); ++i) res->motions[i] = res->motions[i]->copy();
	return res;
}

bool EllArc::judge(const Fpair& pt, bool *val) const {
	bool gov = false;
	if(tlim.first <= TimeNow && TimeNow < tlim.second)
		if(lower <= pt && pt <= upper) {
			float dx = pt.first - pc0.first, dy = pt.second - pc0.second, x = dx*v.first + dy*v.second, y = -dx*v.second + dy*v.first, dist2 = x*x/asq + y*y/bsq, ang;
			if(dist2 < 1 && dist2 >= 1-width) {
				ang = atan2(y, x);
				if(ang >= anglim1 && ang <= anglim2) gov = true;
			}
		}
	if(val!=nullptr) *val = !(gov^solid);
	return gov;
}

// // Displace, rotate: The same as Ellipse!
// EllArc* EllArc::scale(float rate, const Fpair& ps) {
// 	this->Ellipse::scale(rate, ps);
// 	if(rate > 0) width = pow(width, 1/rate);
// 	else if(rate < 0) width = pow(width, -1/rate);
// 	else width = FLT_MIN;
// 	return this;
// }

/***************************************************************************
******* Specific Motion Classes: Customized New Ones Can Be Created *******/

Displace::Displace(const Fpair& tlim, Motor motor, Type motor_type):
	Motion(tlim), motor(motor), motor_type(motor_type) {
	if(motor_type == Type::relpos) state1 = motor(0);
}

Motion* Displace::copy() {
	return new Displace(*this);
}

void Displace::act(Obj* p) {
	if(tlim.first < TimeNow && TimeNow < tlim.second) {
		switch(motor_type) {
			case Type::incre:
				p->displace(motor(TimeNow - tlim.first));
				break;
			case Type::relpos:
				state2 = motor(TimeNow - tlim.first);
				p->displace(state2 - state1);
				state1 = state2;
				break;
			case Type::abspos:
				p->place(motor(TimeNow - tlim.first));
		}
	}
}


Rotate::Rotate(const Fpair& tlim, Motor motor, Type motor_type):
	Motion(tlim), motor(motor), motor_type(motor_type) {
	if(motor_type == Type::relpos) state1 = motor(0);
	else if(motor_type == Type::abspos) throw invalid_argument("\"Type::abspos\" not supported for \"Rotate\" currently!");
}

Motion* Rotate::copy() {
	return new Rotate(*this);
}

void Rotate::act(Obj* p) {
	if(tlim.first < TimeNow && TimeNow < tlim.second) {
		if(motor_type == Type::incre) p->rotate(motor(TimeNow - tlim.first));
		else if(motor_type == Type::relpos) {
			state2 = motor(TimeNow - tlim.first);
			p->rotate(state2 - state1);
			state1 = state2;
		}
	}
}


Scale::Scale(const Fpair& tlim, Motor motor, Type motor_type):
	Motion(tlim), motor(motor), motor_type(motor_type) {
	if(motor_type == Type::relpos ) state1 = motor(0);
	else if(motor_type == Type::abspos) state1 = 1;
}

Motion* Scale::copy() {
	return new Scale(*this);
}

void Scale::act(Obj* p) {
	if(tlim.first < TimeNow && TimeNow < tlim.second) {
		if(motor_type == Type::incre) p->scale(motor(TimeNow - tlim.first));
		else {
			state2 = motor(TimeNow - tlim.first);
			p->scale(state2 / state1);
			state1 = state2;
		}
	}
}


/***************************************************************************
******* Specific Motor Classes: Customized New Ones Can be Created ********/

Underdamp::Underdamp(float alpha, float beta):
	alpha(alpha), beta(beta), S(0), V(0) {};

float Underdamp::operator()(float t) {
	V += (1-S)/alpha-V/beta; S += V; return 1/S;
}


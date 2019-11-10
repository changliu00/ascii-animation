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

#include <sstream>
#include <stdexcept> // invalid_argument, out_of_range
#include <cfloat> // FLT_MAX, FLT_MIN
#include "ascii-anim.h"

using namespace std;

int main()
{
	int fmWid = 128, fmHt = 45, fm_p_sec = 20;
	float t_twth = 2., t_cake = 4., t_shake = 5., t_end = 9.;

	Obj::set_frame(fmWid, fmHt);
	Frame fm(fm_p_sec);

	fm.add_objs({new Obj("bunny"), new Obj("twtythree", {t_twth,FLT_MAX}), new Obj("cake", {t_cake,FLT_MAX})});

	// object: bunny
	Obj* faceout = new Obj("faceout", {0, FLT_MAX}, {0,0}, {
			(new Ellipse("", {0,0}, .3, .3))->displace({-.3, .3}),
			(new Ellipse("", {0,0}, .18, .18))->displace({-.50, 0.}),
			(new Ellipse("", {0,0}, .53, .14))->displace({-.13, -.1}),
			(new Ellipse("", {0,0}, .18, .38))->displace({.3, .2}),
			(new Ellipse("", {0,0}, .4, .12))->displace({-.05, .51})
		});
	Obj* facein = faceout->copy("facein")->scale(.89, {0,0})->displace({-.005,.02});
	for(auto p: facein->objs) dynamic_cast<Ellipse*>(p)->solid = false;
	facein->add_objs({(new Rectangle("", {0,0}, .35, .6, 0., false))->displace({0.,.17})});
	Obj* face = new Obj("face", {0, FLT_MAX}, {0,0}, {faceout, facein});

	float eyea = .08*pi;
	float armw = .04;
	fm["bunny"].add_objs({
		(new EllArc("lear", {0,0}, .25, .40, -.1*pi, .4, -halfpi-.30*pi, -halfpi+.4*pi))->displace({-.38, -.02}),
		(new EllArc("rear", {0,0}, .22, .40, +.06*pi, .4, -halfpi-.4*pi, .4*pi))->displace({.40, -.05}),
		(new Triangle("body", {-.18, -.55}, {.34, -.55}, {-.05, .44}))->displace({-.08, 0.}),
		(new Obj("larm", {0, FLT_MAX}, {0,0}, {
			new Ellipse("", {0,0}, .05+armw, .30+armw, .06*pi),
			new Ellipse("", {0,0}, .05, .30, .06*pi, false)
		}))->displace({-.20, -.20}),
		(new Obj("rarm", {0, FLT_MAX}, {0,0}, {
			new Ellipse("", {0,0}, .05+armw, .30+armw, -.18*pi),
			new Ellipse("", {0,0}, .05, .30, -.18*pi, false)
		}))->displace({.15, -.22}),
		(new Obj("head", {0, FLT_MAX}, {0,0}, {face,
			(new Rectangle("leyebow", {0,0}, .15, .02, .18*pi))->displace({-.3, .38}),
			(new Rectangle("reyebow", {0,0}, .15, .02, -.25*pi))->displace({.20, .38}),
			(new EllArc("leye", {0,0}, .13, .12, 0, .4, eyea, pi-eyea))->displace({-.25, .08}),
			(new EllArc("reye", {0,0}, .13, .12, 0, .4, eyea, pi-eyea))->displace({.15, .08}),
			(new EllArc("llip", {0,0}, .12, .10, 0, .5, -halfpi-.34*pi, -.05*pi))->displace({-.18, -.00}),
			(new EllArc("rlip", {0,0}, .12, .10, 0, .5, -pi, -halfpi+.25*pi))->displace({.04, -.00})
		}))->recenter({0.,-.1})->rotate(-.03*pi)
	});
	fm["bunny"]["lear"].recenter(fm["bunny"]["head"].pc);
	fm["bunny"]["rear"].recenter(fm["bunny"]["head"].pc);

	// object: twtythree
	float thwy = .22, thr = 1.22, thw = .7, thwhalf = .05, tha1 = 25./180*pi, tha2 = 75./180*pi, thp = 1.2, thwpadratio = 1.3;
	Obj *pth0u = new EllArc("", {0, +thwy-thwhalf}, thwy*thr, thwy, 0, thw, -halfpi-tha1, halfpi+tha2),
		*pth0d = new EllArc("", {0, -thwy+thwhalf}, thwy*thr, thwy, 0, thw, -halfpi-tha2, halfpi+tha1),
		*ptw1 = new EllArc("", {0, +thwy-thwhalf}, thwy*thr, thwy, 0, thw, -.18*pi, halfpi+tha2),
		*ptw2 = new Rectangle("", {0,-.10}, .10, .6, -.24*pi),
		*ptw3 = new Rectangle("", {0,-.34}, .55, .1);
	fm["twtythree"].add_objs({
		(new Obj("two", {t_twth,FLT_MAX}, {0,-.35}, {ptw1, ptw2, ptw3}))->displace({-.3, 0}),
		(new Obj("three", {t_twth,FLT_MAX}, {0,-.35}, {pth0u, pth0d}))->displace({.3, 0})
	});
	fm["twtythree"].displace({-.6, 0});

	// object: cake
	float cd_h = .30, cd_w = .08, fl_h = .08, fl_prd = .4;
	Obj *candle = new Rectangle("candle", {0,0}, cd_w, cd_h),
		*flame = (new Ellipse("flame", {0,0}, cd_w/2, fl_h))->recenter({0, -fl_h})->displace({0, fl_h+cd_h/2+.03});
	fm["cake"].add_objs({
		new Rectangle("", {0, -.25}, .9, .3),
		candle->copy()->displace({-.30,0}),
		flame->copy()->displace({-.30,0})->add_motions({
			new Scale({t_cake, FLT_MAX}, [&](float t){return .9 + .1*cos(2*pi/fl_prd*t);}, Motion::Type::abspos) }),
		candle->copy()->displace({-.15,0}),
		flame->copy()->displace({-.15,0})->add_motions({
			new Scale({t_cake+fl_prd/2, FLT_MAX}, [&](float t){return .9 + .1*cos(2*pi/fl_prd*t);}, Motion::Type::abspos) }),
		candle->copy()->displace({.08,0}),
		flame->copy()->displace({.08,0})->add_motions({
			new Scale({t_cake, FLT_MAX}, [&](float t){return .9 + .1*cos(2*pi/fl_prd*t);}, Motion::Type::abspos) }),
		candle->copy()->displace({.22,0}),
		flame->copy()->displace({.22,0})->add_motions({
			new Scale({t_cake+fl_prd/2, FLT_MAX}, [&](float t){return .9 + .1*cos(2*pi/fl_prd*t);}, Motion::Type::abspos) }),
		candle->displace({.34,0}),
		flame->displace({.34,0})->add_motions({
			new Scale({t_cake, FLT_MAX}, [&](float t){return .9 + .1*cos(2*pi/fl_prd*t);}, Motion::Type::abspos) })
	});
	fm["cake"].displace({-.5, -.25});

	////// set motions //////
	// motion: bunny
	float ratio1 = .4, ratio2 = 1, horiz = .85, verti = -.1, t_wait = .4;
	vector<Fpair> anchs = {{-Obj::wid_ht-.6*ratio1, .9}, {-.85, 0}, {-.05, -.05}, {horiz, verti}};
	vector<float> axes = {static_cast<float>(anchs[0].first-.03),
						  static_cast<float>(.51*anchs[1].first+.49*anchs[2].first),
						  static_cast<float>(.51*anchs[2].first+.49*anchs[3].first)};

	vector<Fpair> params(axes.size());
	for(int i=0; i<axes.size(); ++i) {
		float tmp1 = pow(anchs[i].first - axes[i], 2), tmp2 = pow(anchs[i+1].first - axes[i], 2);
		if(tmp1 == tmp2) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "cannot determine parabola with points " << anchs[i] << ", " << anchs[i+1] << " and symmetric axis x = " << axes[i] << "!").str());
		params[i].first = (anchs[i].second-anchs[i+1].second) / (tmp1 - tmp2);
		params[i].second = (tmp1*anchs[i+1].second - tmp2*anchs[i].second) / (tmp1 - tmp2);
	}
	fm["bunny"].add_motions({
		new Displace({0, t_twth-t_wait}, [&](float t)->Fpair {
			float x = (anchs.back().first - anchs.front().first)/(t_twth-t_wait)*t + anchs.front().first, y;
			for(int i=0; i<axes.size(); ++i)
				if(anchs[i].first < x && x <= anchs[i+1].first) y = params[i].first*pow(x - axes[i], 2) + params[i].second;
			return {x, y};
		}, Motion::Type::abspos),
		new Scale({0, t_twth-t_wait}, [&](float t){return t/(t_twth-t_wait)*(ratio2-ratio1)+ratio1;}, Motion::Type::abspos)
	});

	// motion: twth
	Underdamp pgud(10, 4);
	fm["twtythree"].add_motions({
		new Scale({t_twth, t_cake}, [&pgud](float t){return 1/pgud(t);}, Motion::Type::abspos)
	});

	// motion: translate
	float scl = .4, disp = .06, pos0 = -2., cake_y = fm["cake"].pc.second;
	fm["twtythree"].add_motions({
		new Displace({t_cake, t_shake}, [&](float t)->Fpair {return {0.,t/(t_shake-t_cake)*disp};}, Motion::Type::incre),
		new Scale({t_cake, t_shake}, [&](float t){return -t/(t_shake-t_cake)*(1.-scl) + 1.;}, Motion::Type::abspos)
	});
	fm["cake"].add_motions({
		new Displace({t_cake, t_shake}, [&](float t)->Fpair {return {t/(t_shake-t_cake)*(-.5-pos0)+pos0, cake_y};}, Motion::Type::abspos)
	});

	// motion: shake
	float shake_period = 1.1, shake_amp = 15./180*pi, ear_amp = 15./180*pi;
	fm["bunny"]["head"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return shake_amp*sin(2*pi/shake_period*t);}) });
	fm["bunny"]["lear"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return ear_amp*sin(2*pi/shake_period*t);}) });
	fm["bunny"]["rear"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return ear_amp*sin(2*pi/shake_period*t);}) });
	fm["twtythree"]["two"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return shake_amp*sin(2*pi/shake_period*t);}) });
	fm["twtythree"]["three"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return shake_amp*sin(2*pi/shake_period*t);}) });

	// Launch it!
	system("clear");
	(void) fm.play(t_end);
	return 0;
}

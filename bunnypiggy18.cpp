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
#include <cfloat> // FLT_MAX, FLT_MIN
#include "ascii-anim.h"

using namespace std;

int main()
{
	int fmWid = 128, fmHt = 45, fm_p_sec = 20;

	float bodywy = .5, bodyr = 1.2, bodyh = .4, bodyp = 1.1,
		  heady = 0, headwy = .35, headr = 1.1, headp = 1.1,
		  bodyy = heady-headwy*.8-bodywy,

		  earx = .045, eary = .17, earwx = .14, earwy = .4, earc = .9, eara = 28./180*pi, earp = 1.1,
		  eyex = .16, eyey = .10, eyewy = .06, eyer = 1.1,
		  bowy = eyey - .05, bowwy = .08, bowr = 1.25, boww = .7, bowa = 60./180*pi,
		  nosey = -.06, nosew = .16, noseh = .17,
		  lipw = .60, lipwy = .10, lipr = 1., lipx = lipwy*lipr-.025, lipy = nosey-noseh*2/3+.03, lipa = pi/3,
		  knoth = .3, knotw = .25, knotcw = .08, knotp = 1.25,

		  pgearx = .21, pgeary = .23, pgearwy = .27, pgearr = .85, pgearc = .65, pgeara = 35./180*pi, pgearp = 1.1,
		  pgnosex = .07, pgnosey = -.08, pgnosewy = .13, pgnoser = 1.4, pgnosew = .04, pgnosehw = .04,
		  pgmouthy = -.18, pgmouthwy = .1, pgmouthr = 1.4, pgmouthw = .55, pgmoutha = 60./180*pi,

		  thwy = .22, thr = 1.22, thw = .7, thwhalf = .05, tha1 = 25./180*pi, tha2 = 75./180*pi, thp = 1.2, thwpadratio = 1.3,
		  bgnwy = 1e-5;
	int thn = 12, bgnn = 12;

	float t_pig = 2, t_3 = 4, t_shine = 7, t_shake = 8, t_end = 12;

	Obj::set_frame(fmWid, fmHt);
	Frame fm(fm_p_sec);
	fm.add_objs({new IntObj("bgn", true, {t_shine,FLT_MAX}), new Obj("bunny"), new Obj("piggy", {t_pig,FLT_MAX}), new IntObj("thup", true, {t_3,t_shine}), new IntObj("thdn", true, {t_3,t_shine}), new Obj("three", {t_shine,FLT_MAX})});

	////// add objects //////
	// object: bunny
	Obj *pbody = new IntObj("body", true, {0,FLT_MAX}, {0,bodyy+bodywy-bodyh/2}, {
		new Ellipse("", {0, bodyy}, bodywy*bodyr, bodywy),
		new Line("", halfpi, bodyy+bodywy-bodyh, bodyh)
	}),
		*pbodyp = pbody->copy("bodypad")->scale(bodyp);
	dynamic_cast<IntObj*>(pbodyp)->solid = false;

	Obj *phalfknot = new Triangle("", {0,0}, {knotw,knoth/2}, {knotw,-knoth/2}),
		*phalfknotp = phalfknot->copy()->scale(knotp);
	dynamic_cast<Triangle*>(phalfknotp)->solid = false;
	Obj *pknot = new Obj("knot", {0,FLT_MAX}, {0,0}, {
		phalfknotp, phalfknotp->copy()->scale(-1,{0,0}),
		phalfknot, phalfknot->copy()->scale(-1,{0,0}),
		new Ellipse("", {0,0}, knotcw*knotp, knotcw*knotp, 0, false),
		new Ellipse("", {0,0}, knotcw, knotcw)
	});

	fm["bunny"].add_objs({
		pbodyp->copy(), pbody->copy(),
		new Obj("head", {0,FLT_MAX}, {0, heady-headwy}, {
			new Ellipse("pad", {0, heady}, headwy*headr*headp, headwy*headp, 0, false),
			(new Obj("lear", {0,FLT_MAX}, {-earx, eary}, {
				new Ellipse("pad", {-earx, eary+earwy*earc}, earwx*earp, earwy*earp, 0, false),
				new Ellipse("ear", {-earx, eary+earwy*earc}, earwx, earwy, 0, true)
			}))->rotate(+eara),
			(new Obj("rear", {0,FLT_MAX}, {+earx, eary}, {
				new Ellipse("pad", {+earx, eary+earwy*earc}, earwx*earp, earwy*earp, 0, false),
				new Ellipse("ear", {+earx, eary+earwy*earc}, earwx, earwy, 0, true)
			}))->rotate(-eara),
			new Ellipse("face", {0, heady}, headwy*headr, headwy),
			new Obj("eyes", {0,t_shake}, {0, eyey}, {
				new Ellipse("l", {-eyex, eyey}, eyewy*eyer, eyewy, 0, false),
				new Ellipse("r", {+eyex, eyey}, eyewy*eyer, eyewy, 0, false)
			}),
			new Obj("bows", {t_shake,FLT_MAX}, {0, bowy}, {
				new EllArc("l", {-eyex, bowy}, bowwy*bowr, bowwy, 0, boww, halfpi-bowa, halfpi+bowa, false),
				new EllArc("r", {+eyex, bowy}, bowwy*bowr, bowwy, 0, boww, halfpi-bowa, halfpi+bowa, false)
			}),
			new Triangle_isosceles("nose", {0, nosey}, nosew, noseh, pi, false),
			new EllArc("llip", {-lipx, lipy}, lipwy*lipr, lipwy, 0, lipw, -halfpi-lipa, 0, false),
			new EllArc("rlip", {+lipx, lipy}, lipwy*lipr, lipwy, 0, lipw, -pi, -halfpi+lipa, false),
			pknot->copy()->displace({earx+.15,eary+.13})->rotate(-eara+.1)
		})
	});

	// object: piggy
	float pgear_dx = pgearwy*pgearr*(1-pgearc);
	Obj* ppgear0l = (new IntObj("", true, {t_pig,FLT_MAX}, {-pgear_dx, 0}, {
		new Ellipse("", {0,0}, pgearwy*pgearr, pgearwy),
		new Line("", halfpi, 0), new Line("", pi, pgear_dx)
	}))->displace({pgear_dx,0});
	Obj* ppgear0r = (new IntObj("", true, {t_pig,FLT_MAX}, {pgear_dx, 0}, {
		new Ellipse("", {0,0}, pgearwy*pgearr, pgearwy),
		new Line("", halfpi, 0), new Line("", 0, pgear_dx)
	}))->displace({-pgear_dx,0});
	Obj *ppgear0lp = ppgear0l->copy()->scale(pgearp), *ppgear0rp = ppgear0r->copy()->scale(pgearp);
	dynamic_cast<IntObj*>(ppgear0lp)->solid = false; dynamic_cast<IntObj*>(ppgear0rp)->solid = false;

	fm["piggy"].add_objs({
		pbodyp, pbody,
		new Obj("head", {t_pig, FLT_MAX}, fm["bunny"]["head"].pc, {
			fm["bunny"]["head"]["pad"].copy(),
			(new Obj("lear", {t_pig, FLT_MAX}, {0,0}, {
				ppgear0lp->copy(), ppgear0rp->copy(), ppgear0l->copy(), ppgear0r->copy()
			}))->displace({-pgearx,pgeary})->rotate(+pgeara),
			(new Obj("rear", {t_pig, FLT_MAX}, {0,0}, {
				ppgear0lp, ppgear0rp, ppgear0l, ppgear0r
			}))->displace({+pgearx,pgeary})->rotate(-pgeara),
			fm["bunny"]["head"]["face"].copy(),
			fm["bunny"]["head"]["eyes"].copy(), fm["bunny"]["head"]["bows"].copy(),
			new Obj("nose", {t_pig, FLT_MAX}, {0, pgnosey}, {
				new Ellipse("", {0, pgnosey}, pgnosewy*pgnoser, pgnosewy, 0, false),
				new Ellipse("", {0, pgnosey}, pgnosewy*pgnoser-pgnosew, pgnosewy-pgnosew),
				new Ellipse("", {-pgnosex, pgnosey}, pgnosehw, pgnosehw, 0, false),
				new Ellipse("", {+pgnosex, pgnosey}, pgnosehw, pgnosehw, 0, false)
			}),
			new EllArc("mouth", {0, pgmouthy}, pgmouthwy*pgmouthr, pgmouthwy, 0, pgmouthw, -halfpi-pgmoutha, -halfpi+pgmoutha, false)
		}),
		pknot->displace({0, heady-headwy-.1})
	});

	// object: three
	Obj *pth0u = new EllArc("", {0, +thwy-thwhalf}, thwy*thr, thwy, 0, thw, -halfpi-tha1, halfpi+tha2),
		*pth0d = new EllArc("", {0, -thwy+thwhalf}, thwy*thr, thwy, 0, thw, -halfpi-tha2, halfpi+tha1),
		*pth0up = pth0u->copy(), *pth0dp = pth0d->copy();
	dynamic_cast<EllArc*>(pth0up->scale(thp))->solid = false; dynamic_cast<EllArc*>(pth0up)->width *= thwpadratio;
	dynamic_cast<EllArc*>(pth0dp->scale(thp))->solid = false; dynamic_cast<EllArc*>(pth0dp)->width *= thwpadratio;

	fm["thup"].add_objs({new Obj("three"), new Obj("filter")});
	fm["thdn"].add_objs({new Obj("three"), new Obj("filter")});
	fm["thup"]["three"].add_objs({pth0u->copy(), pth0d->copy()});
	fm["thdn"]["three"].add_objs({pth0u->copy(), pth0d->copy()});
	float filter_dx = 2*thwy*thr / thn, filter_intsc = -thwy*thr;
	bool is_up = true;
	for(int i=0; i<thn; ++i) {
		if(is_up) fm["thup"]["filter"].add_objs({new Line("", 0, filter_intsc, filter_dx)});
		else fm["thdn"]["filter"].add_objs({new Line("", 0, filter_intsc, filter_dx)});
		is_up = (!is_up); filter_intsc += filter_dx;
	}
	fm["three"].add_objs({pth0up, pth0dp, pth0u, pth0d});

	// object: background
	fm["bgn"].add_objs({
		new Ellipse("oval", {0,0}, bgnwy*Obj::wid_ht, bgnwy, 0, true, {t_shine,FLT_MAX}),
		new Obj("angles", {t_shine,FLT_MAX})
	});
	float bgn_da = pi/bgnn;
	Obj *pangle = new IntObj("angle", true, {t_shine,FLT_MAX}, {0,0}, { new Line("", halfpi, 0), new Line("", -halfpi+bgn_da, 0) });
	fm["bgn"]["angles"].add_objs({pangle});
	for(int i=1; i<bgnn; ++i) fm["bgn"]["angles"].add_objs({pangle->copy()->rotate(2*i*bgn_da)});

	////////////////////////////////
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
		// if(tmp1 == tmp2) throw invalid_argument(dynamic_cast<stringstream&&>(stringstream() << "cannot determine parabola with points " << anchs[i] << ", " << anchs[i+1] << " and symmetric axis x = " << axes[i] << "!").str());
		if(tmp1 == tmp2) throw invalid_argument(dynamic_cast<stringstream&&>(stringstream() << "cannot determine parabola with points (" << anchs[i].first << ", " << anchs[i].second << "), (" << anchs[i+1].first << ", " << anchs[i+1].second << ") and symmetric axis x = " << axes[i] << "!").str());
		params[i].first = (anchs[i].second-anchs[i+1].second) / (tmp1 - tmp2);
		params[i].second = (tmp1*anchs[i+1].second - tmp2*anchs[i].second) / (tmp1 - tmp2);
	}
	fm["bunny"].add_motions({
		new Displace({0, t_pig-t_wait}, [&](float t)->Fpair {
			float x = (anchs.back().first - anchs.front().first)/(t_pig-t_wait)*t + anchs.front().first, y;
			for(int i=0; i<axes.size(); ++i)
				if(anchs[i].first < x && x <= anchs[i+1].first) y = params[i].first*pow(x - axes[i], 2) + params[i].second;
			return {x, y};
		}, Motion::Type::abspos),
		new Scale({0, t_pig-t_wait}, [&](float t){return t/(t_pig-t_wait)*(ratio2-ratio1)+ratio1;}, Motion::Type::abspos)
	});

	// motion: piggy
	fm["piggy"].displace({-horiz, verti+.06})->scale(1.1);
	fm["piggy"].recenter(fm["piggy"]["body"].pc);
	Underdamp pgud(10, 4);
	fm["piggy"].add_motions({
		new Scale({t_pig, t_3}, [&pgud](float t){return 1/pgud(t);}, Motion::Type::abspos)
	});

	// motion: three
	float thalpha = 10;
	Underdamp thud(thalpha, 5);
	fm["thup"].add_motions({
		new Displace({t_3, t_shine}, [&](float t)->Fpair {return {0, (1+2*thwy-thwhalf)/(1-1/thalpha)*(1-1/thud(t))};}, Motion::Type::abspos) });
	fm["thdn"].add_motions({
		new Displace({t_3, t_shine}, [&fm](float t)->Fpair {return -fm["thup"].pc;}, Motion::Type::abspos)
	});

	// motion: background
	fm["bgn"]["oval"].add_motions({
		new Scale({t_shine, t_shake}, [&](float t) {return (t/(t_shake-t_shine)*(sqrt(2)+.1) + bgnwy)/bgnwy;}, Motion::Type::abspos) });
	fm["bgn"]["angles"].add_motions({
		new Rotate({t_shine, FLT_MAX}, [](float t) {return .5*t;}) });

	// motion: shake
	float shake_period = 1.1, shake_amp = 20./180*pi, knot_amp = 5./180*pi, ear_amp = eara/2;
	fm["bunny"]["head"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return shake_amp*sin(2*pi/shake_period*t);}) });
	fm["bunny"]["head"]["lear"].add_motions({
		new Rotate({t_shake+shake_period/4, FLT_MAX}, [&](float t) {return ear_amp*sin(2*pi/shake_period*t);}) });
	fm["bunny"]["head"]["rear"].add_motions({
		new Rotate({t_shake+shake_period/4, FLT_MAX}, [&](float t) {return ear_amp*sin(2*pi/shake_period*t);}) });
	fm["piggy"]["head"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return shake_amp*sin(2*pi/shake_period*t);}) });
	fm["piggy"]["knot"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return knot_amp*sin(2*pi/shake_period*t);}) });
	fm["three"].recenter({0,-2*thwy+thwhalf});
	fm["three"].add_motions({
		new Rotate({t_shake, FLT_MAX}, [&](float t) {return shake_amp*sin(2*pi/shake_period*t);}) });

	// Launch it!
	(void) system("clear");
	fm.play(t_end);
	return 0;
}


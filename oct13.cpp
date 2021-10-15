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
#include <cmath> // atan, sin, cos
#include "ascii-anim.h"

using namespace std;

int main()
{
	int fmWid = 144, fmHt = 45, fm_p_sec = 20;

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

		  letterh = .20, letterw = .14, letterr = .05, letterarcr = .7, letterpr = 1.4,

		  htw = .26, hth = .24, htr = .18, htpadratio = 1.1,
		  bgnwy = 1e-5;
	int htn = 12, bgnn = 12;

	float t_pig = 2, t_heart = 4, t_shine = 7, t_shake = 8, t_end = 12;
	float htangle = atan(htw/hth), htsin = sin(htangle), htcos = cos(htangle);

	Obj::set_frame(fmWid, fmHt);
	Frame fm(fm_p_sec);
	fm.add_objs({
		new IntObj("bgn", true, {t_shine,FLT_MAX}),
		new Obj("bunny"),
		new Obj("piggy", {t_pig,FLT_MAX}),
		new IntObj("htup", true, {t_heart,t_shine}),
		new IntObj("htdn", true, {t_heart,t_shine}),
		new Obj("heart", {t_shine,FLT_MAX}),
		new Obj("exclaim"),
	});

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

	// object: heart
	Obj *pheart0tri  = new Triangle_isosceles("", {0, -.33*hth}, 2*htw, hth, pi, true),
		*pheart0cirl = new Ellipse("", {htr*htcos - htw, htr*htsin}, htr, htr, 0, true),
		*pheart0cirr = new Ellipse("", {-htr*htcos +htw, htr*htsin}, htr, htr, 0, true),
		heart0sol = Obj("heart0sol", {0,FLT_MAX}, {0,0}, {pheart0tri->copy(), pheart0cirl->copy(), pheart0cirr->copy()});
	dynamic_cast<Triangle_isosceles*>(pheart0tri)->solid = false;
	dynamic_cast<Ellipse*>(pheart0cirl)->solid = false;
	dynamic_cast<Ellipse*>(pheart0cirr)->solid = false;
	Obj heart0pad = Obj("heart0pad", {0,FLT_MAX}, {0,0}, {pheart0tri, pheart0cirl, pheart0cirr});
	heart0pad.scale(htpadratio);

	fm["htup"].add_objs({new Obj("heart"), new Obj("filter")});
	fm["htdn"].add_objs({new Obj("heart"), new Obj("filter")});
	fm["htup"]["heart"].add_objs({heart0pad.copy(), heart0sol.copy()});
	fm["htdn"]["heart"].add_objs({heart0pad.copy(), heart0sol.copy()});

	float filter_dx = 2*(htw + (1.-htcos)*htr) / htn, filter_intsc = -(htw + (1.-htcos)*htr) - filter_dx;
	bool is_up = true;
	for(int i=0; i<htn+2; ++i) {
		if(is_up) fm["htup"]["filter"].add_objs({new Line("", 0, filter_intsc, filter_dx)});
		else fm["htdn"]["filter"].add_objs({new Line("", 0, filter_intsc, filter_dx)});
		is_up = (!is_up); filter_intsc += filter_dx;
	}
	fm["heart"].add_objs({&heart0pad, &heart0sol});

	// object: letters
	Obj tt0("", {0,FLT_MAX}, {0,0}, {
			new Rectangle("", {0,letterh/2}, letterw+letterr, letterr, 0, true),
			new Rectangle("", {0,-letterr/2}, letterr, letterh, 0, true), }),
		oo0("", {0,FLT_MAX}, {0,0}, {
			new EllArc("", {0,0}, (letterw+letterr)/2, (letterh+letterr)/2, 0, letterarcr, -pi, pi, true), }),
		dd0("", {0,FLT_MAX}, {0,0}, {
			new Rectangle("", {-letterw/2,0}, letterr, letterh, 0, true),
			new EllArc("", {-letterw/2,0}, letterw, (letterh+letterr)/2, 0, letterarcr, -pi*.55, pi*.55, true), }),
		aa0("", {0,FLT_MAX}, {0,0}, {
			new Triangle_isosceles("", {0,-(letterh+0.7*letterr)/6}, letterw+2*0.7*letterr, letterh+0.7*letterr, 0, true),
			new Triangle_isosceles("", {0,-(letterh+0.7*letterr)/6 - (2*0.7*letterr+0.7*letterr/letterw*letterh)/3}, letterw, letterh-0.7*letterr/letterw*letterh, 0, false),
			new Rectangle("", {0,-letterh/3}, letterw/2, 0.7*letterr, 0, true) }),
		yy0("", {0,FLT_MAX}, {0,0}, {
			new Triangle_isosceles("", {0,(letterh-letterr)*(.5-.8/3)}, letterw+2*0.7*letterr, (letterh+letterr)*.8, -pi, true),
			new Triangle_isosceles("", {0,(letterh-letterr)*(.5-.8/3) + (2*0.7*letterr+0.7*letterr/letterw*letterh)/3}, letterw, (letterh+letterr)*.8 - 0.7*letterr/letterw*letterh, -pi, false),
			new Rectangle("", {0,(letterh+letterr)*((1.2-.8)/2 - .5)}, letterr, (letterh+letterr)*(1.2-.8), 0, true) }),
		ii0("", {0,FLT_MAX}, {0,0}, {
			new Rectangle("", {0,letterh/2}, letterw*.8, letterr, 0, true),
			new Rectangle("", {0,0}, letterr, letterh, 0, true),
			new Rectangle("", {0,-letterh/2}, letterw*.8, letterr, 0, true), }),
		ss0("", {0,FLT_MAX}, {0,0}, {
			new EllArc("", {0, +(letterh+letterr)/4}, (letterw+letterr)/2, (letterh+letterr)/4, pi, letterarcr, -halfpi-75./180*pi, halfpi+25./180*pi),
			new EllArc("", {0, -(letterh+letterr)/4}, (letterw+2*letterr)/2, (letterh+letterr)/4, 0, letterarcr, -pi, halfpi+25./180*pi), }),
		hh0("", {0,FLT_MAX}, {0,0}, {
			new Rectangle("", {-letterw/2,0}, letterr, letterh+letterr, 0, true),
			new Rectangle("", {+letterw/2,0}, letterr, letterh+letterr, 0, true),
			new Rectangle("", {0,0}, letterw, letterr, 0, true), }),
		ee0("", {0,FLT_MAX}, {0,0}, {
			new Rectangle("", {-letterw/2,0}, letterr, letterh+letterr/2, 0, true),
			new Rectangle("", {0,+letterh/2}, letterw+letterr, letterr, 0, true),
			new Rectangle("", {-letterr/2,0}, letterw, letterr, 0, true),
			new Rectangle("", {0,-letterh/2}, letterw+letterr, letterr/2, 0, true), }),
		exclm0("", {0,FLT_MAX}, {0,0}, {
			new Ellipse("", {0,letterh/2}, letterr*.8, letterr*.8, 0, true),
			new Triangle_isosceles("", {0,letterh*.1}, letterr*.8*2, letterh-letterr, -pi, true),
			new Ellipse("", {0,-letterh/2}, letterr*.8, letterr*.8, 0, true), });
	Obj ttp("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Rectangle("", {0,letterh/2}, letterw+letterr, letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,-letterr/2}, letterr, letterh, 0, false))->scale(letterpr), }),
		oop("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new EllArc("", {0,0}, (letterw+letterr)/2, (letterh+letterr)/2, 0, letterarcr, -pi, pi, false))->scale(letterpr), }),
		ddp("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Rectangle("", {-letterw/2,0}, letterr, letterh, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new EllArc("", {-letterw/2,0}, letterw, (letterh+letterr)/2, 0, letterarcr, -pi*.55, pi*.55, false))->scale(letterpr), }),
		aap("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Triangle_isosceles("", {0,-(letterh+0.7*letterr)/6}, letterw+2*0.7*letterr, letterh+0.7*letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Triangle_isosceles("", {0,-(letterh+0.7*letterr)/6 - (2*0.7*letterr+0.7*letterr/letterw*letterh)/3}, letterw, letterh-0.7*letterr/letterw*letterh, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,-letterh/3}, letterw/2, 0.7*letterr, 0, false))->scale(letterpr) }),
		yyp("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Triangle_isosceles("", {0,(letterh-letterr)*(.5-.8/3)}, letterw+2*0.7*letterr, (letterh+letterr)*.8, -pi, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Triangle_isosceles("", {0,(letterh-letterr)*(.5-.8/3) + (2*0.7*letterr+0.7*letterr/letterw*letterh)/3}, letterw, (letterh+letterr)*.8 - 0.7*letterr/letterw*letterh, -pi, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,(letterh+letterr)*((1.2-.8)/2 - .5)}, letterr, (letterh+letterr)*(1.2-.8), 0, false))->scale(letterpr) }),
		iip("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Rectangle("", {0,letterh/2}, letterw*.8, letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,0}, letterr, letterh, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,-letterh/2}, letterw*.8, letterr, 0, false))->scale(letterpr) }),
		ssp("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new EllArc("", {0, +(letterh+letterr)/4}, (letterw+letterr)/2, (letterh+letterr)/4, pi, letterarcr, -halfpi-75./180*pi, halfpi+25./180*pi))->scale(letterpr),
			dynamic_cast<Obj*>(new EllArc("", {0, -(letterh+letterr)/4}, (letterw+2*letterr)/2, (letterh+letterr)/4, 0, letterarcr, -pi, halfpi+25./180*pi))->scale(letterpr), }),
		hhp("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Rectangle("", {-letterw/2,0}, letterr, letterh+letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {+letterw/2,0}, letterr, letterh+letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,0}, letterw, letterr, 0, false))->scale(letterpr), }),
		eep("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Rectangle("", {-letterw/2,0}, letterr, letterh+letterr/2, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,+letterh/2}, letterw+letterr, letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {-letterr/2,0}, letterw, letterr, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Rectangle("", {0,-letterh/2}, letterw+letterr, letterr/2, 0, false))->scale(letterpr), }),
		exclmp("", {0,FLT_MAX}, {0,0}, {
			dynamic_cast<Obj*>(new Ellipse("", {0,letterh/2}, letterr*.8, letterr*.8, 0, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Triangle_isosceles("", {0,letterh*.1}, letterr*.8*2, letterh-letterr, -pi, false))->scale(letterpr),
			dynamic_cast<Obj*>(new Ellipse("", {0,-letterh/2}, letterr*.8, letterr*.8, 0, false))->scale(letterpr), });
	Obj tt("", {0,FLT_MAX}, {0,0}, {&tt0}),
		oo("", {0,FLT_MAX}, {0,0}, {&oo0}),
		dd("", {0,FLT_MAX}, {0,0}, {&dd0}),
		aa("", {0,FLT_MAX}, {0,0}, {&aa0}),
		yy("", {0,FLT_MAX}, {0,0}, {&yy0}),
		ii("", {0,FLT_MAX}, {0,0}, {&ii0}),
		ss("", {0,FLT_MAX}, {0,0}, {&ss0}),
		hh("", {0,FLT_MAX}, {0,0}, {&hh0}),
		ee("", {0,FLT_MAX}, {0,0}, {&ee0}),
		exclm("", {0,FLT_MAX}, {0,0}, {&exclm0});
	float letterleft = -1.6, lettery = 0.88;
	float movedown = .4;
	fm["exclaim"].add_objs({
		new Rectangle("rect", {0,.88}, 3.6, .30, 0, false, {0,FLT_MAX}),
		tt.copy()->displace({letterleft, lettery}),
		oo.copy()->displace({(letterleft+=letterw+letterr), lettery}),
		dd.copy()->displace({(letterleft+=letterw+2*letterr), lettery}),
		aa.copy()->displace({(letterleft+=letterw+letterr), lettery}),
		yy.copy()->displace({(letterleft+=letterw+letterr), lettery}),
		ii.copy()->displace({(letterleft+=letterw+4*letterr), lettery}),
		ss.copy()->displace({(letterleft+=letterw+letterr), lettery}),
		tt.copy()->displace({(letterleft+=letterw+4*letterr), lettery}),
		hh.copy()->displace({(letterleft+=letterw+2*letterr), lettery}),
		ee.copy()->displace({(letterleft+=letterw+2*letterr), lettery}),
		dd.copy()->displace({(letterleft+=letterw+4*letterr), lettery}),
		aa.copy()->displace({(letterleft+=letterw+letterr), lettery}),
		yy.copy()->displace({(letterleft+=letterw+letterr), lettery}),
		exclm.copy()->displace({(letterleft+=letterw+letterr), lettery}),
	});

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
		if(tmp1 == tmp2) throw invalid_argument(dynamic_cast<ostringstream&&>(ostringstream() << "cannot determine parabola with points " << anchs[i] << ", " << anchs[i+1] << " and symmetric axis x = " << axes[i] << "!").str());
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
		new Scale({t_pig, t_heart}, [&pgud](float t){return 1/pgud(t);}, Motion::Type::abspos)
	});

	// motion: heart
	float htalpha = 10;
	Underdamp htud(htalpha, 5);
	fm["htup"].add_motions({
		new Displace({t_heart, t_shine}, [&](float t)->Fpair {return {0, (1+2*0.33*hth-htw)/(1-1/htalpha)*(1-1/htud(t))};}, Motion::Type::abspos) });
	fm["htdn"].add_motions({
		new Displace({t_heart, t_shine}, [&fm](float t)->Fpair {return -fm["htup"].pc;}, Motion::Type::abspos)
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
	fm["heart"].recenter({0,-2*-0.33*hth-htw});
	float t_beat = t_shake;
	Underdamp pgud1(10, 4);
	fm["heart"].add_motions({
		new Scale({t_beat, t_beat+shake_period}, [&pgud1](float t){return 1/pgud1(t);}, Motion::Type::abspos),
	});
	t_beat += shake_period;
	Underdamp pgud2(10, 4);
	fm["heart"].add_motions({
		new Scale({t_beat, t_beat+shake_period}, [&pgud2](float t){return 1/pgud2(t);}, Motion::Type::abspos),
	});
	t_beat += shake_period;
	Underdamp pgud3(10, 4);
	fm["heart"].add_motions({
		new Scale({t_beat, t_beat+shake_period}, [&pgud3](float t){return 1/pgud3(t);}, Motion::Type::abspos),
	});
	t_beat += shake_period;
	Underdamp pgud4(10, 4);
	fm["heart"].add_motions({
		new Scale({t_beat, t_beat+shake_period}, [&pgud4](float t){return 1/pgud4(t);}, Motion::Type::abspos),
	});

	// Launch it!
	(void) system("clear");
	fm.play(t_end);
	return 0;
}


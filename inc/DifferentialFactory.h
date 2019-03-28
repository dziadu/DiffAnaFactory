/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DIFFERENTIALFACTORY_H
#define DIFFERENTIALFACTORY_H

#include "DistributionFactory.h"
#include "DifferentialContext.h"

#ifdef HAVE_HISTASYMMERRORS
#include "TH2DA.h"
#endif

class DifferentialFactory;

typedef void (FitCallback)(DifferentialFactory * fac, DistributionFactory * sigfac, int fit_res, TH1 * h, uint x_pos, uint y_pos, uint z_pos);

class DifferentialFactory : public DistributionFactory {
public:
	DifferentialFactory();
	DifferentialFactory(const DifferentialContext & ctx);
	DifferentialFactory(const DifferentialContext * ctx);
	virtual ~DifferentialFactory();

	DifferentialFactory & operator=(const DifferentialFactory & fa);

// 	void getDiffs(bool with_canvases = true);

  virtual void prepare();
	virtual void init();
	virtual void proceed();
	virtual void finalize(bool flag_details = false);
  virtual void rename(const char * newname);
  virtual void chdir(const char * newdir);
  virtual void reset();

	virtual void binnorm();
	virtual void scale(Float_t factor);

	virtual void applyAngDists(double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);

	virtual void applyBinomErrors(TH1 * N);

  bool write(TFile * f/* = nullptr*/, bool verbose = false);
  bool write(const char * filename/* = nullptr*/, bool verbose = false);

	void niceDiffs(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);
	void niceSlices(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);

	void fitDiffHists(DistributionFactory * sigfac, FitterFactory & ff, HistFitParams & stdfit, bool integral_only = false);
	bool fitDiffHist(TH1 * hist, HistFitParams & hfp, double min_entries = 0);

  void setFitCallback(FitCallback * cb) { fitCallback = cb; }
	virtual void prepareDiffCanvas();

private:
  virtual void init_diffs();
  virtual void proceed1();
  virtual void proceed2();
  virtual void proceed3();

public:
  DifferentialContext ctx;
  ExtraDimensionMapper * diffs;
	TCanvas ** c_Diffs;		//!
	TObjArray * objectsFits;		//!

private:
	FitCallback * fitCallback;

	ClassDef(DifferentialFactory, 1);
};

#endif // DIFFERENTIALFACTORY_H

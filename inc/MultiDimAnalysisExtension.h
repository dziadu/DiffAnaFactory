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


#ifndef MULTIDIMANALYSISEXTENSION_H
#define MULTIDIMANALYSISEXTENSION_H

#include "TObject.h"
#include "TString.h"
#include "TChain.h"
#include "Rtypes.h"
#include "TDirectory.h"
#include "RootTools.h"

#include "Dim2DistributionFactory.h"
#include "ExtraDimensionMapper.h"
#include "MultiDimAnalysisContext.h"
#include "SmartFactory.h"
#include "FitterFactory.h"

class TCanvas;
class TChain;
class TF1;
class TFile;
class TGraph;
class TGraphErrors;
class TH1;
class TH1D;
class TH2;
class TH2D;
class TStyle;
class TVirtualPad;

#ifdef HAVE_HISTASYMMERRORS
#include "TH2DA.h"
#endif

class MultiDimAnalysisExtension;

typedef void (FitCallbackMD)(MultiDimAnalysisExtension * fac, int fit_res, TH1 * h, int x_pos, int y_pos);

class MultiDimAnalysisExtension {
public:
	MultiDimAnalysisExtension();
	MultiDimAnalysisExtension(const MultiDimAnalysisContext & ctx);
	MultiDimAnalysisExtension(const MultiDimAnalysisContext * ctx);
	virtual ~MultiDimAnalysisExtension();

	MultiDimAnalysisExtension & operator=(const MultiDimAnalysisExtension & fa);

	void GetDiffs(bool with_canvases = true);

  virtual void init(TH1 * h);
	virtual void proceed();
	virtual void finalize(bool flag_details = false);

	virtual void scale(Float_t factor);

	void niceDiffs(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);
	void niceSlices(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);

	void fitDiffHists(FitterFactory & ff, HistFitParams & stdfit, bool integral_only = false);
	bool fitDiffHist(TH1 * hist, HistFitParams & hfp, double min_entries = 0);

	virtual void prepareDiffCanvas();

	void applyBinomErrors(TH2 * N);
	static void applyBinomErrors(TH2 * q, TH2 * N);

	inline void setFitCallback(FitCallbackMD * cb) { fitCallback = cb; }

  bool write(TFile * f/* = nullptr*/, bool verbose = false);
  bool write(const char * filename/* = nullptr*/, bool verbose = false);

protected:
	virtual void prepare();

private:

public:
	MultiDimAnalysisContext ctx;		//||

  ExtraDimensionMapper * diffs;
	TCanvas ** c_Diffs;		//!
	TObjArray * objectsFits;		//!

protected:

private:
	FitCallbackMD * fitCallback;

	ClassDef(MultiDimAnalysisExtension, 1);
};

#endif // MULTIDIMANALYSISEXTENSION_H

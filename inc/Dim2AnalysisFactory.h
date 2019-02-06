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


#ifndef DIM2ANALYSISFACTORY_H
#define DIM2ANALYSISFACTORY_H

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

class Dim2AnalysisFactory;

typedef void (FitCallback)(Dim2AnalysisFactory * fac, int fit_res, TH1 * h, int x_pos, int y_pos);

class Dim2AnalysisFactory : public Dim2DistributionFactory {
public:
	Dim2AnalysisFactory();
	Dim2AnalysisFactory(const MultiDimAnalysisContext & ctx);
	Dim2AnalysisFactory(const MultiDimAnalysisContext * ctx);
	virtual ~Dim2AnalysisFactory();

	Dim2AnalysisFactory & operator=(const Dim2AnalysisFactory & fa);

	enum Stages { RECO, FIT, SIG, ALL };

	void GetDiffs(bool with_canvases = true);

  virtual void init();
	virtual void proceed();
	virtual void finalize(bool flag_details = false);

	virtual void binnorm();
	virtual void scale(Float_t factor);

	void niceDiffs(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);
	void niceSlices(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);

	void fitDiffHists(FitterFactory & ff, HistFitParams & stdfit, bool integral_only = false);
	bool fitDiffHist(TH1 * hist, HistFitParams & hfp, double min_entries = 0);

	virtual void prepareDiffCanvas();

	void applyAngDists(double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);
	static void applyAngDists(TH2 * h, double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);

	void applyBinomErrors(TH2 * N);
	static void applyBinomErrors(TH2 * q, TH2 * N);

	virtual TH2 ** getSigsArray(size_t & size);

	inline void setFitCallback(FitCallback * cb) { fitCallback = cb; }

  bool write(TFile * f/* = nullptr*/, bool verbose = false);
  bool write(const char * filename/* = nullptr*/, bool verbose = false);

protected:
	virtual void prepare(Dimensions dim);
	virtual bool copyHistogram(TH1 * src, TH1 * dst);

private:

public:
	MultiDimAnalysisContext ctx;		//||

// #ifdef HAVE_HISTASYMMERRORS
// 	TH2DA * hSignalCounter;
// #else
// 	TH2D * hSignalCounter;			//->	// discrete X-Y, signal extracted
// #endif

// 	TCanvas * cDiscreteXYSig;		//->
// 	TCanvas * cDiscreteXYSigFull;	//->

  ExtraDimensionMapper * diffs;
// 	TH1D *** hDiscreteXYDiff;		//[10]	// 3rd var distribution in diff bin
	TCanvas ** c_Diffs;		//!

// 	TH1D ** hSliceXYFitQA;			//[10]	// QA values
// 	TCanvas * cSliceXYFitQA;		//!
// 
// 	TH1D ** hSliceXYChi2NDF;		//[10]	// Chi2/NDF values
// 	TCanvas * cSliceXYChi2NDF;		//!
// 	TCanvas * cSliceXYprojX;		//!
// 
// 	TH1D ** hSliceXYDiff;			//!		// slice of x-var in discrete X-Y
// 	TCanvas * cSliceXYDiff;			//!
// 
// 	TObjArray * objectsDiffs;		//!
// 	TObjArray * objectsSlices;		//!
	TObjArray * objectsFits;		//!

protected:

private:
	FitCallback * fitCallback;

	ClassDef(Dim2AnalysisFactory, 1);
};

#endif // DIM2ANALYSISFACTORY_H

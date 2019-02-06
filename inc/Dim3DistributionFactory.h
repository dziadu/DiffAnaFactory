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


#ifndef DIM3DISTRIBUTIONFACTORY_H
#define DIM3DISTRIBUTIONFACTORY_H

#include "TObject.h"
#include "TString.h"
#include "TChain.h"
#include "Rtypes.h"
#include "TDirectory.h"
#include "RootTools.h"

#include "MultiDimDistributionContext.h"
#include "ExtraDimensionMapper.h"
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
class TH3;
class TH3D;
class TStyle;
class TVirtualPad;

#ifdef HAVE_HISTASYMMERRORS
#include "TH2DA.h"
#endif

class Dim3DistributionFactory : public TObject, public SmartFactory {
public:
	Dim3DistributionFactory();
	Dim3DistributionFactory(const MultiDimDistributionContext & ctx);
	Dim3DistributionFactory(const MultiDimDistributionContext * ctx);
	virtual ~Dim3DistributionFactory();

	Dim3DistributionFactory & operator=(const Dim3DistributionFactory & fa);

	enum Stages { RECO, FIT, SIG, ALL };
	void Init(Stages s = ALL);
	void GetDiffs(bool with_canvases = true);

	void Proceed();
	void Finalize(Stages s = ALL, bool flag_details = false);

	void binnorm();
	void scale(Float_t factor);

	static void niceHisto(TVirtualPad * pad, TH1 * hist, float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);

// 	void niceHists(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);
	void niceHists(RootTools::PadFormat pf, const RootTools::GraphFormat & format);
	void niceDiffs(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);
	void niceSlices(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);

	const char * GetName() const { return ("Factory"/* + ctx.histPrefix*/); }

	void prepareDiffCanvas();
	void prepareSigCanvas(bool flag_details = false);

	void applyAngDists(double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);
	static void applyAngDists(TH2 * h, double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);

	void applyBinomErrors(TH2 * N);
	static void applyBinomErrors(TH2 * q, TH2 * N);

	TH3 ** getSigsArray(size_t & size);

private:
	void prepare();
	bool copyHistogram(TH1 * src, TH1 * dst);

public:
	MultiDimDistributionContext ctx;		//||

// #ifdef HAVE_HISTASYMMERRORS
// 	TH2DA * hSignalXY;
// #else
	TH3D * hSignalXY;			//->	// discrete X-Y, signal extracted
// #endif
	TCanvas * cSignalXY;			//->
// 
// 	TCanvas * cDiscreteXYSig;		//->
// 	TCanvas * cDiscreteXYSigFull;	//->

// 	TH1D *** hDiscreteXYDiff;		//[10]	// 3rd var distribution in diff bin

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

	ClassDef(Dim3DistributionFactory, 1);
};

#endif // DIM3DISTRIBUTIONFACTORY_H

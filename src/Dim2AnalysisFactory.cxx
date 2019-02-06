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

#ifndef __CINT__

#include <fstream>
#include <string>
#include "getopt.h"

#include "TCanvas.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TError.h"
#include "TF1.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TH2.h"
#include "TImage.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TVector.h"

#endif /* __CINT__ */

#include "RootTools.h"
#include "Dim2AnalysisFactory.h"

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

// Processing data bar width
const Int_t bar_dotqty = 10000;
const Int_t bar_width = 20;

const Option_t h1opts[] = "h,E1";

const TString flags_fit_a = "B,Q,0";
const TString flags_fit_b = "";

Dim2AnalysisFactory::Dim2AnalysisFactory()
  : SmartFactory("null")
  , ctx(MultiDimAnalysisContext())
  , hSignalCounter(nullptr)
  , cSignalCounter(nullptr)
  , diffs(nullptr)
  , c_Diffs(nullptr)
  , objectsFits(nullptr)
  , dim_version(DIM1)
  , fitCallback(nullptr)
{
  prepare(DIM2);
}

Dim2AnalysisFactory::Dim2AnalysisFactory(const MultiDimAnalysisContext & context)
  : SmartFactory(context.AnaName())
  , ctx(context)
  , hSignalCounter(nullptr)
  , cSignalCounter(nullptr)
  , diffs(nullptr)
  , c_Diffs(nullptr)
  , objectsFits(nullptr)
  , dim_version(DIM1)
  , fitCallback(nullptr)
{
  prepare(DIM2);
}

Dim2AnalysisFactory::Dim2AnalysisFactory(const MultiDimAnalysisContext * context)
  : SmartFactory(context->AnaName())
  , ctx(*context)
  , hSignalCounter(nullptr)
  , cSignalCounter(nullptr)
  , diffs(nullptr)
  , c_Diffs(nullptr)
  , objectsFits(nullptr)
  , dim_version(DIM1)
  , fitCallback(nullptr)
{
  prepare(DIM2);
}

Dim2AnalysisFactory::~Dim2AnalysisFactory()
{
	gSystem->ProcessEvents();
  delete diffs;

	if (objectsFits)	delete objectsFits;
}

Dim2AnalysisFactory & Dim2AnalysisFactory::operator=(const Dim2AnalysisFactory & fa)
{
	Dim2AnalysisFactory * nthis = this;//new Dim2AnalysisFactory(fa.ctx);

	nthis->ctx = fa.ctx;
	nthis->ctx.histPrefix = fa.ctx.histPrefix;

// 	nthis->objectsDiffs = new TObjArray();
// 	nthis->objectsSlices = new TObjArray();
	nthis->objectsFits = new TObjArray();

// 	nthis->objectsDiffs->SetName(ctx.histPrefix + "Diffs");
// 	nthis->objectsSlices->SetName(ctx.histPrefix + "Slices");
	nthis->objectsFits->SetName(ctx.histPrefix + "Fits");

	copyHistogram(fa.hSignalCounter, hSignalCounter);
// 	copyHistogram(fa.hSignalWithCutsXY, hSignalWithCutsXY);
// 	copyHistogram(fa.hDiscreteXY, hDiscreteXY);

// 	for (uint i = 0; i < ctx.cx.bins; ++i)
// 	{
// 		for (uint j = 0; j < ctx.cy.bins; ++j)
// 		{
// 			copyHistogram(fa.hDiscreteXYDiff[i][j], hDiscreteXYDiff[i][j]);
// 		}
// 	}
// 
// 	for (uint i = 0; i < ctx.cx.bins; ++i)
// 	{
// 		copyHistogram(fa.hSliceXYDiff[i], hSliceXYDiff[i]);
// 		nthis->objectsSlices->AddLast(hSliceXYDiff[i]);
// 
// 		copyHistogram(fa.hSliceXYFitQA[i], hSliceXYFitQA[i]);
// 		nthis->objectsSlices->AddLast(hSliceXYFitQA[i]);
// 
// 		copyHistogram(fa.hSliceXYChi2NDF[i], hSliceXYChi2NDF[i]);
// 		nthis->objectsSlices->AddLast(hSliceXYChi2NDF[i]);
// 	}
// 
// 	copyHistogram(fa.hDiscreteXYSig, hDiscreteXYSig);
	return *nthis;
}

void Dim2AnalysisFactory::prepare(Dimensions dim)
{
  if (dim_version < dim)
  {
//     delete diffs;           diffs = nullptr;
//     delete hSignalCounter;  hSignalCounter = nullptr;
  }

  dim_version = dim;

	ctx.update();

	objectsFits = new TObjArray();
	objectsFits->SetName(ctx.histPrefix + "Fits");
}

static TString format_hist_axes(const MultiDimAnalysisContext & ctx)
{
	return TString::Format(";%s%s;%s%s",
                         ctx.x.label.Data(), ctx.x.format_unit().Data(),
                         ctx.y.label.Data(), ctx.y.format_unit().Data());
}

TString format_hist_Vaxis(const MultiDimAnalysisContext & ctx)
{
	return TString::Format("%s%s", ctx.V.label.Data(), ctx.V.format_unit().Data());
}

void Dim2AnalysisFactory::init()
{
	Int_t can_width = 800, can_height = 600;
// 	TString hname, cname;
	TString htitle = format_hist_axes(ctx);
// 	TString htitlec = format_hist_caxes(ctx);
	TString htitlez = format_hist_Vaxis(ctx);

	// input histograms
	if (!hSignalCounter)
	{
		// Signal

#ifdef HAVE_HISTASYMMERRORS
		hSignalCounter = RegTH2<TH2DA>("@@@d/h_@@@a_Signal", htitle,
#else
    hSignalCounter = RegTH2<TH2D>("@@@d/h_@@@a_Signal", htitle,
#endif
				ctx.x.bins, ctx.x.min, ctx.x.max,
				ctx.y.bins, ctx.y.min, ctx.y.max);
		hSignalCounter->GetZaxis()->SetTitle(htitlez);
  }

  if (!cSignalCounter)
    cSignalCounter = RegCanvas("@@@d/c_@@@a_Signal", htitle, can_width, can_height);

  diffs = new ExtraDimensionMapper(ctx.histPrefix.Data(), hSignalCounter, ctx.V, "@@@d/diffs/h_@@@a_Signal");

		// Signal with cut
// 		if (ctx.useCuts())
// 		{
// 			hname = "@@@d/h_@@@a_Lambda";
// 			cname = "@@@d/c_@@@a_Lambda";
// 			hSignalWithCutsXY = RegTH2<TH2D>(hname, htitle, ctx.x.bins, ctx.x.min, ctx.x.max,
// 				ctx.y.bins, ctx.y.min, ctx.y.max);
// 			hSignalWithCutsXY->GetZaxis()->SetTitle(htitlez);
// 
// 			cSignalWithCutsXY = RegCanvas(cname, htitle, can_width, can_height);
// 		}
	
// 		if (ctx.useClip())
// 		{
// 			hname = "@@@d/h_@@@a_LambdaInvMass";
// 			cname = "@@@d/c_@@@a_LambdaInvMass";
// 
// 			// Lambda: X vs Y
// 			if (ctx.cx.bins_arr and ctx.cy.bins_arr)
// 				hDiscreteXY = RegTH2<TH2D>(hname, htitlec, ctx.cx.bins, ctx.cx.bins_arr, ctx.cy.bins, ctx.cy.bins_arr);
// 			else
// 				hDiscreteXY = RegTH2<TH2D>(hname, htitlec,
// 					ctx.cx.bins, ctx.cx.min, ctx.cx.max,
// 					ctx.cy.bins, ctx.cy.min, ctx.cy.max);
// 
// 			hDiscreteXY->GetZaxis()->SetTitle(htitlez);
// 
// 			cDiscreteXY = RegCanvas(cname, htitle, can_width, can_height);
// 
// 			cname += "Full";
// 			cDiscreteXYFull = RegCanvas(cname, htitle, can_width, can_height);
// 		}

	// histograms for fitting
// 	if (s == FIT or s == ALL)
	{
// 		if (ctx.useDiff())
// 		{
// 			GetDiffs();
// 		}
	}

// 	if (s == SIG or s == ALL)
	{
// 		if (ctx.useDiff())
// 		{
// 			objectsSlices = new TObjArray();
// 			objectsSlices->SetName(ctx.histPrefix + "Slices");
// 
// 			hSliceXYDiff = new TH1D*[ctx.cx.bins];
// 			hSliceXYFitQA = new TH1D*[ctx.cx.bins];
// 			hSliceXYChi2NDF = new TH1D*[ctx.cx.bins];
// 
// 			for (uint i = 0; i < ctx.cx.bins; ++i)
// 			{
// 				// common title for all
// 				htitle = TString::Format("#Lambda: %s[%d]=%.1f-%.1f;%s%s;Stat", "X", i,
// 										 ctx.cx.min+ctx.cx.delta*i, ctx.cx.min+ctx.cx.delta*(i+1),
// 										 ctx.y.label.Data(), ctx.y.format_unit().c_str());
// 
// 				// slices
// 				hname = TString::Format("@@@d/Slices/h_@@@a_LambdaInvMassSlice_%s%02d", "X", i);
// 				hSliceXYDiff[i] = RegTH1<TH1D>(hname, htitle, ctx.cy.bins, ctx.cy.min, ctx.cy.max);
// 				objectsSlices->AddLast(hSliceXYDiff[i]);
// 
// 				// Fit QA
// 				hname = TString::Format("@@@d/Slices/h_@@@a_LambdaInvMassFitQA_%s%02d", "X", i);
// 				hSliceXYFitQA[i] = RegTH1<TH1D>(hname, htitle,ctx.cy.bins, ctx.cy.min, ctx.cy.max);
// 				objectsSlices->AddLast(hSliceXYFitQA[i]);
// 
// 				// Chi2/NDF
// 				hname = TString::Format("@@@d/Slices/h_@@@a_LambdaInvMassChi2NDF_%s%02d", "X", i);
// 				hSliceXYChi2NDF[i] = RegTH1<TH1D>(hname, htitle, ctx.cy.bins, ctx.cy.min, ctx.cy.max);
// 				objectsSlices->AddLast(hSliceXYChi2NDF[i]);
// 			}
// 
// 			// Lambda: X vs Y
// 			hname = "@@@d/h_@@@a_LambdaInvMassSig";
// 
// #ifdef HAVE_HISTASYMMERRORS
// 			if (ctx.cx.bins_arr and ctx.cy.bins_arr)
// 				hDiscreteXYSig = RegTH2<TH2DA>(hname, htitlec, ctx.cx.bins, ctx.cx.bins_arr, ctx.cy.bins, ctx.cy.bins_arr);
// 			else
// 				hDiscreteXYSig = RegTH2<TH2DA>(hname, htitlec,
// 					ctx.cx.bins, ctx.cx.min, ctx.cx.max,
// 					ctx.cy.bins, ctx.cy.min, ctx.cy.max);
// #else
// 			if (ctx.cx.bins_arr and ctx.cy.bins_arr)
// 				hDiscreteXYSig = RegTH2<TH2D>(hname, htitlec, ctx.cx.bins, ctx.cx.bins_arr, ctx.cy.bins, ctx.cy.bins_arr);
// 			else
// 				hDiscreteXYSig = RegTH2<TH2D>(hname, htitlec,
// 					ctx.cx.bins, ctx.cx.min, ctx.cx.max,
// 					ctx.cy.bins, ctx.cy.min, ctx.cy.max);
// #endif
// 			hDiscreteXYSig->GetZaxis()->SetTitle(htitlez);
// 
// 			cname = "@@@d/c_@@@a_LambdaInvMassSig";
// 			cDiscreteXYSig = RegCanvas(cname, htitle, can_width, can_height);
// 
// 			cname += "Full";
// 			cDiscreteXYSigFull = RegCanvas(cname, htitle, can_width, can_height);
// 
// 			// Slices
// 			cname = "@@@d/Slices/c_@@@a_LambdaInvMassSlice";
// 			cSliceXYDiff = RegCanvas(cname.Data(), "#Lambda: Slice of Y distribution", can_width, can_height, ctx.cx.bins);
// 
// 			// Fit QA
// 			cname = "@@@d/Slices/c_@@@a_LambdaInvMassFitQA";
// 			cSliceXYFitQA = RegCanvas(cname.Data(), "#Lambda: Slice of Y distribution", can_width, can_height, ctx.cx.bins);
// 
// 			// Chi2NDF
// 			cname = "@@@d/Slices/c_@@@a_LambdaInvMassChi2NDF";
// 			cSliceXYChi2NDF = RegCanvas(cname.Data(), "#Lambda: Slice of Y distribution", can_width, can_height, ctx.cx.bins);
// 
// 			cname = "@@@d/Slices/c_@@@a_LambdaInvMassProjX";
// 			cSliceXYprojX = RegCanvas(cname.Data(), "#Lambda: ProjectionsX of Y distribution", can_width, can_height, ctx.cx.bins);
// 		}
	}
}

void Dim2AnalysisFactory::GetDiffs(bool with_canvases)
{
	Int_t can_width = 800, can_height = 600;
	TString hname, htitle, cname;

// 	if (ctx.useDiff())
// 	{
// 		objectsDiffs = new TObjArray();
// 		objectsDiffs->SetName(ctx.histPrefix + "Diffs");
// 
// 		// Lambda: differential plots
// 		hDiscreteXYDiff = new TH1D**[ctx.cx.bins];
// 		if (with_canvases)
// 			c_Diffs = new TCanvas*[ctx.cx.bins];
// 
// 		for (uint i = 0; i < ctx.cx.bins; ++i)
// 		{
// 			hDiscreteXYDiff[i] = new TH1D*[ctx.cy.bins];
// 
// 			for (uint j = 0; j < ctx.cy.bins; ++j)
// 			{
// 				hname = TString::Format("@@@d/Diffs/h_@@@a_LambdaDiff_%s%02d_%s%02d", "X", i, "Y", j);
// 				htitle = TString::Format(
// 					"#Lambda: %s[%d]=%.1f-%.1f, %s[%d]=%.0f-%.0f;M [MeV/c^{2}];Stat",
// 					ctx.cx.label.Data(), i,
// 					ctx.cx.min+ctx.cx.delta*i,
// 					ctx.cx.min+ctx.cx.delta*(i+1),
// 					ctx.cy.label.Data(), j,
// 					ctx.cy.min+ctx.cy.delta*j,
// 					ctx.cy.min+ctx.cy.delta*(j+1));
// 
// 					hDiscreteXYDiff[i][j] = RegTH1<TH1D>(hname, htitle, ctx.V.bins, ctx.V.min, ctx.V.max);
// 
// 					objectsDiffs->AddLast(hDiscreteXYDiff[i][j]);
// 			}
// 
// 			if (with_canvases)
// 			{
// 				cname = TString::Format("@@@d/Diffs/c_@@@a_LambdaDiff_%s%02d", "X", i);
// 				c_Diffs[i] = RegCanvas(cname, "test", can_width, can_height, ctx.cy.bins);
// 			}
// 		}
// 	}
}

void Dim2AnalysisFactory::proceed()
{
	Bool_t isInRange = kFALSE;

	hSignalCounter->Fill(*ctx.x.var, *ctx.y.var, *ctx.var_weight); // FIXME fill it after analysis (fits or so)
// 	if (ctx.useClip() and
// 			*ctx.x.var > ctx.cx.min and *ctx.x.var < ctx.cx.max and
// 			*ctx.y.var > ctx.cy.min and *ctx.y.var < ctx.cy.max)
// 	{
// 		const Int_t xcbin = Int_t( (*ctx.x.var - ctx.cx.min)/ctx.cx.delta );
// 		const Int_t ycbin = Int_t( (*ctx.y.var - ctx.cy.min)/ctx.cy.delta );
// 
// 		hDiscreteXYDiff[xcbin][ycbin]->Fill(*ctx.V.var, *ctx.var_weight);
// 
// 		isInRange = kTRUE;
// 	}

  diffs->Fill2D(*ctx.x.var, *ctx.y.var, *ctx.V.var, *ctx.var_weight);
// 	if (ctx.useCuts() and *ctx.V.var > ctx.cutMin and *ctx.V.var < ctx.cutMax)
// 	{
// 		hSignalWithCutsXY->Fill(*ctx.x.var, *ctx.y.var, *ctx.var_weight);
// 
// 		if (isInRange)
// 		{
// 			hDiscreteXY->Fill(*ctx.x.var, *ctx.y.var, *ctx.var_weight);
// 		}
// 	}
}

void Dim2AnalysisFactory::binnorm()
{
	if (hSignalCounter) hSignalCounter->Scale( 1.0 / ( hSignalCounter->GetXaxis()->GetBinWidth(1) * hSignalCounter->GetYaxis()->GetBinWidth(1) ) );
// 
// 	// Signal with cut
// 	if (ctx.useCuts())
// 	{
// 		if (hSignalWithCutsXY) hSignalWithCutsXY->Scale( 1.0 / ( hSignalWithCutsXY->GetXaxis()->GetBinWidth(1) * hSignalWithCutsXY->GetYaxis()->GetBinWidth(1) ) );
// 	}
// 
// 	if (ctx.useClip())
// 	{
// 		if (hDiscreteXY) hDiscreteXY->Scale( 1.0 / ( hDiscreteXY->GetXaxis()->GetBinWidth(1) * hDiscreteXY->GetYaxis()->GetBinWidth(1) ) );
// 		if (hDiscreteXYSig) hDiscreteXYSig->Scale( 1.0 / ( hDiscreteXYSig->GetXaxis()->GetBinWidth(1) * hDiscreteXYSig->GetYaxis()->GetBinWidth(1) ) );
// 
// // 		if (ctx.useDiff())
// // 		{
// // 			for (uint i = 0; i < ctx.cx.bins; ++i)
// // 			{
// // 				for (uint j = 0; j < ctx.cy.bins; ++j)
// // 				{
// // 					if (hDiscreteXYDiff) hDiscreteXYDiff[i][j]->Scale(factor);
// // 				}
// // 				if (hSliceXYDiff) hSliceXYDiff[i]->Scale(factor);
// // 			}
// // 		}
// 	}
}

void Dim2AnalysisFactory::scale(Float_t factor)
{
	if (hSignalCounter) hSignalCounter->Scale(factor);

	// Signal with cut
// 	if (ctx.useCuts())
// 	{
// 		if (hSignalWithCutsXY) hSignalWithCutsXY->Scale(factor);
// 	}

// 	if (ctx.useClip())
// 	{
// 		if (hDiscreteXY) hDiscreteXY->Scale(factor);
// 		if (hDiscreteXYSig) hDiscreteXYSig->Scale(factor);

// 		if (ctx.useDiff())
// 		{
			for (uint i = 0; i < diffs->getNHists(); ++i)
			{
          TH1 * h = (*diffs)[i];
					if (h) h->Scale(factor);
// 				if (hSliceXYDiff) hSliceXYDiff[i]->Scale(factor);
			}
// 		}
// 	}
}

void Dim2AnalysisFactory::finalize(bool flag_details)
{
// 	switch (s)
// 	{
// 		case RECO:
			prepareSigCanvas(flag_details);
// 			break;
// 		case FIT:
			prepareDiffCanvas();
// 			break;
// 		case SIG:
			prepareSigCanvas(flag_details);
// 			break;
// 		case ALL:
			prepareSigCanvas(flag_details);
			prepareDiffCanvas();
			prepareSigCanvas(flag_details);
// 			break;
// 	}
}

void Dim2AnalysisFactory::niceHisto(TVirtualPad * pad, TH1 * hist, float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY, bool centerX)
{
	RootTools::NicePad(pad, mt, mr, mb, ml);
	RootTools::NiceHistogram(hist, ndivx, ndivy, xls, 0.005, xts, xto, yls, 0.005, yts, yto, centerY, centerX);
}

void Dim2AnalysisFactory::niceHists(RootTools::PadFormat pf, const RootTools::GraphFormat & format)
{
// 	RootTools::NicePad(cSignalXY->cd(), pf);
	RootTools::NiceHistogram(hSignalCounter, format);
	hSignalCounter->GetYaxis()->CenterTitle(kTRUE);
// 
// 	// Signal with cut
// 	if (ctx.useCuts())
// 	{
// 		RootTools::NicePad(cSignalWithCutsXY->cd(), pf);
// 		RootTools::NiceHistogram(hSignalWithCutsXY, format);
// 		hSignalWithCutsXY->GetYaxis()->CenterTitle(kTRUE);
// 	}
// 
// 	if (ctx.useClip())
// 	{
// 		RootTools::NicePad(cDiscreteXY->cd(), pf);
// 		RootTools::NicePad(cDiscreteXYFull->cd(), pf);
// 		RootTools::NiceHistogram(hDiscreteXY, format);
// 		hDiscreteXY->GetYaxis()->CenterTitle(kTRUE);
// 
// 		RootTools::NicePad(cDiscreteXYSig->cd(), pf);
// 		RootTools::NicePad(cDiscreteXYSigFull->cd(), pf);
// 		RootTools::NiceHistogram(hDiscreteXYSig, format);
// 		hDiscreteXYSig->GetYaxis()->CenterTitle(kTRUE);
// 	}
}

void Dim2AnalysisFactory::niceDiffs(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY, bool centerX)
{
	if (diffs)
	{
    for (uint i = 0; i < diffs->getNHists(); ++i)
    {
// 			TVirtualPad * p = c_Diffs[i]->cd(1+i); FIXME
// 			RootTools::NicePad(p, mt, mr, mb, ml);

			TH1 * h = (*diffs)[i];
			RootTools::NiceHistogram(h, ndivx, ndivy, xls, 0.005, xts, xto, yls, 0.005, yts, yto, centerY, centerX);
		}
	}
}

void Dim2AnalysisFactory::niceSlices(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY, bool centerX)
{
// 	if (ctx.useDiff())
// 	{
// 	for (uint i = 0; i < ctx.cx.bins; ++i)
// 	{
// 		TVirtualPad * p = cSliceXYDiff->cd(1+i);
// 		RootTools::NicePad(p, mt, mr, mb, ml);
// 
// 		TH1 * h = hSliceXYDiff[i];
// 		RootTools::NiceHistogram(h, ndivx, ndivy, xls, 0.005, xts, xto, yls, 0.005, yts, yto, centerY, centerX);
// 	}
// 	}
}

void Dim2AnalysisFactory::prepareDiffCanvas()
{
	TLatex * latex = new TLatex();
	latex->SetNDC();
	latex->SetTextSize(0.07);

	TLatex * nflatex = new TLatex();
	nflatex->SetNDC();
	nflatex->SetTextSize(0.07);
	nflatex->SetTextAlign(23);

// 	if (ctx.useDiff())
// 	{
// 		for (uint i = 0; i < ctx.cx.bins; ++i)
// 		{
// 			for (uint j = 0; j < ctx.cy.bins; ++j)
// 			{
// 				c_Diffs[i]->cd(1+j);
// 
// 				TH1 * h = hDiscreteXYDiff[i][j];
// 
// 				h->Draw();
// 				latex->DrawLatex(0.12, 0.85, TString::Format("%02d", j));
// 
// 				TList * flist = h->GetListOfFunctions();
// 				size_t fs = flist->GetEntries();
// 				if (fs < 3)
// 				{
// 					latex->DrawLatex(0.55, 0.85, TString::Format("E=%g", hDiscreteXYDiff[i][j]->GetEntries()));
// 					latex->DrawLatex(0.55, 0.80, TString::Format("R=%g", hDiscreteXYDiff[i][j]->GetRMS()));
// 					latex->DrawLatex(0.55, 0.75, TString::Format("E/R=%g", hDiscreteXYDiff[i][j]->GetEntries() / hDiscreteXYDiff[i][j]->GetRMS()));
// 
// 					nflatex->DrawLatex(0.5, 0.5, "No fit");
// 					continue;
// 				}
// 
// 				Float_t Y_l = ctx.cy.min+ctx.cy.delta*j;
// 				Float_t Y_h = ctx.cy.min+ctx.cy.delta*(j+1);
// 				Float_t X_l = ctx.cx.min+ctx.cx.delta*i;
// 				Float_t X_h = ctx.cx.min+ctx.cx.delta*(i+1);
// 
// 				TF1 * tfLambdaSum = (TF1 *)flist->At(0);
// 				TF1 * tfLambdaSig = (TF1 *)flist->At(1);
// 				TF1 * tfLambdaBkg = (TF1 *)flist->At(2);
// 
// 				tfLambdaSig->SetLineColor(kBlack);
// 				tfLambdaSig->SetLineWidth(1);
// 				tfLambdaSig->SetFillColor(kGray);
// 				tfLambdaSig->SetFillStyle(3000);
// 
// 				tfLambdaBkg->SetLineColor(kGray+2);
// 				tfLambdaBkg->SetLineWidth(1);
// 				// 	tfLambdaBkg->SetLineStyle(7);
// 
// 				tfLambdaSum->SetLineColor(kRed);
// 				tfLambdaSum->SetLineWidth(1);
// 
// 				tfLambdaBkg->Draw("same");
// 				tfLambdaSum->Draw("same");
// 
// 				TH1 * hsigclone = ( (TH1*) h->Clone("hsig") );
// 				hsigclone->Add(tfLambdaBkg, -1);
// 				hsigclone->Delete();
// 
// 				h->SetTitle("");
// 				TLatex * latex = new TLatex();
// 				latex->SetNDC();
// 				latex->SetTextColor(/*36*/1);
// 				latex->SetTextSize(0.06);
// 				Int_t oldalign = latex->GetTextAlign();
// 				Int_t centeralign = 23;
// 
// 				Float_t centerpos = (1-gPad->GetRightMargin()+gPad->GetLeftMargin())/2;
// 
// 				latex->SetTextAlign(centeralign);
// 				latex->DrawLatex(centerpos, 1.01, TString::Format("%.2f < %s < %.2f", X_l, ctx.cx.label.Data(), X_h));
// 				latex->DrawLatex(centerpos, 0.96, TString::Format("%.0f < %s < %.0f", Y_l, ctx.cy.label.Data(), Y_h));
// 				latex->SetTextAlign(oldalign);
// 				latex->SetTextColor(/*36*/1);
// 
// 				int fitnpar = tfLambdaSig->GetNpar();
// 				for (int i = 0; i < fitnpar; ++i)
// 				{
// 					latex->DrawLatex(0.5, 0.81-0.05*i, TString::Format("[%d] %5g#pm%.2g", i,
// 																		tfLambdaSig->GetParameter(i),
// 													 tfLambdaSig->GetParError(i)));
// 				}
// 				latex->DrawLatex(0.5, 0.25, TString::Format("#chi^{2}/ndf = %g",
// 															 tfLambdaSum->GetChisquare()/tfLambdaSum->GetNDF()));
// 				latex->DrawLatex(0.5, 0.20, TString::Format(" %.2g/%d",
// 															 tfLambdaSum->GetChisquare(), tfLambdaSum->GetNDF()));
// 
// 			}
// 		}
// 	}

	latex->Delete();
}

void Dim2AnalysisFactory::prepareSigCanvas(bool flag_details)
{
	TString colzopts = "colz";
	if (flag_details)
		colzopts += ",text";
	TString coltopts = "col,text";

	hSignalCounter->GetXaxis()->SetTitle(ctx.x.format_string());
	hSignalCounter->GetYaxis()->SetTitle(ctx.y.format_string());
	hSignalCounter->GetZaxis()->SetTitle(ctx.z.format_string());

	cSignalCounter->cd(0);
	hSignalCounter->Draw("colz");
	RootTools::NicePalette(hSignalCounter, 0.05);
	RootTools::NoPalette(hSignalCounter);
	gPad->Update();

// 	if (cDiscreteXY)
// 	{
// 		hDiscreteXY->GetXaxis()->SetTitle(haxx);
// 		hDiscreteXY->GetYaxis()->SetTitle(haxy);
// 		hDiscreteXY->GetZaxis()->SetTitle(haxz);
// 
// 		cDiscreteXY->cd(0);
// 		hDiscreteXY->Draw(colzopts);
// 		RootTools::NicePalette(hDiscreteXY, 0.05);
// 		hDiscreteXY->SetMarkerSize(1.6);
// 		gPad->Update();
// 
// 		cDiscreteXYFull->cd(0);
// 		TH2I * h1 = (TH2I *)hSignalCounter->DrawCopy("colz"); FIXME
// 		hDiscreteXY->Draw(coltopts+",same");
// 		RootTools::NoPalette(h1);
// 		gPad->Update();
// 	}

// 	if (cDiscreteXYSig)
// 	{
// 		hDiscreteXYSig->GetXaxis()->SetTitle(haxx);
// 		hDiscreteXYSig->GetYaxis()->SetTitle(haxy);
// 		hDiscreteXYSig->GetZaxis()->SetTitle(haxz);
// 
// 		hDiscreteXYSig->SetMarkerColor(kWhite);
// 		hDiscreteXYSig->SetMarkerSize(1.6);
// 
// 		cDiscreteXYSig->cd(0);
// 		hDiscreteXYSig->Draw(colzopts);
// 		RootTools::NicePalette(hDiscreteXYSig, 0.05);
// 		gPad->Update();
// 
// 		cDiscreteXYSigFull->cd(0);
// 		TH2I * h2 = (TH2I *)hSignalWithCutsXY->DrawCopy("colz");
// 		hDiscreteXYSig->Draw(coltopts+",same");
// 		RootTools::NoPalette(h2);
// 		gPad->Update();
// 	}

	float qa_min = 0.;
	float qa_max = 0.;

	float cn_min = 0.;
	float cn_max = 0.;

// 	for (uint i = 0; i < ctx.cx.bins; ++i)
// 	{
// 		if (cSliceXYDiff)
// 		{
// 			cSliceXYDiff->cd(1+i);
// 			hSliceXYDiff[i]->Draw("E");
// 		}
// 
// 		if (cSliceXYFitQA)
// 		{
// 			cSliceXYFitQA->cd(1+i);
// 			hSliceXYFitQA[i]->Draw("E");
// 
// 			for (uint j = 0; j < ctx.cy.bins; ++j)
// 			{
// 				float cnt = hSliceXYFitQA[i]->GetBinContent(1+j);
// 				float err_lo = cnt - hSliceXYFitQA[i]->GetBinErrorLow(1+j);
// 				float err_up = cnt + hSliceXYFitQA[i]->GetBinErrorUp(1+j);
// 
// 				if (cnt and (err_lo or err_up))
// 				{
// 					if (qa_min == 0.)
// 						qa_min = err_lo;
// 					else
// 						qa_min = err_lo < qa_min ? err_lo : qa_min;
// 
// 					if (qa_max == 0.)
// 						qa_max = err_up;
// 					else
// 						qa_max = err_up > qa_max ? err_up : qa_max;
// 				}
// 			}
// 		}
// 
// 		if (cSliceXYChi2NDF)
// 		{
// 			cSliceXYChi2NDF->cd(1+i);
// 			hSliceXYChi2NDF[i]->Draw("E");
// 
// 			for (uint j = 0; j < ctx.cy.bins; ++j)
// 			{
// 				float cnt = hSliceXYChi2NDF[i]->GetBinContent(1+j);
// 				float err_lo = cnt - hSliceXYChi2NDF[i]->GetBinErrorLow(1+j);
// 				float err_up = cnt + hSliceXYChi2NDF[i]->GetBinErrorUp(1+j);
// 
// 				if (cnt and (err_lo or err_up))
// 				{
// 					if (cn_min == 0.)
// 						cn_min = err_lo;
// 					else
// 						cn_min = err_lo < cn_min ? err_lo : cn_min;
// 
// 					if (cn_max == 0.)
// 						cn_max = err_up;
// 					else
// 						cn_max = err_up > cn_max ? err_up : cn_max;
// 				}
// 			}
// 		}
// 
// 		if (cSliceXYprojX)
// 		{
// 			cSliceXYprojX->cd();
// 
// 			int bmaxx, bmaxy, bmaxz; hDiscreteXYSig->GetMaximumBin(bmaxx, bmaxy, bmaxz);
// 			int bminx, bminy, bminz; hDiscreteXYSig->GetMinimumBin(bminx, bminy, bminz);
// 			double max = hDiscreteXYSig->GetBinContent(bmaxx, bmaxy, bmaxz);
// 			double min = hDiscreteXYSig->GetBinContent(bminx, bminy, bminz);
// 			double ddelta = (max - min) * 0.1;
// 
// 			char buff[1000];
// 			for (uint j = 0; j < ctx.cy.bins; ++j)
// 			{
// 				sprintf(buff, "h_%s_xysig_proj_%d", ctx.histPrefix.Data(), j);
// 				TH1 * h = hDiscreteXYSig->ProjectionX(buff, 1+j, 1+j);
// 				h->SetLineWidth(1);
// 				h->SetLineColor(j*4);
// 				h->SetMarkerStyle(j+20);
// 				h->SetMarkerColor(j*4);
// 
// 				if (j == 0)
// 				{
// 					h->Draw("hist,E,C");
// 					h->GetYaxis()->SetRangeUser(min - ddelta, max + ddelta);
// 				}
// 				else
// 				{
// 					h->Draw("hist,E,C,same");
// 				}
// 			}
// 		}
// 	}

// 	float qa_del = 0.05 * (qa_max - qa_min);
// 	float cn_del = 0.05 * (cn_max - cn_min);
// 
// 	for (uint i = 0; i < ctx.cx.bins; ++i)
// 	{
// 		if (cSliceXYFitQA)
// 		{
// 			cSliceXYFitQA->cd(1+i);
// 			hSliceXYFitQA[i]->GetYaxis()->SetRangeUser(qa_min - qa_del, qa_max + qa_del);
// 		}
// 
// 		if (cSliceXYChi2NDF)
// 		{
// 			cSliceXYChi2NDF->cd(1+i);
// 			hSliceXYChi2NDF[i]->GetYaxis()->SetRangeUser(cn_min - cn_del, cn_max + cn_del);
// 		}
// 	}
}

// TODO this two should be moved somewhere else, not in library
void Dim2AnalysisFactory::applyAngDists(double a2, double a4, double corr_a2, double corr_a4)
{
// 	const size_t hists_num = 4;
// 	TH2 * hist_to_map[hists_num] = { hSignalCounter, hSignalWithCutsXY, hDiscreteXY, hDiscreteXYSig };
// 
// 	for (size_t x = 0; x < hists_num; ++x)
// 		applyAngDists(hist_to_map[x], a2, a4, corr_a2, corr_a4);
}

void Dim2AnalysisFactory::applyAngDists(TH2 * h, double a2, double a4, double corr_a2, double corr_a4)
{
	TF1 * f = new TF1("local_legpol", "angdist", -1, 1);
	f->SetParameter(0, 1.0);
	f->SetParameter(1, a2);
	f->SetParameter(2, a4);

	bool has_corr = false;
	TF1 * f_corr = nullptr;
	if ( corr_a2 != 0.0 or corr_a4 != 0.0 )
	{
		has_corr = true;
		f_corr = new TF1("local_legpol_corr", "angdist", -1, 1);

		f_corr->SetParameter(0, 1.0);
		f_corr->SetParameter(1, corr_a2);
		f_corr->SetParameter(2, corr_a4);
	}

	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		double bin_l = h->GetXaxis()->GetBinLowEdge(x);
		double bin_r = h->GetXaxis()->GetBinUpEdge(x);
		double bin_w = bin_r - bin_l;
		double corr_factor = 1.0;

		double angmap = f->Integral(bin_l, bin_r);

		if (has_corr)
		{
			corr_factor = f_corr->Integral(bin_l, bin_r);
		}
		else
		{
			angmap /= bin_w;
		}

		double scaling_factor = angmap / corr_factor;
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double tmp_val = h->GetBinContent(x, y);
			h->SetBinContent(x, y, tmp_val * scaling_factor);
		}
	}
	f->Delete();
	if (f_corr) f_corr->Delete();
}

// TODO move away
void Dim2AnalysisFactory::applyBinomErrors(TH2* N)
{
// 	const size_t hists_num = 4;
// 	TH2 * hmap[hists_num] = { hSignalCounter, hSignalWithCutsXY, hDiscreteXY, hDiscreteXYSig };
// 
// 	for (size_t x = 0; x < hists_num; ++x)
// 		applyBinomErrors(hmap[x], N);
}

// TODO move away
void Dim2AnalysisFactory::applyBinomErrors(TH2* q, TH2* N)
{
	RootTools::calcBinomialErrors(q, N);
}

TH2** Dim2AnalysisFactory::getSigsArray(size_t & size)
{
	size = 4;

	TH2 ** hmap = new TH2*[size];
	hmap[0] = hSignalCounter;
// 	hmap[1] = hSignalWithCutsXY;
// 	hmap[2] = hDiscreteXY;
// 	hmap[3] = hDiscreteXYSig;

	return hmap;
}

bool Dim2AnalysisFactory::copyHistogram(TH1 * src, TH1 * dst)
{
	if (!src or !dst)
		return false;

	size_t bins_x = src->GetXaxis()->GetNbins();
	size_t bins_y = src->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double bc = src->GetBinContent(x, y);
			double be = src->GetBinError(x, y);
			dst->SetBinContent(x, y, bc);
			dst->SetBinError(x, y, be);
		}
	}

	return true;
}

void Dim2AnalysisFactory::fitDiffHists(FitterFactory & ff, HistFitParams & stdfit, bool integral_only)
{
// // 	FitResultData res;
// 	bool res;
// 
// 	TLatex * nofit_text = new TLatex();
// 	nofit_text->SetTextAlign(23);
// 	nofit_text->SetNDC();
// 
// 	int info_text = 0;
// 	for (uint i = 0; i < ctx.cx.bins; ++i)
// 	{
// 		c_Diffs[i]->Draw(h1opts);
// 		for (uint j = 0; j < ctx.cy.bins; ++j)
// 		{
// 			TVirtualPad * pad = c_Diffs[i]->cd(1+j);
// 			RootTools::NicePad(pad, 0.10, 0.01, 0.15, 0.10);
// 
// 			TH1D * hfit = hDiscreteXYDiff[i][j];
// 			hfit->SetStats(0);
// 			hfit->Draw();
// 			info_text = 0;
// 
// 			if (!integral_only)
// 			{
// 				HistFitParams hfp = stdfit;
// 				FitterFactory::FIND_FLAGS fflags = ff.findParams(hfit, hfp, true);
// 
// 				if (fflags == FitterFactory::NOT_FOUND)
// 				{
// 					hfp.setNewName(hfit->GetName());
// 					ff.insertParameters(hfp);
// 				}
// // 				bool hasfunc = ( fflags == FitterFactory::USE_FOUND);
// 				bool hasfunc = true;
// 
// 				if ( ((!hasfunc) or (hasfunc and !hfp.fit_disabled)) /*and*/ /*(hDiscreteXYDiff[i][j]->GetEntries() > 50)*//* and (hDiscreteXYDiff[i][j]->GetRMS() < 15)*/ )
// 				{
// 					if (( hfit->GetEntries() / hfit->GetRMS() ) < 5)
// 					{
// // 						PR(( hDiscreteXYDiff[i][j]->GetEntries() / hDiscreteXYDiff[i][j]->GetRMS() ));
// //						pad->SetFillColor(40);		// FIXME I dont want colors in the putput
// 						info_text = 1;
// 					}
// 					else
// 					{
// 						if (fflags == FitterFactory::USE_FOUND)
// 							printf("+ Fitting %s with custom function\n", hDiscreteXYDiff[i][j]->GetName());
// 						else
// 							printf("+ Fitting %s with standard function\n", hDiscreteXYDiff[i][j]->GetName());
// 
// 						res = fitDiffHist(hDiscreteXYDiff[i][j], hfp);
// 
// 						if (res)
// 							ff.updateParams(hDiscreteXYDiff[i][j], hfp);
// 
// 						if (fitCallback)
// 							(*fitCallback)(this, res, hDiscreteXYDiff[i][j], i, j);
// 
// // 						FIXME
// // 						std::cout << "    Signal: " << res.signal << " +/- " << res.signal_err << std::endl;
// 
// // 						hSliceXYDiff[i]->SetBinContent(1+j, res.signal);
// // 						hSliceXYDiff[i]->SetBinError(1+j, res.signal_err);
// // 						hDiscreteXYSig->SetBinContent(1+i, 1+j, res.signal);
// // 						hDiscreteXYSig->SetBinError(1+i, 1+j, res.signal_err);
// 
// // 						if (res.mean != 0)
// // 						{
// // 							hSliceXYFitQA[i]->SetBinContent(1+j, res.mean);
// // 							hSliceXYFitQA[i]->SetBinError(1+j, res.sigma);
// // 							hSliceXYChi2NDF[i]->SetBinContent(1+j, res.chi2/res.ndf);
// // 						}
// 					}
// 				}
// 				else
// 				{
// //					pad->SetFillColor(42);
// 					info_text = 2;
// 				}
// 			}
// 			else
// 			{
// // 				FIXME
// // 				res.signal = hDiscreteXYDiff[i][j]->Integral();
// // 
// // 				if (res.signal < 0)  // FIXME old value 500
// // 				{
// // 					res.signal = 0;
// // 				}
// // 
// // 				res.signal_err = RootTools::calcTotalError( hDiscreteXYDiff[i][j], 1, hDiscreteXYDiff[i][j]->GetNbinsX() );
// // 				hSliceXYDiff[i]->SetBinContent(1+j, res.signal);
// // 				hSliceXYDiff[i]->SetBinError(1+j, res.signal_err);
// // 				hDiscreteXYSig->SetBinContent(1+i, 1+j, res.signal);
// // 				hDiscreteXYSig->SetBinError(1+i, 1+j, res.signal_err);
// 
// 				if (fitCallback)
// 					(*fitCallback)(this, -1, hDiscreteXYDiff[i][j], i, j);
// 
// 			}
// 
// 			Double_t hmax = hDiscreteXYDiff[i][j]->GetBinContent(hDiscreteXYDiff[i][j]->GetMaximumBin());
// 			hfit->GetYaxis()->SetRangeUser(0, hmax * 1.1);
// 			hfit->GetYaxis()->SetNdivisions(504, kTRUE);
// 
// 			switch (info_text)
// 			{
// 				case 1:
// 					nofit_text->DrawLatex(0.65, 0.65, "No fit");
// 					break;
// 				case 2:
// 					nofit_text->DrawLatex(0.65, 0.65, "Fit disabled");
// 					break;
// 				default:
// 					break;
// 			}
// 		}
// 
// 		cSliceXYDiff->cd(1+i)/*->Draw()*/;
// 		hSliceXYDiff[i]->Draw(h1opts);
// 	}
// 
// 	cDiscreteXYSig->cd();
// // 	hDiscreteXYSig->SetMarkerColor(kWhite);
// 
// // 	if (flag_details)
// // 		gStyle->SetPaintTextFormat(".3g");
// // 		hDiscreteXYSig->Draw("colz,text10");
// // 		gStyle->SetPaintTextFormat("g");
// // 	else
// // 		hDiscreteXYSig->Draw("colz");
// 
// 	RootTools::NicePalette(hDiscreteXYSig, 0.05);
// 
// 	printf("Raw/fine binning counts:  %f / %f  for %s\n", hDiscreteXY->Integral(), hDiscreteXYSig->Integral(), ctx.histPrefix.Data());
}

bool Dim2AnalysisFactory::fitDiffHist(TH1 * hist, HistFitParams & hfp, double min_entries)
{
	Int_t bin_l = hist->FindBin(hfp.fun_l);
	Int_t bin_u = hist->FindBin(hfp.fun_u);

	// rebin histogram if requested
	if (hfp.rebin != 0)
		hist->Rebin(hfp.rebin);

	// if no data in requested range, nothing to do here
	if (hist->Integral(bin_l, bin_u) == 0)
		return false;

	// remove all saved function, potentially risky move
	// if has stored other functions than fit functions
	hist->GetListOfFunctions()->Clear();

	// declare functions for fit and signal
	TF1 * tfLambdaSum = nullptr;
	TF1 * tfLambdaSig = nullptr;

	// do fit using FitterFactory
	bool res = FitterFactory::fit(hfp, hist, "B,Q", "", min_entries);

	// if fit converged retrieve fit functions from histogram
	// otherwise nothing to do here
	if (res)
	{
		tfLambdaSum = (TF1*)hist->GetListOfFunctions()->At(0);
		tfLambdaSig = (TF1*)hist->GetListOfFunctions()->At(1);
	}
	else
		return false;

	// do not draw Sig function in the histogram
	tfLambdaSig->SetBit(TF1::kNotDraw);

	return res;
}

bool Dim2AnalysisFactory::write(const char* filename, bool verbose)
{
  return SmartFactory::write(filename, verbose)
      && diffs->write(filename, verbose);
}

bool Dim2AnalysisFactory::write(TFile* f, bool verbose)
{
  listRegisteredObjects();
  return SmartFactory::write(f, verbose)
      && diffs->write(f, verbose);
}

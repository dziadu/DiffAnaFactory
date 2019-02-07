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

#include "TLatex.h"
#include "TList.h"
#include "TSystem.h"

#endif /* __CINT__ */

#include "RootTools.h"
#include "MultiDimAnalysisExtension.h"

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

// Processing data bar width
const Int_t bar_dotqty = 10000;
const Int_t bar_width = 20;

const Option_t h1opts[] = "h,E1";

const TString flags_fit_a = "B,Q,0";
const TString flags_fit_b = "";

MultiDimAnalysisExtension::MultiDimAnalysisExtension(MultiDimDefinition::Dimensions dim)
  : mda_ctx(MultiDimAnalysisContext())
  , diffs(nullptr)
  , c_Diffs(nullptr)
  , objectsFits(nullptr)
  , fitCallback(nullptr)
{
}

MultiDimAnalysisExtension::MultiDimAnalysisExtension(const MultiDimAnalysisContext & context)
  : mda_ctx(context)
  , diffs(nullptr)
  , c_Diffs(nullptr)
  , objectsFits(nullptr)
  , fitCallback(nullptr)
{
}

MultiDimAnalysisExtension::MultiDimAnalysisExtension(const MultiDimAnalysisContext * context)
  : mda_ctx(*context)
  , diffs(nullptr)
  , c_Diffs(nullptr)
  , objectsFits(nullptr)
  , fitCallback(nullptr)
{
}

MultiDimAnalysisExtension::~MultiDimAnalysisExtension()
{
	gSystem->ProcessEvents();
  delete diffs;

	if (objectsFits)	delete objectsFits;
}

MultiDimAnalysisExtension & MultiDimAnalysisExtension::operator=(const MultiDimAnalysisExtension & fa)
{
	MultiDimAnalysisExtension * nthis = this;//new MultiDimAnalysisExtension(fa.ctx);

	nthis->mda_ctx = fa.mda_ctx;
	nthis->mda_ctx.name = fa.mda_ctx.name;

	nthis->objectsFits = new TObjArray();

	nthis->objectsFits->SetName(mda_ctx.name + "Fits");

	return *nthis;
}

void MultiDimAnalysisExtension::prepare()
{
	mda_ctx.update();

	objectsFits = new TObjArray();
	objectsFits->SetName(mda_ctx.name + "Fits");
}

void MultiDimAnalysisExtension::init(TH1 * h, SmartFactory * sf)
{
  diffs = new ExtraDimensionMapper(mda_ctx.name.Data(), h,
                                   mda_ctx.V,
                                   "@@@d/diffs/h_@@@a_Signal",
                                   sf);
}

void MultiDimAnalysisExtension::GetDiffs(bool with_canvases)
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

void MultiDimAnalysisExtension::proceed(MultiDimDefinition::Dimensions dim)
{
  if (MultiDimDefinition::DIM3 == dim)
    diffs->Fill3D(*mda_ctx.x.var,
                  *mda_ctx.y.var,
                  *mda_ctx.z.var,
                  *mda_ctx.V.var,
                  *mda_ctx.var_weight);
  else if (MultiDimDefinition::DIM2 == dim)
    diffs->Fill2D(*mda_ctx.x.var,
                  *mda_ctx.y.var,
                  *mda_ctx.V.var,
                  *mda_ctx.var_weight);
  else if (MultiDimDefinition::DIM1 == dim)
    diffs->Fill1D(*mda_ctx.x.var,
                  *mda_ctx.V.var,
                  *mda_ctx.var_weight);
}

void MultiDimAnalysisExtension::proceed1()
{
  diffs->Fill1D(*mda_ctx.x.var,
                *mda_ctx.V.var,
                *mda_ctx.var_weight);
}

void MultiDimAnalysisExtension::proceed2()
{
  diffs->Fill2D(*mda_ctx.x.var,
                *mda_ctx.y.var,
                *mda_ctx.V.var,
                *mda_ctx.var_weight);
}

void MultiDimAnalysisExtension::proceed3()
{
  diffs->Fill3D(*mda_ctx.x.var,
                *mda_ctx.y.var,
                *mda_ctx.z.var,
                *mda_ctx.V.var,
                *mda_ctx.var_weight);
}

void MultiDimAnalysisExtension::scale(Float_t factor)
{
 	if (diffs)
 	{
		for (uint i = 0; i < diffs->getNHists(); ++i)
		{
      TH1 * h = (*diffs)[i];
      if (h) h->Scale(factor);
// 				if (hSliceXYDiff) hSliceXYDiff[i]->Scale(factor);
		}
 	}
}

void MultiDimAnalysisExtension::finalize(bool flag_details)
{
  prepareDiffCanvas();
}

void MultiDimAnalysisExtension::niceDiffs(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY, bool centerX)
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

void MultiDimAnalysisExtension::niceSlices(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY, bool centerX)
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

void MultiDimAnalysisExtension::prepareDiffCanvas()
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

// TODO move away
void MultiDimAnalysisExtension::applyBinomErrors(TH2* N)
{
// 	const size_t hists_num = 4;
// 	TH2 * hmap[hists_num] = { hSignalCounter, hSignalWithCutsXY, hDiscreteXY, hDiscreteXYSig };
// 
// 	for (size_t x = 0; x < hists_num; ++x)
// 		applyBinomErrors(hmap[x], N);
}

// TODO move away
void MultiDimAnalysisExtension::applyBinomErrors(TH2* q, TH2* N)
{
	RootTools::calcBinomialErrors(q, N);
}

void MultiDimAnalysisExtension::fitDiffHists(FitterFactory & ff, HistFitParams & stdfit, bool integral_only)
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

bool MultiDimAnalysisExtension::fitDiffHist(TH1 * hist, HistFitParams & hfp, double min_entries)
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

bool MultiDimAnalysisExtension::write(const char* filename, bool verbose)
{
  return diffs->write(filename, verbose);
}

bool MultiDimAnalysisExtension::write(TFile* f, bool verbose)
{
  return diffs->write(f, verbose);
}

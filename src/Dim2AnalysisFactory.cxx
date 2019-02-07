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

#endif /* __CINT__ */

#include "Dim2AnalysisFactory.h"

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

Dim2AnalysisFactory::Dim2AnalysisFactory()
  : Dim2DistributionFactory()
  , MultiDimAnalysisExtension(MultiDimAnalysisContext())
{
  Dim2DistributionFactory::prepare();
  MultiDimAnalysisExtension::prepare();
}

Dim2AnalysisFactory::Dim2AnalysisFactory(const MultiDimAnalysisContext & context)
  : Dim2DistributionFactory(context)
  , MultiDimAnalysisExtension(context)
{
  Dim2DistributionFactory::prepare();
  MultiDimAnalysisExtension::prepare();
}

Dim2AnalysisFactory::Dim2AnalysisFactory(const MultiDimAnalysisContext * context)
  : Dim2DistributionFactory(context)
  , MultiDimAnalysisExtension(context)
{
  Dim2DistributionFactory::prepare();
  MultiDimAnalysisExtension::prepare();
}

Dim2AnalysisFactory::~Dim2AnalysisFactory()
{
// 	gSystem->ProcessEvents();
}

Dim2AnalysisFactory & Dim2AnalysisFactory::operator=(const Dim2AnalysisFactory & fa)
{
	Dim2AnalysisFactory * nthis = this;//new Dim2AnalysisFactory(fa.ctx);

	*nthis = fa;
	return *nthis;
}

void Dim2AnalysisFactory::init()
{
  Dim2DistributionFactory::init();
  MultiDimAnalysisExtension::init(hSignalCounter);
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

  Dim2DistributionFactory::proceed();

  MultiDimAnalysisContext & ctx = MultiDimAnalysisExtension::ctx;

  MultiDimAnalysisExtension::proceed(DIM2);
}

void Dim2AnalysisFactory::binnorm()
{
  Dim2DistributionFactory::binnorm();
}

void Dim2AnalysisFactory::scale(Float_t factor)
{
  Dim2DistributionFactory::scale(factor);
  MultiDimAnalysisExtension::scale(factor);
}

void Dim2AnalysisFactory::finalize(bool flag_details)
{
  Dim2DistributionFactory::finalize(flag_details);
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

bool Dim2AnalysisFactory::write(const char* filename, bool verbose)
{
  return Dim2DistributionFactory::write(filename, verbose)
      && MultiDimAnalysisExtension::write(filename, verbose);
}

bool Dim2AnalysisFactory::write(TFile* f, bool verbose)
{
return Dim2DistributionFactory::write(f, verbose)
      && MultiDimAnalysisExtension::write(f, verbose);
}

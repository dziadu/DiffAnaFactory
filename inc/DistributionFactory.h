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


#ifndef DISTRIBUTIONFACTORY_H
#define DISTRIBUTIONFACTORY_H

#include "RootTools.h"

#include "DistributionContext.h"
#include "ExtraDimensionMapper.h"
#include "SmartFactory.h"
#include "FitterFactory.h"

#ifdef HAVE_HISTASYMMERRORS
#include "TH2DA.h"
#endif

class DistributionFactory : public TObject, public SmartFactory {
public:
	DistributionFactory();
	DistributionFactory(const DistributionContext & ctx);
	DistributionFactory(const DistributionContext * ctx);
	virtual ~DistributionFactory();

	DistributionFactory & operator=(const DistributionFactory & fa);

  virtual void init();
	virtual void proceed();
	virtual void finalize(bool flag_details = false);

	virtual void binnorm();
	virtual void scale(Float_t factor);

	static void niceHisto(TVirtualPad * pad, TH1 * hist, float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);

// 	void niceHists(float mt, float mr, float mb, float ml, int ndivx, int ndivy, float xls, float xts, float xto, float yls, float yts, float yto, bool centerY = false, bool centerX = false);
	virtual void niceHists(RootTools::PadFormat pf, const RootTools::GraphFormat & format);

	virtual const char * GetName() const { return ("Factory"/* + ctx.histPrefix*/); }

	virtual void prepareSigCanvas(bool flag_details = false);

	virtual void applyAngDists(double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);
	static void applyAngDists(TH1 * h, double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);

	virtual void applyBinomErrors(TH1 * N);
	static void applyBinomErrors(TH1 * q, TH1 * N);

	virtual TH3 ** getSigsArray(size_t & size);

protected:
	virtual void prepare();
	virtual bool copyHistogram(TH1 * src, TH1 * dst);

public:
	DistributionContext ctx;		//||

// #ifdef HAVE_HISTASYMMERRORS
// 	TH2DA * hSignalCounter;
// #else
// 	TH2D * hSignalCounter;			//->	// discrete X-Y, signal extracted
// #endif
  TH1 * hSignalCounter;			//->	// discrete X-Y, signal extracted
	TCanvas * cSignalCounter;			//->

// 	TCanvas * cDiscreteXYSig;		//->
// 	TCanvas * cDiscreteXYSigFull;	//->

	ClassDef(DistributionFactory, 1);
};

#endif // DISTRIBUTIONFACTORY_H

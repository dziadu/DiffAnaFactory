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

#include "Dim2DistributionFactory.h"
#include "MultiDimAnalysisContext.h"
#include "MultiDimAnalysisExtension.h"

#ifdef HAVE_HISTASYMMERRORS
#include "TH2DA.h"
#endif

class Dim2AnalysisFactory : public Dim2DistributionFactory, public MultiDimAnalysisExtension {
public:
	Dim2AnalysisFactory();
	Dim2AnalysisFactory(const MultiDimAnalysisContext & ctx);
	Dim2AnalysisFactory(const MultiDimAnalysisContext * ctx);
	virtual ~Dim2AnalysisFactory();

	Dim2AnalysisFactory & operator=(const Dim2AnalysisFactory & fa);

	void GetDiffs(bool with_canvases = true);

  virtual void init();
	virtual void proceed();
	virtual void finalize(bool flag_details = false);

	virtual void binnorm();
	virtual void scale(Float_t factor);

	void applyAngDists(double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);
	static void applyAngDists(TH2 * h, double a2, double a4, double corr_a2 = 0.0, double corr_a4 = 0.0);

	void applyBinomErrors(TH2 * N);
	static void applyBinomErrors(TH2 * q, TH2 * N);

  bool write(TFile * f/* = nullptr*/, bool verbose = false);
  bool write(const char * filename/* = nullptr*/, bool verbose = false);

public:

	ClassDef(Dim2AnalysisFactory, 1);
};

#endif // DIM2ANALYSISFACTORY_H

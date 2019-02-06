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


#ifndef EXTRADIMENSIONMAPPER_H
#define EXTRADIMENSIONMAPPER_H

#include <TH1.h>
#include <TObject.h>
#include <Rtypes.h>

#include "MultiDimDistributionContext.h"
#include "SmartFactory.h"

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

class ExtraDimensionMapper;

// typedef void (FitCallback)(ExtraDimensionMapper * fac, int fit_res, TH1 * h, int x_pos, int y_pos);

class ExtraDimensionMapper : public TObject, public SmartFactory {
public:
	ExtraDimensionMapper(const std::string & name, TH1 * hist, const AxisCfg & axis, const std::string & dir_and_name);
	virtual ~ExtraDimensionMapper();

  UInt_t getBinsX() const { return nbins_x; }
  UInt_t getBinsY() const { return nbins_y; }
  UInt_t getBinsZ() const { return nbins_z; }

  UInt_t getBin(UInt_t x, UInt_t y = 0, UInt_t z = 0) const;

  TH1D * get(UInt_t x, UInt_t y = 0, UInt_t z = 0);
  TH1D * find(Double_t x, Double_t y = 0.0, Double_t z = 0.0);

  size_t getNHists() const { return nhists; }
  TH1 * operator[](int n) { return histograms[n]; }
  const TH1 * operator[](int n) const { return histograms[n]; }

  void Fill1D(Double_t x, Double_t v, Double_t w = 1.0);
  void Fill2D(Double_t x, Double_t y, Double_t v, Double_t w = 1.0);
  void Fill3D(Double_t x, Double_t y, Double_t z, Double_t v, Double_t w = 1.0);

// 	ExtraDimensionMapper & operator=(const ExtraDimensionMapper & fa);
private:
  void map1D(TH1 * hist, const AxisCfg & axis);
  void map2D(TH1 * hist, const AxisCfg & axis);
  void map3D(TH1 * hist, const AxisCfg & axis);
  void formatName(char * buff, TH1 * hist, UInt_t x, UInt_t y = 0, UInt_t z = 0);

public:
	AxisCfg axis;        //||
	std::string prefix_name;

	UInt_t nhists;
  UInt_t nbins_x, nbins_y, nbins_z;

  TH1 * refHist;
	TH1D ** histograms;   //!

// 	TObjArray * objectsFits;

	ClassDef(ExtraDimensionMapper, 1);

private:
// 	FitCallback * fitCallback;
};

#endif // EXTRADIMENSIONMAPPER_H

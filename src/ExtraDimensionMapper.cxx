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

#include "TSystem.h"

#endif /* __CINT__ */

#include "ExtraDimensionMapper.h"

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

// const Option_t h1opts[] = "h,E1";

// const TString flags_fit_a = "B,Q,0";
// const TString flags_fit_b = "";

ExtraDimensionMapper::ExtraDimensionMapper(const std::string & name, TH1 * hist, const AxisCfg & axis, const std::string & dir_and_name)
  : SmartFactory("")
  , axis(axis)
  , prefix_name(dir_and_name)
  , ref_hist(hist)
{
  nbins_x = hist->GetNbinsX();
  nbins_y = hist->GetNbinsY();
  nbins_z = hist->GetNbinsZ();
  nhists = nbins_x * nbins_y * nbins_z;

  histograms = new TH1D*[nhists];
  if (nbins_y == 0)
    map1D(hist, axis);
  else if (nbins_z == 0)
    map2D(hist, axis);
  else
    map3D(hist, axis);

//   objectsFits = new TObjArray();
// 	objectsFits->SetName(ctx.histPrefix + "Fits");
}

ExtraDimensionMapper::ExtraDimensionMapper(const std::string & name, TH1 * hist, const AxisCfg & axis, const std::string & dir_and_name, SmartFactory * sf)
  : SmartFactory("")
  , axis(axis)
  , prefix_name(dir_and_name)
  , ref_hist(hist)
{
  nbins_x = hist->GetNbinsX();
  nbins_y = hist->GetNbinsY();
  nbins_z = hist->GetNbinsZ();
  nhists = nbins_x * nbins_y * nbins_z;

  setSource(sf->getSource());
  setSourceName(sf->getSourceName());
  setTarget(sf->getTarget());
  setTargetName(sf->getTargetName());
  chdir(sf->directory_name().c_str());
  rename(sf->objects_name().c_str());

  histograms = new TH1D*[nhists];
  if (nbins_y == 0)
    map1D(hist, axis);
  else if (nbins_z == 0)
    map2D(hist, axis);
  else
    map3D(hist, axis);

//   objectsFits = new TObjArray();
// 	objectsFits->SetName(ctx.histPrefix + "Fits");
}

ExtraDimensionMapper::~ExtraDimensionMapper()
{
	gSystem->ProcessEvents();
  delete [] histograms;
}

UInt_t ExtraDimensionMapper::getBin(UInt_t x, UInt_t y, UInt_t z) const
{
  return z * (nbins_x * nbins_y) + y * nbins_x + x;
}

void ExtraDimensionMapper::map1D(TH1* hist, const AxisCfg& axis)
{
  char buff[1024];
  for (UInt_t i = 0; i < nbins_x; ++i) {
    formatName(buff, hist, i);
    if (axis.bins_arr) {
      std::cerr << "Histogram bins arrays are not supported yet." << std::endl;
      std::abort();
    } else {
      histograms[getBin(i, 0, 0)] = RegTH1<TH1D>(buff, buff, axis.bins, axis.min, axis.max);
    }
  }
}

void ExtraDimensionMapper::map2D(TH1* hist, const AxisCfg& axis)
{
  char buff[1024];
  for (UInt_t i = 0; i < nbins_x; ++i) {
    for (UInt_t j = 0; j < nbins_y; ++j) {
      formatName(buff, hist, i, j);
      if (axis.bins_arr) {
        std::cerr << "Histogram bins arrays are not supported yet." << std::endl;
        std::abort();
      } else {
        histograms[getBin(i, j, 0)] = RegTH1<TH1D>(buff, buff, axis.bins, axis.min, axis.max);
      }
    }
  }
}

void ExtraDimensionMapper::map3D(TH1* hist, const AxisCfg& axis)
{
  char buff[1024];
  for (UInt_t i = 0; i < nbins_x; ++i) {
    for (UInt_t j = 0; j < nbins_y; ++j) {
      for (UInt_t k = 0; k < nbins_z; ++k) {
        formatName(buff, hist, i, j, k);
        if (axis.bins_arr) {
          std::cerr << "Histogram bins arrays are not supported yet." << std::endl;
          std::abort();
        } else {
          histograms[getBin(i, j, k)] = RegTH1<TH1D>(buff, buff, axis.bins, axis.min, axis.max);
        }
      }
    }
  }
}

void ExtraDimensionMapper::formatName(char* buff, TH1* hist, UInt_t x, UInt_t y, UInt_t z)
{
  if (nbins_y == 0)
    sprintf(buff, "%s_X%02d", prefix_name.c_str(), x);
  else if (nbins_z == 0)
    sprintf(buff, "%s_X%02d_Y%02d", prefix_name.c_str(), x, y);
  else
    sprintf(buff, "%s_X%02d_Y%02d_Z%02d", prefix_name.c_str(), x, y, z);
}


// ExtraDimensionMapper & ExtraDimensionMapper::operator=(const ExtraDimensionMapper & edm)
// {
// 	ExtraDimensionMapper * nthis = this;//new ExtraDimensionMapper(fa.ctx);
// 
// 	nthis->axis = edm.axis;
// 	nthis->nhists = edm.nhists;
//   objectsFits = new TObjArray();
// nthis->objectsFits->SetName(ctx.histPrefix + "Fits");
// 	for (uint i = 0; i < nthis->nhists; ++i)
// 	{
// 		copyHistogram(edm.histograms[i], nthis->histograms[i]);
// 	}
// 
// 	return *nthis;
// }

TH1D * ExtraDimensionMapper::get(UInt_t x, UInt_t y, UInt_t z)
{
  if (x >= nbins_x or y >= nbins_y or z >= nbins_z)
    return nullptr;

  return histograms[getBin(x, y, z)];
}

TH1D * ExtraDimensionMapper::find(Double_t x, Double_t y, Double_t z)
{
  UInt_t bin = ref_hist->FindBin(x, y, z);
  Int_t bx, by, bz;
  ref_hist->GetBinXYZ(bin, bx, by, bz);
  return histograms[getBin(bx-1, by-1, bz-1)];
}


void ExtraDimensionMapper::Fill1D(Double_t x, Double_t v, Double_t w)
{
  UInt_t bin = ref_hist->FindBin(x);
  Int_t bx, by, bz;
  ref_hist->GetBinXYZ(bin, bx, by, bz);
  histograms[getBin(bx-1, by-1, bz-1)]->Fill(v, w);
}

void ExtraDimensionMapper::Fill2D(Double_t x, Double_t y, Double_t v, Double_t w)
{
  UInt_t bin = ref_hist->FindBin(x, y);
  Int_t bx, by, bz;
  ref_hist->GetBinXYZ(bin, bx, by, bz);
  histograms[getBin(bx-1, by-1, bz-1)]->Fill(v, w);
}

void ExtraDimensionMapper::Fill3D(Double_t x, Double_t y, Double_t z, Double_t v, Double_t w)
{
  UInt_t bin = ref_hist->FindBin(x, y, z);
  Int_t bx, by, bz;
  ref_hist->GetBinXYZ(bin, bx, by, bz);
  histograms[getBin(bx-1, by-1, bz-1)]->Fill(v, w);
}

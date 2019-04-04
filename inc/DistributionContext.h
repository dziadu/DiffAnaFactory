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


#ifndef DISTRIBUTIONCONTEXT_H
#define DISTRIBUTIONCONTEXT_H

#include <TNamed.h>

class AxisCfg
{
// class AxisCfg : public TObject {
public:
	AxisCfg();
	AxisCfg(const AxisCfg & a);
	AxisCfg & operator=(const AxisCfg & a);
	bool operator==(const AxisCfg & ctx);
	bool operator!=(const AxisCfg & ctx);

	virtual ~AxisCfg() {}

	TString label;			// label for the axis
	TString unit;			// unit for the axis
// 	TString title;			// title for the axis

	UInt_t bins;			// number of bins
	Double_t min;			// minimum axis value
	Double_t max;			// maximum axis value
	Double_t * bins_arr;	//! here one can put custom bins division array
	mutable Double_t delta;	// CAUTION: overriden by validate(), do not set by hand
  Float_t * var;		//!	here is the address of the variable which is used to fill data

	TString format_unit() const;
  TString format_string() const;
  TString format_hist_string(const char * title = nullptr,
                             const char * ylabel = "Counts")
                            const;

	static TString format_unit(const char * unit);
	static TString format_unit(const TString & unit);

  void print() const;

	ClassDef(AxisCfg, 1);
};

enum Dimensions {
  DIM0, DIM1, DIM2, DIM3
};

class DistributionContext : public TNamed
{
public:
  Dimensions dim; // define dimension
	// config
	mutable TString name;      // prefix for histograms
	TString dir_name;
	TString hist_name;         // name for histograms
	TString diff_var_name;

  TString label;
  TString unit;
  TString axis_text;

	AxisCfg x, y, z;     // x, y are two dimensions, V is a final Variable axis

	// cut range when useCut==kTRUE
// 	Double_t cutMin;			// Cut: min
// 	Double_t cutMax;			// Cut: max

	// variables to use for diff analysis
	Float_t * var_weight;	//!
	// variable used for cuts when cutCut==kTRUE

	DistributionContext();
	DistributionContext(Dimensions dim);
	DistributionContext(const DistributionContext & ctx);
	virtual ~DistributionContext();

	DistributionContext & operator=(const DistributionContext & ctx);
	bool operator==(const DistributionContext & ctx);
	bool operator!=(const DistributionContext & ctx);

	virtual bool update();
	virtual int validate() const;

	virtual void format_diff_axis();
  virtual TString format_hist_axes(const char * title = nullptr) const;

	virtual const char * AnaName() const { return name.Data(); }

	virtual bool findJsonFile(const char * initial_path, const char * filename, int search_depth = -1);
	virtual bool configureFromJson(const char * name);
	virtual bool configureToJson(const char * name, const char * jsonfile);

  void print() const;

protected:
	TString json_fn;
	bool json_found;

  ClassDef(DistributionContext, 1);
};

#endif // DISTRIBUTIONCONTEXT_H

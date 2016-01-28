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


#ifndef DIFFANALYSISCONTEXT_H
#define DIFFANALYSISCONTEXT_H

#include "TNamed.h"
#include "TString.h"
#include "Rtypes.h"

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
	Double_t * bins_arr;	// here one can put custom bins division array
	mutable Double_t delta, cdelta;	// CAUTION: overriden by validate(), do not set by hand
	Float_t * var;		//!	here is the address of the variable which is used to fill data

	std::string format_unit();

	static std::string format_unit(const char * unit);
	static std::string format_unit(const TString & unit);
	static std::string format_unit(const std::string & unit);

	ClassDef(AxisCfg, 2);
};

class DiffAnalysisContext : public TNamed
{
public:
	// config
	TString histPrefix;	// prefix for histograms
	mutable TString ctxName;	// prefix for histograms

	AxisCfg x, y, z;
	AxisCfg cx, cy;

	// cut range when useCut==kTRUE
	Double_t cutMin;			// Cut: min
	Double_t cutMax;			// Cut: max

	// variables to use for diff analysis
	Float_t * var_weight;	//!
	// variable used for cuts when cutCut==kTRUE

	DiffAnalysisContext();
	DiffAnalysisContext(const DiffAnalysisContext & ctx);
	virtual ~DiffAnalysisContext();

	DiffAnalysisContext & operator=(const DiffAnalysisContext & ctx);
	bool operator==(const DiffAnalysisContext & ctx);
	bool operator!=(const DiffAnalysisContext & ctx);

	void update() const;
	bool validate() const;

	// flags
	inline bool useCuts() const { return (cutMin or cutMax); }
	inline bool useClip() const { return cx.bins; }
	inline bool useDiff() const { return (z.bins and useClip()); }

	inline virtual const char * GetName() const { return ctxName.Data(); }
    inline virtual void SetName(const char* name) { ctxName = name; }
	const char * AnaName() const { return histPrefix.Data(); }

	bool findJsonFile(const char * initial_path, const char * filename, int search_depth = -1);
	bool configureFromJson(const char * name);
	bool configureToJson(const char * name, const char * jsonfile);

private:

	TString json_fn;
	bool json_found;
	ClassDef(DiffAnalysisContext, 1);
};

#endif // DIFFANALYSISCONTEXT_H

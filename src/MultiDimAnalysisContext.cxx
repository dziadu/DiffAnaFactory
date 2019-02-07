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

#include "MultiDimAnalysisContext.h"
#include <json/json.h>

#include <iostream>
#include <fstream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

void MultiDimAnalysisContext::format_V_axis()
{
	TString hunit = "1/";
  if (x.bins) hunit += x.unit.Data();
  if (y.bins) hunit += y.unit.Data();
  if (z.bins) hunit += z.unit.Data();

  UInt_t dim_cnt = 0;
  if (x.bins) ++dim_cnt;
  if (y.bins) ++dim_cnt;
  if (z.bins) ++dim_cnt;

	TString htitle = TString::Format("d^{%d}%s/", dim_cnt, diff_var_name.Data());
  if (x.bins) htitle += TString("d")+x.label.Data();
  if (y.bins) htitle += TString("d")+y.label.Data();
  if (z.bins) htitle += TString("d")+z.label.Data();

	V.label = htitle;
	V.unit = hunit;
}

MultiDimAnalysisContext::MultiDimAnalysisContext()
{
	// config
	name = "Dummy";	// prefix for histograms

	// basic
	// x and y binning for full range

	// cut range when useCut==kTRUE
// 	cutMin = cutMax = 0;

	// variables to use for diff analysis
	var_weight = 0;
	// variable used for cuts when cutCut==kTRUE
}

MultiDimAnalysisContext::MultiDimAnalysisContext(const MultiDimAnalysisContext & ctx) : MultiDimDistributionContext(ctx)
{
	*this = ctx;
	name = ctx.name;
}


MultiDimAnalysisContext::~MultiDimAnalysisContext()
{}

extern bool jsonReadTStringKey(const Json::Value & jsondata, const char * key, TString & target);
extern bool jsonReadIntKey(const Json::Value & jsondata, const char * key, int & target);
extern bool jsonReadUIntKey(const Json::Value & jsondata, const char * key, uint & target);
extern bool jsonReadFloatKey(const Json::Value & jsondata, const char * key, float & target);
extern bool jsonReadDoubleKey(const Json::Value & jsondata, const char * key, double & target);

bool MultiDimAnalysisContext::configureFromJson(const char * name)
{
	std::ifstream ifs(json_fn.Data());
	if (!ifs.is_open())
		return false;

	std::cout << "  Found JSON config file for " << name << std::endl;
	Json::Value ana, cfg, axis;
	Json::Reader reader;

	bool parsing_success = reader.parse(ifs, ana);

	if (!parsing_success)
	{
		std::cout << "  Parsing failed\n";
		return false;
	}
	else
		std::cout << "  Parsing successfull\n";

	if (!ana.isMember(name))
	{
		std::cout << "  No data for " << name << std::endl;
		return false;
	}

	cfg = ana[name];

	const size_t axis_num = 3;
	const char * axis_labels[axis_num] = { "x", "y", "V" };
	AxisCfg * axis_ptrs[axis_num] = { &x, &y, &V };

	for (uint i = 0; i < axis_num; ++i)
	{
		if (!cfg.isMember(axis_labels[i]))
			continue;
			
		axis = cfg[axis_labels[i]];

// 		jsonReadTStringKey(axis, "title", axis_ptrs[i]->title);
		jsonReadTStringKey(axis, "label", axis_ptrs[i]->label);
		jsonReadTStringKey(axis, "unit", axis_ptrs[i]->unit);
// 		jsonReadIntKey(axis, "bins", axis_ptrs[i]->bins);
		jsonReadUIntKey(axis, "bins", axis_ptrs[i]->bins);
		jsonReadDoubleKey(axis, "min", axis_ptrs[i]->min);
		jsonReadDoubleKey(axis, "max", axis_ptrs[i]->max);
	}

	ifs.close();
	return true;
}

bool MultiDimAnalysisContext::configureToJson(const char * name, const char * jsonfile)
{
	(void)jsonfile;

	Json::Value ana, cfg, axis;

	cfg["title"]	= "d^{2}N/dp_{t}dy.{cm}";

	axis["bins"]	= 100;
	axis["min"]		= 0;
	axis["max"]		= 100;
	axis["label"]	= "xlabel";
	axis["var"]		= "y.{cm}";

	cfg["x"] = axis;

	axis["bins"]	= 100;
	axis["min"]		= 0;
	axis["max"]		= 100;
	axis["label"]	= "ylabel";
	axis["var"]		= "p_{t}";

	cfg["y"] = axis;

	axis["bins"]	= 100;
	axis["min"]		= 0;
	axis["max"]		= 100;
	axis["label"]	= "Vlabel";
	axis["var"]		= "none";

	cfg["V"] = axis;

	ana[name] = cfg;

	std::cout << ana;

// 	Json::StyledWriter sw;
// 	std::cout << sw.write(ana);
	
// 	Json::FastWriter fw;
// 	std::cout << fw.write(ana);

	return true;
}

MultiDimAnalysisContext & MultiDimAnalysisContext::operator=(const MultiDimAnalysisContext & ctx)
{
// 	histPrefix = ctx.histPrefix;
  MultiDimDistributionContext::operator=(ctx);
	name = ctx.name;
	V = ctx.V;

	return *this;
}

bool MultiDimAnalysisContext::operator==(const MultiDimAnalysisContext & ctx)
{
  bool res = (MultiDimDistributionContext)*this == (MultiDimDistributionContext)ctx;
  if (!res) return false;

	if (this->V != ctx.V)
	{
		fprintf(stderr, "Different number of z bins: %d vs %d\n", this->V.bins, ctx.V.bins);
		return false;
	}

	return true;
}

bool MultiDimAnalysisContext::operator!=(const MultiDimAnalysisContext & ctx)
{
	return !operator==(ctx);
}

void MultiDimAnalysisContext::print() const
{
  MultiDimDistributionContext::print();
  V.print();
}

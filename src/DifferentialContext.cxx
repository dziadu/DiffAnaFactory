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

#include "DifferentialContext.h"
#include <json/json.h>

#include <iostream>
#include <fstream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

void DifferentialContext::format_diff_axis()
{
	TString hunit = "1/";
  if (DIM1 <= dim) hunit += x.unit.Data();
  if (DIM2 <= dim) hunit += y.unit.Data();
  if (DIM3 == dim) hunit += z.unit.Data();

  UInt_t dim_cnt = 0;
  TString htitle;
  if (DIM3 == dim) dim_cnt = 3;
  if (DIM2 == dim) dim_cnt = 2;
  if (DIM1 == dim) dim_cnt = 1;

  if (DIM1 < dim)
    htitle = TString::Format("d^{%d}%s/", dim_cnt, diff_var_name.Data());
  else
    htitle = TString::Format("d%s/", diff_var_name.Data());

  if (DIM1 <= dim) htitle += TString("d")+x.label.Data();
  if (DIM2 <= dim) htitle += TString("d")+y.label.Data();
  if (DIM3 == dim) htitle += TString("d")+z.label.Data();

	label = htitle;
	unit = hunit;
}

DifferentialContext::DifferentialContext()
{
	// basic
	// x and y binning for full range

	// cut range when useCut==kTRUE
// 	cutMin = cutMax = 0;

	// variables to use for diff analysis
	var_weight = 0;
	// variable used for cuts when cutCut==kTRUE
}

DifferentialContext::DifferentialContext(const DifferentialContext & ctx) : DistributionContext(ctx)
{
	*this = ctx;
	name = ctx.name;
}


DifferentialContext::~DifferentialContext()
{}

extern bool jsonReadTStringKey(const Json::Value & jsondata, const char * key, TString & target);
extern bool jsonReadIntKey(const Json::Value & jsondata, const char * key, int & target);
extern bool jsonReadUIntKey(const Json::Value & jsondata, const char * key, uint & target);
extern bool jsonReadFloatKey(const Json::Value & jsondata, const char * key, float & target);
extern bool jsonReadDoubleKey(const Json::Value & jsondata, const char * key, double & target);

bool DifferentialContext::configureFromJson(const char * name)
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

bool DifferentialContext::configureToJson(const char * name, const char * jsonfile)
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

DifferentialContext & DifferentialContext::operator=(const DifferentialContext & ctx)
{
// 	histPrefix = ctx.histPrefix;
  DistributionContext::operator=(ctx);
	name = ctx.name;
	V = ctx.V;

	return *this;
}

bool DifferentialContext::operator==(const DifferentialContext & ctx)
{
  bool res = (DistributionContext)*this == (DistributionContext)ctx;
  if (!res) return false;

	if (this->V != ctx.V)
	{
		fprintf(stderr, "Different number of z bins: %d vs %d\n", this->V.bins, ctx.V.bins);
		return false;
	}

	return true;
}

bool DifferentialContext::operator!=(const DifferentialContext & ctx)
{
	return !operator==(ctx);
}

void DifferentialContext::print() const
{
  DistributionContext::print();
  V.print();
}

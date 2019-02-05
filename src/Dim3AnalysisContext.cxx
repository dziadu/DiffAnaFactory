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


#include "Dim3AnalysisContext.h"
#include <json/json.h>


#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

void Dim3AnalysisContext::format_z_axis()
{
	TString hunit = TString::Format("1/%s%s", x.unit.Data(), y.unit.Data());
	TString htitle = TString::Format("d^{2}%s/d%sd%s", diff_var_name.Data(), x.label.Data(), y.label.Data());

	z.label = htitle;
	z.unit = hunit;
}

Dim3AnalysisContext::Dim3AnalysisContext() : Dim2AnalysisContext(), json_found(false)
{
	// config
	TString histPrefix = "Dummy";	// prefix for histograms	

	// basic
	// x and y binning for full range

	// cut range when useCut==kTRUE
// 	cutMin = cutMax = 0;

	// variables to use for diff analysis
// 	var_weight = 0;
	// variable used for cuts when cutCut==kTRUE
}

Dim3AnalysisContext::Dim3AnalysisContext(const Dim3AnalysisContext & ctx) : Dim2AnalysisContext()
{
	*this = ctx;
	histPrefix = ctx.histPrefix;
}

void Dim3AnalysisContext::update() const
{
	z.delta = ((z.max - z.min)/z.bins);

  Dim2AnalysisContext::update();
}

bool Dim3AnalysisContext::validate() const
{
	if (!z.var)
		return false;

  Dim2AnalysisContext::validate();
// 	update();

	return true;
}

Dim3AnalysisContext::~Dim3AnalysisContext()
{}

extern bool jsonReadTStringKey(const Json::Value & jsondata, const char * key, TString & target);
extern bool jsonReadIntKey(const Json::Value & jsondata, const char * key, int & target);
extern bool jsonReadUIntKey(const Json::Value & jsondata, const char * key, uint & target);
extern bool jsonReadFloatKey(const Json::Value & jsondata, const char * key, float & target);
extern bool jsonReadDoubleKey(const Json::Value & jsondata, const char * key, double & target);

bool Dim3AnalysisContext::configureFromJson(const char * name)
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

	const size_t axis_num = 4;
	const char * axis_labels[axis_num] = { "x", "y", "z", "V" };
	AxisCfg * axis_ptrs[axis_num] = { &x, &y, &z, &V};

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

bool Dim3AnalysisContext::configureToJson(const char * name, const char * jsonfile)
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
	axis["label"]	= "zlabel";
	axis["var"]		= "none";

	cfg["z"] = axis;

	ana[name] = cfg;

	std::cout << ana;

// 	Json::StyledWriter sw;
// 	std::cout << sw.write(ana);
	
// 	Json::FastWriter fw;
// 	std::cout << fw.write(ana);

	return true;
}

bool Dim3AnalysisContext::findJsonFile(const char * initial_path, const char * filename, int search_depth)
{
	const size_t max_len = 1024*16;
	int depth_counter = 0;
	char * resolv_name = new char[max_len];
	char * test_path = new char[max_len];
	struct stat buffer;

	strncpy(test_path, initial_path, max_len);

	char * ret_val = 0;
	while (true)
	{
		ret_val = realpath(test_path, resolv_name);
		if (!ret_val)
			break;

		std::string name = resolv_name;
		name += "/";
		name += filename;

		if (stat (name.c_str(), &buffer) == 0)
		{
			json_found = true;
			json_fn = name;
			break;
		}

		strncpy(test_path, resolv_name, max_len);
		strncpy(test_path+strlen(test_path), "/..", 4);

		if (strcmp(resolv_name, "/") == 0)
			break;

		++depth_counter;
		if (search_depth >= 0 and (depth_counter > search_depth))
			break;
	}

	if (json_found)
		printf(" Found json config at %s\n", json_fn.Data());

	delete [] resolv_name;
	delete [] test_path;

	return json_found;
}

Dim3AnalysisContext & Dim3AnalysisContext::operator=(const Dim3AnalysisContext & ctx)
{
// 	histPrefix = ctx.histPrefix;
	ctxName = ctx.ctxName;

	x = ctx.x;
	y = ctx.y;
	z = ctx.z;

	var_weight = ctx.var_weight;

	diff_var_name = ctx.diff_var_name;

	return *this;
}

bool Dim3AnalysisContext::operator==(const Dim3AnalysisContext & ctx)
{
	if (this->x != ctx.x)
	{
		fprintf(stderr, "Different number of x bins: %d vs %d\n", this->x.bins, ctx.x.bins);
		return false;
	}

	if (this->y != ctx.y)
	{
		fprintf(stderr, "Different number of y bins: %d vs %d\n", this->y.bins, ctx.y.bins);
		return false;
	}

	if (this->z != ctx.z)
	{
		fprintf(stderr, "Different number of z bins: %d vs %d\n", this->z.bins, ctx.z.bins);
		return false;
	}

	return true;
}

bool Dim3AnalysisContext::operator!=(const Dim3AnalysisContext & ctx)
{
	return !operator==(ctx);
}

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

#include "DistributionContext.h"
#include <json/json.h>

#include <sys/stat.h>
#include <iostream>
#include <fstream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

AxisCfg::AxisCfg()
  : bins(0)
  , min(0)
  , max(0)
  , bins_arr(nullptr)
  , delta(0.0)
  , var(nullptr)
{
}

AxisCfg::AxisCfg(const AxisCfg & a)
{
	*this = a;
}

AxisCfg & AxisCfg::operator=(const AxisCfg & a)
{
	label = a.label;
	unit = a.unit;

	bins = a.bins;
	min = a.min;
	max = a.max;
	bins_arr = a.bins_arr;
	delta = a.delta;
	var = a.var;

	return *this;
}

bool AxisCfg::operator==(const AxisCfg & ctx)
{
	return (this->bins == ctx.bins);
}

bool AxisCfg::operator!=(const AxisCfg & ctx)
{
	return (this->bins != ctx.bins);
}

TString AxisCfg::format_hist_string(const char * title,
                           const char * ylabel) const
{
    return TString::Format("%s;%s;%s",
                           title,
                           format_string().Data(),
                           ylabel);
}

TString AxisCfg::format_unit() const
{
	return format_unit(unit);
}

TString AxisCfg::format_string() const
{
  return TString::Format("%s%s", label.Data(), format_unit().Data());
}


TString AxisCfg::format_unit(const char * unit)
{
  return format_unit(TString(unit));
}

TString AxisCfg::format_unit(const TString & unit)
{
	TString funit;
	if (unit.Length())
		funit = " [" + unit + "]";

	return funit;
}

void AxisCfg::print() const
{
  if (!bins_arr)
    printf(" Axis: %d in [ %f; %f ] -- %s\n",
           bins, min, max, format_string().Data());
  else
  {
    TString buff;
    for (uint i = 0; i < bins; ++i)
    {
      buff += "| ";
      buff += bins_arr[i];
      buff += " ";
    }
    buff += "|";
    printf(" Axis: %d in %s -- %s\n",
           bins, buff.Data(), format_string().Data());
  }
}

DistributionContext::DistributionContext()
  : TNamed()
  , dim(DIM0)
  , var_weight(nullptr)
  , json_found(false)
{
}

DistributionContext::DistributionContext(Dimensions dim)
  : TNamed()
  , dim(dim)
  , var_weight(nullptr)
  , json_found(false)
{
}

DistributionContext::DistributionContext(const DistributionContext & ctx)
  : TNamed()
{
	*this = ctx;
	name = ctx.name;
}

bool DistributionContext::update()
{
	x.delta = ((x.max - x.min)/x.bins);
	y.delta = ((y.max - y.min)/y.bins);
  z.delta = ((z.max - z.min)/z.bins);

  if (0 == hist_name.Length())
    hist_name = name;

  if (0 == dir_name.Length())
    dir_name = hist_name;

  if (name.EndsWith("Ctx"))
    SetName(name);
  else
    SetName(name+"Ctx");

  return true;
}

int DistributionContext::validate() const
{
  if (0 == name.Length()) return 1;
  if (DIM1 <= dim && x.bins and !x.var) return 11;
  if (DIM2 <= dim && y.bins and !y.var) return 12;
  if (DIM3 == dim && z.bins and !z.var) return 13;

  return 0;
// 	return update();
}

DistributionContext::~DistributionContext()
{}

bool jsonReadTStringKey(const Json::Value & jsondata, const char * key, TString & target)
{
	if (jsondata.isMember(key))
	{
		target = jsondata[key].asCString();
		std::cout << "    + " << key << ": " << target.Data() << std::endl;
		return true;
	}
	return false;
}

bool jsonReadIntKey(const Json::Value & jsondata, const char * key, int & target)
{
	if (jsondata.isMember(key))
	{
		target = jsondata[key].asInt();
		std::cout << "    + " << key << ": " << target << std::endl;
		return true;
	}
	return false;
}

bool jsonReadUIntKey(const Json::Value & jsondata, const char * key, uint & target)
{
	if (jsondata.isMember(key))
	{
		target = jsondata[key].asInt();
		std::cout << "    + " << key << ": " << target << std::endl;
		return true;
	}
	return false;
}

bool jsonReadFloatKey(const Json::Value & jsondata, const char * key, float & target)
{
	if (jsondata.isMember(key))
	{
		target = jsondata[key].asFloat();
		std::cout << "    + " << key << ": " << target << std::endl;
		return true;
	}
	return false;
}

bool jsonReadDoubleKey(const Json::Value & jsondata, const char * key, double & target)
{
	if (jsondata.isMember(key))
	{
		target = jsondata[key].asDouble();
		std::cout << "    + " << key << ": " << target << std::endl;
		return true;
	}
	return false;
}

bool DistributionContext::configureFromJson(const char * name)
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
	const char * axis_labels[axis_num] = { "x", "y", "z" };
	AxisCfg * axis_ptrs[axis_num] = { &x, &y, &z };

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

bool DistributionContext::configureToJson(const char * name, const char * jsonfile)
{
	(void)jsonfile;

	Json::Value ana, cfg, axis;

	cfg["title"]	= "d^{2}N/dp_{t}dy.{cm}";

	axis["bins"]	= 0;
	axis["min"]		= 0;
	axis["max"]		= 0;
	axis["label"]	= "";
	axis["var"]		= "";

	cfg["x"] = axis;
	cfg["y"] = axis;
	cfg["z"] = axis;
	cfg["V"] = axis;

	ana[name] = cfg;

	std::cout << ana;

// 	Json::StyledWriter sw;
// 	std::cout << sw.write(ana);
	
// 	Json::FastWriter fw;
// 	std::cout << fw.write(ana);

	return true;
}

bool DistributionContext::findJsonFile(const char * initial_path, const char * filename, int search_depth)
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

DistributionContext & DistributionContext::operator=(const DistributionContext & ctx)
{
  dim = ctx.dim;
// 	ctx_name = ctx.ctx_name;
	hist_name = ctx.hist_name;
  dir_name = ctx.dir_name;

  x = ctx.x;
	y = ctx.y;
  z = ctx.z;

	var_weight = ctx.var_weight;

	diff_var_name = ctx.diff_var_name;

	return *this;
}

bool DistributionContext::operator==(const DistributionContext & ctx)
{
  if (this == &ctx)
    return true;

  if (this->dim != ctx.dim)
	{
		fprintf(stderr, "Not the same dimensions: %d vs %d\n", this->dim, ctx.dim);
		return false;
	}

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

bool DistributionContext::operator!=(const DistributionContext & ctx)
{
	return !operator==(ctx);
}

TString DistributionContext::format_hist_axes(const char * title) const {
  if (DIM3 == dim)
    return TString::Format("%s;%s%s%s;%s%s%s",
                           title,
                           x.label.Data(), x.format_unit().Data(),
                           y.label.Data(), y.format_unit().Data(),
                           z.label.Data(), z.format_unit().Data());
  if (DIM2 == dim)
    return TString::Format("%s;%s%s;%s%s",
                           title,
                           x.label.Data(), x.format_unit().Data(),
                           y.label.Data(), y.format_unit().Data());
  if (DIM1 == dim)
    return TString::Format("%s;%s%s;Counts [aux]",
                           title,
                           x.label.Data(), x.format_unit().Data());

  return TString::Format("%s;;", title);
}

void DistributionContext::print() const
{
  printf("Context: %s   Hist name: %s\n", name.Data(), hist_name.Data());
  printf(" Var name: %s\n", diff_var_name.Data());
  x.print();
  y.print();
  z.print();
}

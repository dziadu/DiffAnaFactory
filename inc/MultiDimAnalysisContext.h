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


#ifndef MULTIDIMANALYSISCONTEXT_H
#define MULTIDIMANALYSISCONTEXT_H

#include "MultiDimDistributionContext.h"

class MultiDimAnalysisContext : public MultiDimDistributionContext
{
public:
	AxisCfg V;     // x, y are two dimensions, V is a final Variable axis

	MultiDimAnalysisContext();
	MultiDimAnalysisContext(const MultiDimAnalysisContext & ctx);
	virtual ~MultiDimAnalysisContext();

	MultiDimAnalysisContext & operator=(const MultiDimAnalysisContext & ctx);
	bool operator==(const MultiDimAnalysisContext & ctx);
	bool operator!=(const MultiDimAnalysisContext & ctx);

	// flags
// 	virtual bool useCuts() const { return (cutMin or cutMax); }

	virtual void format_V_axis();

	virtual bool configureFromJson(const char * name);
	virtual bool configureToJson(const char * name, const char * jsonfile);

private:
	TString json_fn;
	ClassDef(MultiDimAnalysisContext, 2);
};

#endif // MULTIDIMANALYSISCONTEXT_H

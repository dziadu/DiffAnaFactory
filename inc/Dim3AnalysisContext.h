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


#ifndef DIM3ANALYSISCONTEXT_H
#define DIM3ANALYSISCONTEXT_H

#include "TNamed.h"
#include "TString.h"
#include "Rtypes.h"

#include "Dim2AnalysisContext.h"

class Dim3AnalysisContext : public Dim2AnalysisContext
{
public:
	AxisCfg z;

	Dim3AnalysisContext();
	Dim3AnalysisContext(const Dim3AnalysisContext & ctx);
	virtual ~Dim3AnalysisContext();

	Dim3AnalysisContext & operator=(const Dim3AnalysisContext & ctx);
	bool operator==(const Dim3AnalysisContext & ctx);
	bool operator!=(const Dim3AnalysisContext & ctx);

	void update() const;
	bool validate() const;

	// flags
// 	inline bool useCuts() const { return (cutMin or cutMax); }
// 	inline bool useClip() const { return cx.bins; }
// 	inline bool useTriple() const { return (z.bins and useClip()); }

	void format_z_axis();

	inline virtual const char * GetName() const { return ctxName.Data(); }
  inline virtual void SetName(const char* name) { ctxName = name; }
	const char * AnaName() const { return histPrefix.Data(); }

	bool findJsonFile(const char * initial_path, const char * filename, int search_depth = -1);
	bool configureFromJson(const char * name);
	bool configureToJson(const char * name, const char * jsonfile);

private:

	TString json_fn;
	bool json_found;
	ClassDef(Dim3AnalysisContext, 1);
};

#endif // DIM3ANALYSISCONTEXT_H

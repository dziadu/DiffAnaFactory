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

#ifndef DIFFERENTIALCONTEXT_H
#define DIFFERENTIALCONTEXT_H

#include "DistributionContext.h"

class DifferentialContext : public DistributionContext
{
public:
	AxisCfg V;     // x, y are two dimensions, V is a final Variable axis

	DifferentialContext();
	DifferentialContext(const DifferentialContext & ctx);
	virtual ~DifferentialContext();

	DifferentialContext & operator=(const DifferentialContext & ctx);
	bool operator==(const DifferentialContext & ctx);
	bool operator!=(const DifferentialContext & ctx);

	// flags
// 	virtual bool useCuts() const { return (cutMin or cutMax); }

	virtual bool configureFromJson(const char * name);
	virtual bool configureToJson(const char * name, const char * jsonfile);

  void print() const;
private:
	TString json_fn;
	ClassDef(DifferentialContext, 2);
};

#endif // DIFFERENTIALCONTEXT_H

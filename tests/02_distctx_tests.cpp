#include <cppunit/extensions/HelperMacros.h>

#include "MultiDimDistributionContext.h"

class DistCtxCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( DistCtxCase );
	CPPUNIT_TEST( MyTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() override;

protected:
	void MyTest();

  int bins;
  float min, max;
  AxisCfg axis;
	MultiDimDistributionContext d3ctx;
};

CPPUNIT_TEST_SUITE_REGISTRATION( DistCtxCase );

void DistCtxCase::setUp()
{
  bins = 10;
  min = 0;
  max = 10;

  d3ctx.histPrefix = "test";
  d3ctx.x.label = "d2_x"; d3ctx.x.unit = "mm";
  d3ctx.y.label = "d2_y";
  d3ctx.z.label = "d2_z"; d3ctx.z.unit = "cm";
}

void DistCtxCase::MyTest()
{
  std::string output_string;

  output_string = d3ctx.z.format_unit();
	CPPUNIT_ASSERT_EQUAL(std::string(" [cm]"), output_string);

  MultiDimDistributionContext d3ctx_2 = d3ctx;

  output_string = d3ctx_2.z.format_unit();
	CPPUNIT_ASSERT_EQUAL(std::string(" [cm]"), output_string);

  CPPUNIT_ASSERT_EQUAL(true, d3ctx == d3ctx_2);

  d3ctx_2.x.bins = 13;
  CPPUNIT_ASSERT_EQUAL(false, d3ctx == d3ctx_2);

  CPPUNIT_ASSERT_EQUAL(true, d3ctx == d3ctx);
}

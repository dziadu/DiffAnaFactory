#include <cppunit/extensions/HelperMacros.h>

#include "MultiDimAnalysisContext.h"

class AnaCtxCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( AnaCtxCase );
	CPPUNIT_TEST( MyTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();

protected:
	void MyTest();

  int bins;
  float min, max;
  AxisCfg axis;
  MultiDimAnalysisContext a3ctx;
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnaCtxCase );

void AnaCtxCase::setUp()
{
  bins = 10;
  min = 0;
  max = 10;

  a3ctx.V.label = "a2_V";
}

void AnaCtxCase::MyTest()
{
  std::string output_string;

  a3ctx.x.bins = 10; a3ctx.x.label = "a2_x"; a3ctx.x.unit = "mm";
  a3ctx.y.bins = 10; a3ctx.y.label = "a2_y";
  a3ctx.format_V_axis();

  output_string = a3ctx.x.format_unit();
	CPPUNIT_ASSERT_EQUAL(std::string(" [mm]"), output_string);
  output_string = a3ctx.V.label;
	CPPUNIT_ASSERT_EQUAL(std::string("d^{2}/da2_xda2_y"), output_string);

  a3ctx.z.bins = 10; a3ctx.z.label = "a2_z";
  a3ctx.format_V_axis();

  output_string = a3ctx.V.label;
  CPPUNIT_ASSERT_EQUAL(std::string("d^{3}/da2_xda2_yda2_z"), output_string);

  MultiDimAnalysisContext a3ctx_2 = a3ctx;
  a3ctx_2.format_V_axis();

  output_string = a3ctx_2.V.label;
  CPPUNIT_ASSERT_EQUAL(std::string("d^{3}/da2_xda2_yda2_z"), output_string);

  CPPUNIT_ASSERT_EQUAL(true, a3ctx == a3ctx_2);

  a3ctx_2.x.bins = 13;
  CPPUNIT_ASSERT_EQUAL(false, a3ctx == a3ctx_2);

  CPPUNIT_ASSERT_EQUAL(true, a3ctx == a3ctx);
}

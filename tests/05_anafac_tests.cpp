#include <cppunit/extensions/HelperMacros.h>

#include "Dim3AnalysisFactory.h"

class AnaFacCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( AnaFacCase );
	CPPUNIT_TEST( MyTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();

protected:
	void MyTest();

  int bins;
  float min, max;
  MultiDimAnalysisContext a3ctx;
  Dim3AnalysisFactory * a3fac;
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnaFacCase );

void AnaFacCase::setUp()
{
  bins = 10;
  min = 0;
  max = 10;

  a3ctx.x.bins = 10; a3ctx.x.min = -10; a3ctx.x.max = 10; a3ctx.x.label = "a3_x"; a3ctx.x.unit = "cm";
  a3ctx.y.bins = 10; a3ctx.y.min = -10; a3ctx.y.max = 10; a3ctx.y.label = "a3_y"; a3ctx.y.unit = "mm";
  a3ctx.z.bins = 10; a3ctx.z.min = -10; a3ctx.z.max = 10; a3ctx.z.label = "a3_z"; a3ctx.z.unit = "um";

  a3ctx.V.bins = 10; a3ctx.V.min = -10; a3ctx.V.max = 10; a3ctx.V.label = "a3_V";

  a3fac = new Dim3AnalysisFactory(a3ctx);
  a3fac->Init();
}

void AnaFacCase::MyTest()
{
  TH1 * h = (*a3fac->diffs)[0];
  h->FillRandom("gaus");

  a3ctx.format_V_axis();

  int n1 = h->GetBinContent(5);

  a3fac->scale(3);
  int n2 = h->GetBinContent(5);
	CPPUNIT_ASSERT_EQUAL(n1*3, n2);

  std::string output_string;
  output_string = a3ctx.V.label;
  CPPUNIT_ASSERT_EQUAL(std::string("d^{3}/da3_xda3_yda3_z"), output_string);
}

#include <cppunit/extensions/HelperMacros.h>

#include <ExtraDimensionMapper.h>

#include <TH3D.h>

class BasicCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( BasicCase );
	CPPUNIT_TEST( MyTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();

protected:
	void MyTest();

  int bins;
  float min, max;
  AxisCfg axis;
	ExtraDimensionMapper * edm1, * edm2, *edm3;
  TH1D * h1;
  TH2D * h2;
  TH3D * h3;
};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicCase );

void BasicCase::setUp()
{
  bins = 10;
  min = 0;
  max = 10;
  h1 = new TH1D("h1", "h1", bins, min, max);
  h2 = new TH2D("h2", "h2", bins, min, max, bins, min, max);
  h3 = new TH3D("h3", "h3", bins, min, max, bins, min, max, bins, min, max);

  axis.bins = 20;
  axis.min = -3;
  axis.max = 3;

  edm1 = new ExtraDimensionMapper(h1, axis, "");
  edm2 = new ExtraDimensionMapper(h2, axis, "");
  edm3 = new ExtraDimensionMapper(h3, axis, "");
}

void BasicCase::MyTest()
{
// 	std::string pattern_string("%%d pattern");
// 	std::string test_string("test pattern");
// 	std::string replace_string("test");

// 	std::string output_string = SmartFactory::placeholder(pattern_string, "%%d", replace_string);
// 	CPPUNIT_ASSERT_EQUAL(output_string, test_string);

  for (int i = 0; i < bins; ++i)
  {
    CPPUNIT_ASSERT(edm1->get(i));
    edm1->get(i)->FillRandom("gaus", 10000);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, edm1->get(i)->GetMean(), 0.05 );
    for (int j = 0; j < bins; ++j)
    {
      CPPUNIT_ASSERT(edm2->get(i, j));
      edm2->get(i)->FillRandom("gaus", 10000);
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, edm2->get(i)->GetMean(), 0.05 );
      for (int k = 0; k < bins; ++k)
      {
        CPPUNIT_ASSERT(edm3->get(i, j, k));
        edm3->get(i)->FillRandom("gaus", 10000);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, edm3->get(i)->GetMean(), 0.05 );
      }
    }
  }
}

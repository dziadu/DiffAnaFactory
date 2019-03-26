#include <cppunit/extensions/HelperMacros.h>

#include <ExtraDimensionMapper.h>

#include <TH3D.h>

class BasicCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( BasicCase );
	CPPUNIT_TEST( IndexTest );
  CPPUNIT_TEST( FillTest );
  CPPUNIT_TEST( ReverseTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() override;
  void tearDown() override;

protected:
	void IndexTest();
  void FillTest();
  void ReverseTest();

  uint bins;
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
  bins = 2;
  min = 0;
  max = 2;
  h1 = new TH1D("h1", "h1", bins, min, max);
  h2 = new TH2D("h2", "h2", bins, min, max, bins, min, max);
  h3 = new TH3D("h3", "h3", bins, min, max, bins, min, max, bins, min, max);

  axis.bins = 20;
  axis.min = -3;
  axis.max = 3;

  edm1 = new ExtraDimensionMapper(DIM1, "edm1", h1, axis, "edm1");
  edm2 = new ExtraDimensionMapper(DIM2, "edm2", h2, axis, "edm2");
  edm3 = new ExtraDimensionMapper(DIM3, "edm3", h3, axis, "edm3");
}

void BasicCase::tearDown()
{
  if (h1) { delete h1; h1 = nullptr; }
  if (h2) { delete h2; h2 = nullptr; }
  if (h3) { delete h3; h3 = nullptr; }

  if (edm1) { delete edm1; edm1 = nullptr; }
  if (edm2) { delete edm2; edm2 = nullptr; }
  if (edm3) { delete edm3; edm3 = nullptr; }
}

void BasicCase::IndexTest()
{
    Int_t found_bin = h2->FindBin(0.5, 0.5);
    CPPUNIT_ASSERT_EQUAL(5, found_bin);

    Int_t bx, by, bz;
    h2->GetBinXYZ(found_bin, bx, by, bz);
    CPPUNIT_ASSERT_EQUAL(1, bx);
    CPPUNIT_ASSERT_EQUAL(1, by);
    CPPUNIT_ASSERT_EQUAL(0, bz);

    Int_t mapped_bin = edm2->getBin(bx-1, by-1, 0);
    CPPUNIT_ASSERT_EQUAL(0, mapped_bin);
}

void BasicCase::FillTest()
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
      edm2->get(i, j)->FillRandom("gaus", 10000);
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, edm2->get(j)->GetMean(), 0.05 );
      for (int k = 0; k < bins; ++k)
      {
        CPPUNIT_ASSERT(edm3->get(i, j, k));
        edm3->get(i, j, k)->FillRandom("gaus", 10000);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, edm3->get(k)->GetMean(), 0.05 );
      }
    }
  }
}

void BasicCase::ReverseTest()
{
  UInt_t bin, x, y, z;
  bool rc;

  rc = edm1->reverseBin(-1, x);
  CPPUNIT_ASSERT(!rc);
  rc = edm1->reverseBin(0, x);
  CPPUNIT_ASSERT(rc);
  rc = edm1->reverseBin(1, x);
  CPPUNIT_ASSERT(rc);
  rc = edm1->reverseBin(2, x);
  CPPUNIT_ASSERT(!rc);

  for (uint i = 0; i < bins; ++i)
  {
    bin = edm1->getBin(i);
    rc = edm1->reverseBin(bin, x);
    CPPUNIT_ASSERT(rc);
    CPPUNIT_ASSERT_EQUAL(i, x);

    for (uint j = 0; j < bins; ++j)
    {
      bin = edm2->getBin(i, j);
      rc = edm2->reverseBin(bin, x, y);
      CPPUNIT_ASSERT(rc);
      CPPUNIT_ASSERT_EQUAL(i, x);
      CPPUNIT_ASSERT_EQUAL(j, y);

      for (uint k = 0; k < bins; ++k)
      {
        bin = edm3->getBin(i, j, k);
        rc = edm3->reverseBin(bin, x, y, z);
        CPPUNIT_ASSERT(rc);
        CPPUNIT_ASSERT_EQUAL(i, x);
        CPPUNIT_ASSERT_EQUAL(j, y);
        CPPUNIT_ASSERT_EQUAL(k, z);
      }
    }
  }
}

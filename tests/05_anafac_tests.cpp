#include <cppunit/extensions/HelperMacros.h>

#include "ExtraDimensionMapper.h"
#include "Dim2AnalysisFactory.h"
#include "Dim3AnalysisFactory.h"

#include <TFile.h>

class AnaFacCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( AnaFacCase );
	CPPUNIT_TEST( InitializationTest );
  CPPUNIT_TEST( ScaleTest );
  CPPUNIT_TEST( FillTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() override;
  void tearDown() override;

protected:
	void InitializationTest();
  void ScaleTest();
  void FillTest();

  int bins;
  float min, max;
  MultiDimAnalysisContext a3ctx;
  Dim2AnalysisFactory * a2fac;
  Dim3AnalysisFactory * a3fac;
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnaFacCase );

void AnaFacCase::setUp()
{
  a3ctx.x.bins = 4; a3ctx.x.min = -10; a3ctx.x.max = 10; a3ctx.x.label = "a3_x"; a3ctx.x.unit = "cm";
  a3ctx.y.bins = 3; a3ctx.y.min = -10; a3ctx.y.max = 10; a3ctx.y.label = "a3_y"; a3ctx.y.unit = "mm";
  a3ctx.z.bins = 2; a3ctx.z.min = -10; a3ctx.z.max = 10; a3ctx.z.label = "a3_z"; a3ctx.z.unit = "um";

  a3ctx.V.bins = 100; a3ctx.V.min = -10; a3ctx.V.max = 10; a3ctx.V.label = "a3_V";

  a3ctx.histPrefix = "a2fac";
  a2fac = new Dim2AnalysisFactory(a3ctx);
  a2fac->init();

  a3ctx.histPrefix = "a3fac";
  a3fac = new Dim3AnalysisFactory(a3ctx);
  a3fac->init();
}

void AnaFacCase::tearDown()
{
  if (a2fac) delete a2fac;
  if (a3fac) delete a3fac;
}

void AnaFacCase::InitializationTest()
{
  ExtraDimensionMapper * edm;

  edm = a2fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a3ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a3ctx.y.bins, edm->getBinsY());
//   CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());

  edm = a3fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a3ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a3ctx.y.bins, edm->getBinsY());
  CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());
}

void AnaFacCase::ScaleTest()
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
  a3fac->finalize();

//   TFile * file = TFile::Open("/tmp/res.root", "RECREATE");
//
//   a3fac->MultiDimAnalysisExtension::ctx.Write();
//   a3fac->write(file, true);
//   file->Close();
}

void AnaFacCase::FillTest()
{
  ExtraDimensionMapper * edm;

  edm = a2fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a3ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a3ctx.y.bins, edm->getBinsY());
//   CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());

  edm = a3fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a3ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a3ctx.y.bins, edm->getBinsY());
  CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());
}

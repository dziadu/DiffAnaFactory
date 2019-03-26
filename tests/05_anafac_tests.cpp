#include <cppunit/extensions/HelperMacros.h>

#include "ExtraDimensionMapper.h"
#include "DifferentialFactory.h"
#include "DifferentialFactory.h"

#include <TFile.h>

class AnaFacCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( AnaFacCase );
	CPPUNIT_TEST( InitializationTest );
  CPPUNIT_TEST( ScaleTest );
  CPPUNIT_TEST( FillTest );
  CPPUNIT_TEST( WriteTest );
  CPPUNIT_TEST( ReadTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() override;
  void tearDown() override;

protected:
	void InitializationTest();
  void ScaleTest();
  void FillTest();
  void WriteTest();
  void ReadTest();

  int bins;
  float min, max;
  DifferentialContext a2ctx;
  DifferentialContext a3ctx;
  DifferentialFactory * a2fac;
  DifferentialFactory * a3fac;
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnaFacCase );

void AnaFacCase::setUp()
{
  a3ctx.dim = DIM3;
  a3ctx.x.bins = 4; a3ctx.x.min = -10; a3ctx.x.max = 10; a3ctx.x.label = "a3_x"; a3ctx.x.unit = "cm";
  a3ctx.y.bins = 3; a3ctx.y.min = -10; a3ctx.y.max = 10; a3ctx.y.label = "a3_y"; a3ctx.y.unit = "mm";
  a3ctx.z.bins = 2; a3ctx.z.min = -10; a3ctx.z.max = 10; a3ctx.z.label = "a3_z"; a3ctx.z.unit = "um";

  a3ctx.V.bins = 100; a3ctx.V.min = -10; a3ctx.V.max = 10; a3ctx.V.label = "a3_V";

  a2ctx = a3ctx;

  a2ctx.dim = DIM2;
  a2ctx.name = "a2fac";
  a2ctx.hist_name = "a2fac";

  a3ctx.name = "a3fac";
  a3ctx.hist_name = "a3fac";
}

void AnaFacCase::tearDown()
{
  if (a2fac) delete a2fac; a2fac = nullptr;
  if (a3fac) delete a3fac; a3fac = nullptr;
}

void AnaFacCase::InitializationTest()
{
  ExtraDimensionMapper * edm;

  a2fac = new DifferentialFactory(a2ctx);
  a2fac->init();

  a3fac = new DifferentialFactory(a3ctx);
  a3fac->init();

  edm = a2fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a2ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a2ctx.y.bins, edm->getBinsY());
//   CPPUNIT_ASSERT_EQUAL(a2ctx.z.bins, edm->getBinsZ());

  edm = a3fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a3ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a3ctx.y.bins, edm->getBinsY());
  CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());
}

void AnaFacCase::ScaleTest()
{
  a3fac = new DifferentialFactory(a3ctx);
  a3fac->init();

  TH1 * h = (*a3fac->diffs)[0];
  h->FillRandom("gaus");

  a3ctx.format_diff_axis();

  int n1 = h->GetBinContent(5);

  a3fac->scale(3);
  int n2 = h->GetBinContent(5);
	CPPUNIT_ASSERT_EQUAL(n1*3, n2);

  std::string output_string;
  output_string = a3ctx.label;
  CPPUNIT_ASSERT_EQUAL(std::string("d^{3}/da3_xda3_yda3_z"), output_string);
  a3fac->finalize();
}

void AnaFacCase::FillTest()
{
  a2fac = new DifferentialFactory(a2ctx);
  a2fac->init();

  a3fac = new DifferentialFactory(a3ctx);
  a3fac->init();

  ExtraDimensionMapper * edm;

  edm = a2fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a2ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a2ctx.y.bins, edm->getBinsY());
//   CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());

  edm = a3fac->diffs;

  CPPUNIT_ASSERT_EQUAL(a3ctx.x.bins, edm->getBinsX());
  CPPUNIT_ASSERT_EQUAL(a3ctx.y.bins, edm->getBinsY());
  CPPUNIT_ASSERT_EQUAL(a3ctx.z.bins, edm->getBinsZ());
}

void AnaFacCase::WriteTest()
{
  a2fac = new DifferentialFactory(a2ctx);
  a2fac->init();
  CPPUNIT_ASSERT_EQUAL(true, 0 != a2fac->ctx.validate());
  CPPUNIT_ASSERT_EQUAL(true, a2fac->ctx.update());
  (*a2fac->diffs)[0]->FillRandom("gaus", 1000);

  a3fac = new DifferentialFactory(a3ctx);
  a3fac->init();
  CPPUNIT_ASSERT_EQUAL(true, 0 != a3fac->ctx.validate());
  CPPUNIT_ASSERT_EQUAL(true, a3fac->ctx.update());
  (*a3fac->diffs)[0]->FillRandom("gaus", 1000);

  TFile * file = TFile::Open("/tmp/res.root", "RECREATE");

  a2fac->ctx.Write();
  a2fac->write(file, true);

  a3fac->ctx.Write();
  a3fac->write(file, true);

  file->Close();
}

void AnaFacCase::ReadTest()
{
  TFile * file = TFile::Open("/tmp/res.root", "OPEN");
  if (!file or !file->IsOpen())
  {
    std::cerr << "File " << file << " can't be open!";
    return;
  }

  DifferentialContext * dactx = nullptr;

  std::string tmpname = "a3fac";
  tmpname.append("Ctx");
  file->ls();
  dactx = (DifferentialContext *)file->Get(tmpname.c_str());

  CPPUNIT_ASSERT_EQUAL(true, dactx != nullptr);
  dactx->print();

  DifferentialFactory * fac = nullptr;
  fac = new DifferentialFactory(dactx);
  fac->setSource(file);
  fac->init();

//   fac->listRegisteredObjects();
//   fac->diffs->listRegisteredObjects();

  CPPUNIT_ASSERT_EQUAL(1000, (int)(*fac->diffs)[0]->GetEntries());

//   a3fac->MultiDimAnalysisExtension::ctx.Write();
//   a3fac->write(file, true);
  file->Close();
}

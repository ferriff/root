#include "ntuple_test.hxx"

#include <ROOT/RNTupleProcessor.hxx>

using ROOT::Experimental::RNTupleModel;
using ROOT::Experimental::RNTupleWriter;
using ROOT::Experimental::Internal::RNTupleProcessor;
using ROOT::Experimental::Internal::RNTupleSourceSpec;

TEST(RNTupleProcessor, Basic)
{
   FileRaii fileGuard("test_ntuple_processor_basic.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard.GetPath());

      for (unsigned i = 0; i < 10; ++i) {
         *fldX = static_cast<float>(i);
         ntuple->Fill();
      }
   }

   std::vector<RNTupleSourceSpec> ntuples;
   try {
      RNTupleProcessor proc(ntuples);
      FAIL() << "creating a processor without at least one RNTuple should throw";
   } catch (const RException &err) {
      EXPECT_THAT(err.what(), testing::HasSubstr("at least one RNTuple must be provided"));
   }

   ntuples = {{"ntuple", fileGuard.GetPath()}};

   int nEntries = 0;
   for (const auto &entry : RNTupleProcessor(ntuples)) {
      auto x = entry->GetPtr<float>("x");
      EXPECT_EQ(static_cast<float>(entry.GetGlobalEntryIndex()), *x);
      ++nEntries;
   }
   EXPECT_EQ(nEntries, 10);
}

TEST(RNTupleProcessor, SimpleChain)
{
   FileRaii fileGuard1("test_ntuple_processor_simple_chain1.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto fldY = model->MakeField<std::vector<float>>("y");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard1.GetPath());

      for (unsigned i = 0; i < 5; ++i) {
         *fldX = static_cast<float>(i);
         *fldY = {static_cast<float>(i), static_cast<float>(i * 2)};
         ntuple->Fill();
      }
   }
   FileRaii fileGuard2("test_ntuple_processor_simple_chain2.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto fldY = model->MakeField<std::vector<float>>("y");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard2.GetPath());

      for (unsigned i = 5; i < 8; ++i) {
         *fldX = static_cast<float>(i);
         *fldY = {static_cast<float>(i), static_cast<float>(i * 2)};
         ntuple->Fill();
      }
   }

   std::vector<RNTupleSourceSpec> ntuples = {{"ntuple", fileGuard1.GetPath()}, {"ntuple", fileGuard2.GetPath()}};

   std::uint64_t nEntries = 0;
   for (const auto &entry : RNTupleProcessor(ntuples)) {
      auto x = entry->GetPtr<float>("x");
      EXPECT_EQ(static_cast<float>(entry.GetGlobalEntryIndex()), *x);

      auto y = entry->GetPtr<std::vector<float>>("y");
      std::vector<float> yExp = {static_cast<float>(entry.GetGlobalEntryIndex()), static_cast<float>(nEntries * 2)};
      EXPECT_EQ(yExp, *y);

      if (entry.GetLocalEntryIndex() == 0) {
         EXPECT_THAT(entry.GetGlobalEntryIndex(), testing::AnyOf(0, 5));
      }

      ++nEntries;
   }
   EXPECT_EQ(nEntries, 8);
}

TEST(RNTupleProcessor, EmptyNTuples)
{
   FileRaii fileGuard1("test_ntuple_processor_empty_ntuples1.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard1.GetPath());
   }
   FileRaii fileGuard2("test_ntuple_processor_empty_ntuples2.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard2.GetPath());

      for (unsigned i = 0; i < 2; ++i) {
         *fldX = static_cast<float>(i);
         ntuple->Fill();
      }
   }
   FileRaii fileGuard3("test_ntuple_processor_empty_ntuples3.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard3.GetPath());
   }
   FileRaii fileGuard4("test_ntuple_processor_empty_ntuples4.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard4.GetPath());

      for (unsigned i = 2; i < 5; ++i) {
         *fldX = static_cast<float>(i);
         ntuple->Fill();
      }
   }
   FileRaii fileGuard5("test_ntuple_processor_empty_ntuples5.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard5.GetPath());
   }

   std::vector<RNTupleSourceSpec> ntuples = {{"ntuple", fileGuard1.GetPath()},
                                             {"ntuple", fileGuard2.GetPath()},
                                             {"ntuple", fileGuard3.GetPath()},
                                             {"ntuple", fileGuard4.GetPath()},
                                             {"ntuple", fileGuard5.GetPath()}};

   std::uint64_t nEntries = 0;

   try {
      RNTupleProcessor proc(ntuples);
      FAIL() << "creating a processor where the first RNTuple does not contain any entries should throw";
   } catch (const RException &err) {
      EXPECT_THAT(err.what(), testing::HasSubstr("first RNTuple does not contain any entries"));
   }

   ntuples = {{"ntuple", fileGuard2.GetPath()},
              {"ntuple", fileGuard3.GetPath()},
              {"ntuple", fileGuard4.GetPath()},
              {"ntuple", fileGuard5.GetPath()}};

   for (const auto &entry : RNTupleProcessor(ntuples)) {
      auto x = entry->GetPtr<float>("x");
      EXPECT_EQ(static_cast<float>(nEntries), *x);
      ++nEntries;
   }
   EXPECT_EQ(nEntries, 5);
}

TEST(RNTupleProcessor, ChainUnalignedModels)
{
   FileRaii fileGuard1("test_ntuple_processor_simple_chain1.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x", 0.);
      auto fldY = model->MakeField<char>("y", 'a');
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard1.GetPath());
      ntuple->Fill();
   }
   FileRaii fileGuard2("test_ntuple_processor_simple_chain2.root");
   {
      auto model = RNTupleModel::Create();
      auto fldX = model->MakeField<float>("x", 1.);
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "ntuple", fileGuard2.GetPath());
      ntuple->Fill();
   }

   std::vector<RNTupleSourceSpec> ntuples = {{"ntuple", fileGuard1.GetPath()}, {"ntuple", fileGuard2.GetPath()}};

   auto proc = RNTupleProcessor(ntuples);
   auto entry = proc.begin();
   auto x = (*entry)->GetPtr<float>("x");
   auto y = (*entry)->GetPtr<char>("y");
   EXPECT_EQ(0., *x);
   EXPECT_EQ('a', *y);

   try {
      entry++;
      FAIL() << "trying to connect a new page source containing additional (unknown) fields is not supported";
   } catch (const RException &err) {
      EXPECT_THAT(err.what(), testing::HasSubstr("field \"y\" not found in current RNTuple"));
   }
}

/// \file ROOT/RNTupleRange.hxx
/// \ingroup NTuple ROOT7
/// \author Jakob Blomer <jblomer@cern.ch>
/// \date 2018-10-05
/// \warning This is part of the ROOT 7 prototype! It will change without notice. It might trigger earthquakes. Feedback
/// is welcome!

/*************************************************************************
 * Copyright (C) 1995-2019, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT7_RNTupleRange
#define ROOT7_RNTupleRange

#include <ROOT/RNTupleUtil.hxx>

namespace ROOT {
namespace Experimental {

// clang-format off
/**
\class ROOT::Experimental::RNTupleGlobalRange
\ingroup NTuple
\brief Used to loop over indexes (entries or collections) between start and end
*/
// clang-format on
class RNTupleGlobalRange {
private:
   NTupleSize_t fStart;
   NTupleSize_t fEnd;

public:
   class RIterator {
   private:
      NTupleSize_t fIndex = kInvalidNTupleIndex;

   public:
      using iterator = RIterator;
      using iterator_category = std::forward_iterator_tag;
      using value_type = NTupleSize_t;
      using difference_type = NTupleSize_t;
      using pointer = NTupleSize_t *;
      using reference = NTupleSize_t &;

      RIterator() = default;
      explicit RIterator(NTupleSize_t index) : fIndex(index) {}
      ~RIterator() = default;

      iterator operator++(int) /* postfix */
      {
         auto r = *this;
         fIndex++;
         return r;
      }
      iterator &operator++() /* prefix */
      {
         ++fIndex;
         return *this;
      }
      reference operator*() { return fIndex; }
      pointer operator->() { return &fIndex; }
      bool operator==(const iterator &rh) const { return fIndex == rh.fIndex; }
      bool operator!=(const iterator &rh) const { return fIndex != rh.fIndex; }
   };

   RNTupleGlobalRange(NTupleSize_t start, NTupleSize_t end) : fStart(start), fEnd(end) {}
   RIterator begin() const { return RIterator(fStart); }
   RIterator end() const { return RIterator(fEnd); }
   NTupleSize_t size() const { return fEnd - fStart; }
   bool IsValid() const { return (fStart != kInvalidNTupleIndex) && (fEnd != kInvalidNTupleIndex); }
};

// clang-format off
/**
\class ROOT::Experimental::RNTupleClusterRange
\ingroup NTuple
\brief Used to loop over entries of collections in a single cluster
*/
// clang-format on
class RNTupleClusterRange {
private:
   const DescriptorId_t fClusterId;
   const NTupleSize_t fStart;
   const NTupleSize_t fEnd;

public:
   class RIterator {
   private:
      RNTupleLocalIndex fLocalIndex;

   public:
      using iterator = RIterator;
      using iterator_category = std::forward_iterator_tag;
      using value_type = RNTupleLocalIndex;
      using difference_type = RNTupleLocalIndex;
      using pointer = RNTupleLocalIndex *;
      using reference = RNTupleLocalIndex &;

      RIterator() = default;
      explicit RIterator(RNTupleLocalIndex localIndex) : fLocalIndex(localIndex) {}
      ~RIterator() = default;

      iterator operator++(int) /* postfix */
      {
         auto r = *this;
         fLocalIndex++;
         return r;
      }
      iterator &operator++() /* prefix */
      {
         fLocalIndex++;
         return *this;
      }
      reference operator*() { return fLocalIndex; }
      pointer operator->() { return &fLocalIndex; }
      bool operator==(const iterator &rh) const { return fLocalIndex == rh.fLocalIndex; }
      bool operator!=(const iterator &rh) const { return fLocalIndex != rh.fLocalIndex; }
   };

   RNTupleClusterRange(DescriptorId_t clusterId, NTupleSize_t start, NTupleSize_t end)
      : fClusterId(clusterId), fStart(start), fEnd(end)
   {
   }
   RIterator begin() const { return RIterator(RNTupleLocalIndex(fClusterId, fStart)); }
   RIterator end() const { return RIterator(RNTupleLocalIndex(fClusterId, fEnd)); }
   NTupleSize_t size() const { return fEnd - fStart; }
};

} // namespace Experimental
} // namespace ROOT

#endif

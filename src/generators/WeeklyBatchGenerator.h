/*
 * Project: ChocAn Data Center System (Straw Hats)
 *
 * Team + Ownership:
 * - Aldo Medina    (UC02 / Submit Service Record; Provider Terminal flow + record save)
 * - Lulu Chen      (UC01 / Validate Member Eligibility; Data Management support)
 * - Audity Moumi   (UC04 / Weekly Batch + Reports + EFT outputs)
 * - Rania Dayekh   (UC05 / Provider Directory generation)
 *
 * Created on:   2025-04-15
 * Last updated: 2025-04-24
 * Purpose: Generates all 5 weekly report files for a 7-day window (UC04).
 */

// Primary owner: Audity Moumi (UC04 / weekly batch + EFT)

#pragma once
#include <string>
#include <vector>
using namespace std;
#include "models/ServiceRecord.h"
#include "repositories/MemberRepository.h"
#include "repositories/ProviderRepository.h"
#include "repositories/ServiceRepository.h"

class WeeklyBatchGenerator {
public:
  WeeklyBatchGenerator(const MemberRepository&   memberRepo,
                       const ProviderRepository& providerRepo,
                       const ServiceRepository&  serviceRepo);

  // Run full batch for the 7-day window ending on weekEndingDate (MM-DD-YYYY).
  // Reads records from recordsPath, writes all outputs under outputDir.
  // Returns true on success (even if 0 records).
  bool run(const string& recordsPath,
           const string& weekEndingDate,
           const string& outputDir) const;

private:
  const MemberRepository&   memberRepo_;
  const ProviderRepository& providerRepo_;
  const ServiceRepository&  serviceRepo_;

  // helpers
  bool isInWindow_(const string& date, const tm& endTm) const;
  void writeProviderReports_(const vector<ServiceRecord>& recs,
                             const string& dir) const;
  void writeMemberReports_(const vector<ServiceRecord>& recs,
                           const string& dir) const;
  void writeWeeklySummary_(const vector<ServiceRecord>& recs,
                           const string& dir,
                           const string& weekEndingDate) const;
  void writeEFT_(const vector<ServiceRecord>& recs,
                 const string& dir) const;
  void writeAPSummary_(const vector<ServiceRecord>& recs,
                       const string& dir) const;
};

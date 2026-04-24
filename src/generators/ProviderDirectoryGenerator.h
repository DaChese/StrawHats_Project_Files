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
 * Purpose: Writes a sorted provider directory file to data/output/ (UC05).
 */

// Primary owner: Rania Dayekh (UC05 / provider directory)

#pragma once
#include <string>
using namespace std;
#include "repositories/ServiceRepository.h"

class ProviderDirectoryGenerator {
public:
  explicit ProviderDirectoryGenerator(const ServiceRepository& svcRepo);

  // Generates output/<outDir>/provider_directory_<YYYYMMDD>.txt
  // Returns the path written, or empty string on failure.
  string generate(const string& outputDir) const;

private:
  const ServiceRepository& svcRepo_;
};

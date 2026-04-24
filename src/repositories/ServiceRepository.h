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
 * Purpose: Read-only service code store backed by services.csv; sorted lookup for directory.
 */

// Primary owner: Rania Dayekh (UC05 / provider directory) + Aldo Medina (UC02 / code lookup)

#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
#include "models/ServiceCode.h"

class ServiceRepository {
public:
  bool loadFromCsv(const string& path);
  optional<ServiceCode> findByCode(const string& code) const;
  vector<ServiceCode>   allSortedByName() const;  // for directory generation

private:
  unordered_map<string, ServiceCode> codes_;
};

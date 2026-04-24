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
 * Purpose: Append-only service record store; locked CSV schema, used by batch reporting.
 */

// Primary owner: Aldo Medina (UC02 / record save) + Audity Moumi (UC04 / batch read)

#pragma once
#include <string>
#include <vector>
using namespace std;
#include "models/ServiceRecord.h"

class ServiceRecordRepository {
public:
  // Locked CSV schema — never change column order or names
  static constexpr const char* HEADER =
    "timestamp,dateOfService,providerNumber,memberNumber,serviceCode,fee,comments";

  explicit ServiceRecordRepository(const string& path);
  bool save(const ServiceRecord& record);
  vector<ServiceRecord> loadAll() const;  // for batch/reporting

private:
  string path_;
  bool ensureHeader_();
};

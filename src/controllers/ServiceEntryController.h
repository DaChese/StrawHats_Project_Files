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
 * Purpose: Validates date format, looks up service codes, and saves completed service records.
 */

// Primary owner: Aldo Medina (UC02 / service entry + record save)

#pragma once
#include <string>
using namespace std;
#include "models/ServiceRecord.h"
#include "repositories/ServiceRepository.h"
#include "repositories/ServiceRecordRepository.h"

struct ServiceEntryResult {
  bool success = false;
  string message;
  double fee = 0.0;
  string serviceName;
};

class ServiceEntryController {
public:
  ServiceEntryController(const ServiceRepository& svcRepo,
                         ServiceRecordRepository& recordRepo);

  // Validate date format MM-DD-YYYY
  bool validateDate(const string& date) const;

  // Look up service code, returns result with fee/name on success
  ServiceEntryResult lookupServiceCode(const string& code) const;

  // Save a completed service record, returns success/fail
  bool saveRecord(const ServiceRecord& record) const;

private:
  const ServiceRepository& svcRepo_;
  ServiceRecordRepository& recordRepo_;
};

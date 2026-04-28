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

#include "controllers/ServiceEntryController.h"
#include "util/StringUtil.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

ServiceEntryController::ServiceEntryController(const ServiceRepository& svcRepo,
                                               ServiceRecordRepository& recordRepo)
  : svcRepo_(svcRepo), recordRepo_(recordRepo) {}

bool ServiceEntryController::validateDate(const string& date) const {
  // Expect MM-DD-YYYY
  if (date.size() != 10) return false;
  if (date[2] != '-' || date[5] != '-') return false;

  string mm = date.substr(0, 2);
  string dd = date.substr(3, 2);
  string yyyy = date.substr(6, 4);

  if (!strutil::isDigits(mm) || !strutil::isDigits(dd) || !strutil::isDigits(yyyy))
    return false;

  int m = stoi(mm), d = stoi(dd), y = stoi(yyyy);
  if (m < 1 || m > 12) return false;
  if (y < 1900) return false;

  // Days per month (non-leap year baseline)
  static const int daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
  int maxDay = daysInMonth[m];
  // Leap year: divisible by 4, except centuries unless divisible by 400
  if (m == 2) {
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (leap) maxDay = 29;
  }
  if (d < 1 || d > maxDay) return false;
  return true;
}

ServiceEntryResult ServiceEntryController::lookupServiceCode(const string& code) const {
  ServiceEntryResult res;

  if (!strutil::isDigits(code) || code.size() != 6) {
    res.message = "Invalid service code (must be 6 digits).";
    return res;
  }

  auto found = svcRepo_.findByCode(code);
  if (!found.has_value()) {
    res.message = "Service code not found.";
    return res;
  }

  ostringstream feeStr;
  feeStr << fixed << setprecision(2) << found->fee;

  res.success = true;
  res.fee = found->fee;
  res.serviceName = found->name;
  res.message = found->name + " ($" + feeStr.str() + ")";
  return res;
}

bool ServiceEntryController::saveRecord(const ServiceRecord& record) const {
  return recordRepo_.save(record);
}

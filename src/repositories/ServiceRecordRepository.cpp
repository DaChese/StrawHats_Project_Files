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

#include "repositories/ServiceRecordRepository.h"
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

ServiceRecordRepository::ServiceRecordRepository(const string& path)
  : path_(path) {}

bool ServiceRecordRepository::ensureHeader_() {
  // Write locked header only if file is missing or empty
  {
    ifstream check(path_);
    if (check.is_open()) {
      check.seekg(0, ios::end);
      if (check.tellg() > 0) return true; // already has content
    }
  }
  ofstream out(path_);
  if (!out.is_open()) return false;
  out << HEADER << "\n";
  return true;
}

bool ServiceRecordRepository::save(const ServiceRecord& r) {
  if (!ensureHeader_()) return false;

  ofstream out(path_, ios::app);
  if (!out.is_open()) return false;

  // comments are optional — written as quoted field (may be empty string)
  out << r.timestamp      << ","
      << r.dateOfService  << ","
      << r.providerNumber << ","
      << r.memberNumber   << ","
      << r.serviceCode    << ","
      << fixed << setprecision(2) << r.fee << ","
      << "\"" << r.comments << "\"\n";
  return true;
}

vector<ServiceRecord> ServiceRecordRepository::loadAll() const {
  vector<ServiceRecord> records;
  ifstream in(path_);
  if (!in.is_open()) return records;

  string line;
  bool first = true;
  while (getline(in, line)) {
    if (first) { first = false; continue; } // skip header
    if (line.empty()) continue;

    // manual split: first 6 commas are plain, last field is quoted
    // format: timestamp,dateOfService,providerNumber,memberNumber,serviceCode,fee,"comments"
    auto splitFixed = [&](const string& s) -> vector<string> {
      vector<string> cols;
      size_t pos = 0;
      for (int i = 0; i < 6; i++) {
        size_t comma = s.find(',', pos);
        if (comma == string::npos) { cols.push_back(s.substr(pos)); return cols; }
        cols.push_back(s.substr(pos, comma - pos));
        pos = comma + 1;
      }
      // remainder is the quoted comments field
      string rem = s.substr(pos);
      if (!rem.empty() && rem.front() == '"' && rem.back() == '"')
        rem = rem.substr(1, rem.size() - 2);
      cols.push_back(rem);
      return cols;
    };

    auto cols = splitFixed(line);
    if (cols.size() < 7) continue;

    ServiceRecord r;
    r.timestamp      = cols[0];
    r.dateOfService  = cols[1];
    r.providerNumber = cols[2];
    r.memberNumber   = cols[3];
    r.serviceCode    = cols[4];
    try { r.fee = stod(cols[5]); } catch (...) { r.fee = 0.0; }
    r.comments       = cols[6];
    records.push_back(r);
  }
  return records;
}

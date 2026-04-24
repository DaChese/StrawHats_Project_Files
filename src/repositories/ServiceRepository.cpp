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

#include "repositories/ServiceRepository.h"
#include "util/Csv.h"
#include "util/StringUtil.h"
#include <algorithm>
#include <fstream>
using namespace std;

bool ServiceRepository::loadFromCsv(const string& path) {
  ifstream in(path);
  if (!in.is_open()) return false;

  string line;
  bool first = true;

  while (getline(in, line)) {
    if (first) { first = false; continue; }
    if (line.empty()) continue;

    auto cols = csv::splitLine(line);
    if (cols.size() < 3) continue;

    ServiceCode sc;
    sc.code = strutil::trim(cols[0]);
    sc.name = strutil::trim(cols[1]);
    try { sc.fee = stod(strutil::trim(cols[2])); } catch (...) { sc.fee = 0.0; }

    if (!sc.code.empty())
      codes_[sc.code] = sc;
  }
  return true;
}

optional<ServiceCode> ServiceRepository::findByCode(const string& code) const {
  auto it = codes_.find(code);
  if (it == codes_.end()) return nullopt;
  return it->second;
}

vector<ServiceCode> ServiceRepository::allSortedByName() const {
  vector<ServiceCode> v;
  v.reserve(codes_.size());
  for (auto& [k, sc] : codes_) v.push_back(sc);
  sort(v.begin(), v.end(), [](const ServiceCode& a, const ServiceCode& b) {
    return a.name < b.name;
  });
  return v;
}

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
 * Purpose: In-memory provider store backed by providers.csv; supports CRUD + persistence.
 */

// Primary owner: Aldo Medina (UC02 / provider login path)

#include "repositories/ProviderRepository.h"
#include "util/Csv.h"
#include "util/StringUtil.h"
#include <fstream>
using namespace std;

bool ProviderRepository::loadFromCsv(const string& path) {
  ifstream in(path);
  if (!in.is_open()) return false;

  string line;
  bool first = true;
  while (getline(in, line)) {
    if (first) { first = false; continue; }
    if (line.empty()) continue;
    auto cols = csv::splitLine(line);
    if (cols.size() < 3) continue;
    Provider p;
    p.providerNumber = strutil::trim(cols[0]);
    p.name           = strutil::trim(cols[1]);
    p.pin            = strutil::trim(cols[2]);
    if (!p.providerNumber.empty()) providers_[p.providerNumber] = p;
  }
  return true;
}

bool ProviderRepository::saveToCsv(const string& path) const {
  ofstream out(path);
  if (!out.is_open()) return false;
  out << "providerNumber,name,pin\n";
  for (auto& [k, p] : providers_)
    out << p.providerNumber << "," << p.name << "," << p.pin << "\n";
  return true;
}

optional<Provider> ProviderRepository::findByNumber(const string& providerNumber) const {
  auto it = providers_.find(providerNumber);
  if (it == providers_.end()) return nullopt;
  return it->second;
}

vector<Provider> ProviderRepository::all() const {
  vector<Provider> v;
  v.reserve(providers_.size());
  for (auto& [k, p] : providers_) v.push_back(p);
  return v;
}

bool ProviderRepository::add(const Provider& p) {
  if (providers_.count(p.providerNumber)) return false;
  providers_[p.providerNumber] = p;
  return true;
}

bool ProviderRepository::update(const Provider& p) {
  if (!providers_.count(p.providerNumber)) return false;
  providers_[p.providerNumber] = p;
  return true;
}

bool ProviderRepository::remove(const string& number) {
  return providers_.erase(number) > 0;
}

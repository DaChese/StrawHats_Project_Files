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
 * Purpose: In-memory member store backed by members.csv; supports CRUD + persistence.
 */

// Primary owner: Lulu Chen (UC01 / member validation + data management)

#include "repositories/MemberRepository.h"
#include "util/Csv.h"
#include "util/StringUtil.h"
#include <fstream>
using namespace std;

bool MemberRepository::loadFromCsv(const string& path) {
  ifstream in(path);
  if (!in.is_open()) return false;

  string line;
  bool first = true;
  while (getline(in, line)) {
    if (first) { first = false; continue; }
    if (line.empty()) continue;
    auto cols = csv::splitLine(line);
    if (cols.size() < 3) continue;
    Member m;
    m.memberNumber = strutil::trim(cols[0]);
    m.name         = strutil::trim(cols[1]);
    m.status       = strutil::trim(cols[2]);
    if (!m.memberNumber.empty()) members_[m.memberNumber] = m;
  }
  return true;
}

bool MemberRepository::saveToCsv(const string& path) const {
  ofstream out(path);
  if (!out.is_open()) return false;
  out << "memberNumber,name,status\n";
  for (auto& [k, m] : members_)
    out << m.memberNumber << "," << m.name << "," << m.status << "\n";
  return true;
}

optional<Member> MemberRepository::findByNumber(const string& memberNumber) const {
  auto it = members_.find(memberNumber);
  if (it == members_.end()) return nullopt;
  return it->second;
}

vector<Member> MemberRepository::all() const {
  vector<Member> v;
  v.reserve(members_.size());
  for (auto& [k, m] : members_) v.push_back(m);
  return v;
}

bool MemberRepository::add(const Member& m) {
  if (members_.count(m.memberNumber)) return false;
  members_[m.memberNumber] = m;
  return true;
}

bool MemberRepository::update(const Member& m) {
  if (!members_.count(m.memberNumber)) return false;
  members_[m.memberNumber] = m;
  return true;
}

bool MemberRepository::remove(const string& number) {
  return members_.erase(number) > 0;
}

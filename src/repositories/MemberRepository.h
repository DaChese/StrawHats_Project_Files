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

#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
#include "models/Member.h"

class MemberRepository {
public:
  bool loadFromCsv(const string& path);
  bool saveToCsv(const string& path) const;

  optional<Member> findByNumber(const string& memberNumber) const;
  vector<Member>   all() const;

  bool add(const Member& m);          // false if number already exists
  bool update(const Member& m);       // false if not found
  bool remove(const string& number);  // false if not found

private:
  unordered_map<string, Member> members_;
};

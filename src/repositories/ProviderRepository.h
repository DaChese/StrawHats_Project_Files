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

#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
#include "models/Provider.h"

class ProviderRepository {
public:
  bool loadFromCsv(const string& path);
  bool saveToCsv(const string& path) const;

  optional<Provider> findByNumber(const string& providerNumber) const;
  vector<Provider>   all() const;

  bool add(const Provider& p);
  bool update(const Provider& p);
  bool remove(const string& number);

private:
  unordered_map<string, Provider> providers_;
};

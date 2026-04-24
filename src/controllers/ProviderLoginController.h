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
 * Purpose: Validates provider number + PIN against the provider repo; returns a LoginResult.
 */

// Primary owner: Aldo Medina (UC02 / provider login)

#pragma once
#include <optional>
#include <string>
using namespace std;
#include "models/Provider.h"
#include "repositories/ProviderRepository.h"

struct LoginResult {
  bool success = false;
  string message;
  optional<Provider> provider;
};

class ProviderLoginController {
public:
  explicit ProviderLoginController(const ProviderRepository& repo);
  LoginResult login(const string& providerNumber, const string& pin) const;

private:
  const ProviderRepository& repo_;
};

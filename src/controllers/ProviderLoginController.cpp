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

#include "controllers/ProviderLoginController.h"
#include "util/StringUtil.h"
using namespace std;

ProviderLoginController::ProviderLoginController(const ProviderRepository& repo)
  : repo_(repo) {}

LoginResult ProviderLoginController::login(const string& providerNumber, const string& pin) const {
  LoginResult res;

  if (!strutil::isDigits(providerNumber) || providerNumber.size() != 9) {
    res.message = "Invalid provider number (must be 9 digits).";
    return res;
  }
  if (!strutil::isDigits(pin) || pin.size() != 4) {
    res.message = "Invalid PIN (must be 4 digits).";
    return res;
  }

  auto found = repo_.findByNumber(providerNumber);
  if (!found.has_value()) {
    res.message = "Provider not found.";
    return res;
  }
  if (found->pin != pin) {
    res.message = "Incorrect PIN.";
    return res;
  }

  res.success = true;
  res.message = "Login successful.";
  res.provider = found;
  return res;
}

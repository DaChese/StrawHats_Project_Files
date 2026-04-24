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
 * Purpose: Checks member number format and status (VALIDATED / SUSPENDED / NOT_FOUND).
 */

// Primary owner: Lulu Chen (UC01 / member validation)

#pragma once
#include <string>
using namespace std;
#include "repositories/MemberRepository.h"

enum class MemberStatusResult {
  VALIDATED,
  SUSPENDED,
  NOT_FOUND,
  INVALID_INPUT
};

struct MemberValidationResult {
  MemberStatusResult status;
  string message;
};

class MemberValidationController {
public:
  explicit MemberValidationController(const MemberRepository& repo);
  MemberValidationResult validate(const string& memberNumber) const;

private:
  const MemberRepository& repo_;
};

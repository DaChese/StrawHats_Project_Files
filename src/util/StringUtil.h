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
 * Purpose: Shared string helpers used across repos and controllers.
 */

// Primary owner: all (shared utility)

#pragma once
#include <string>
using namespace std;

namespace strutil {
  string trim(const string& s);    // strips leading/trailing whitespace
  bool isDigits(const string& s);  // true only if every char is 0-9
}

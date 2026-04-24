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
 * Purpose: Minimal CSV parser — splits a single line, handles quoted fields.
 */

// Primary owner: all (shared utility)

#pragma once
#include <string>
#include <vector>
using namespace std;

namespace csv {
  // Splits one CSV line into fields; handles commas inside quoted fields.
  vector<string> splitLine(const string& line);
}

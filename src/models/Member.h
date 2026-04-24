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
 * Purpose: Plain data structs for Member, Provider, ServiceCode, and ServiceRecord.
 */

// Primary owner: all (shared models)

#pragma once
#include <string>
using namespace std;

struct Member {
  string memberNumber; // 9 digits
  string name;
  string status;       // "VALIDATED" or "SUSPENDED"
};

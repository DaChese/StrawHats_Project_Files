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

// Primary owner: Aldo Medina (UC02 / service record save path)

#pragma once
#include <string>
using namespace std;

struct ServiceRecord {
  string timestamp;       // MM-DD-YYYY HH:MM:SS (when record was created)
  string dateOfService;   // MM-DD-YYYY (entered by provider)
  string providerNumber;  // 9 digits
  string memberNumber;    // 9 digits
  string serviceCode;     // 6 digits
  double fee = 0.0;
  string comments;        
};

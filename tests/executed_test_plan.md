# Executed Test Plan — Straw Hats ChocAn

## Locked Output Tag Reference

| Tag | Exact string printed |
|-----|----------------------|
| Login success | `[LOGIN_SUCCESS] Login successful.` |
| Login fail — not found | `[LOGIN_FAIL] Provider not found.` |
| Login fail — wrong PIN | `[LOGIN_FAIL] Incorrect PIN.` |
| Login fail — bad number | `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` |
| Login fail — bad PIN | `[LOGIN_FAIL] Invalid PIN (must be 4 digits).` |
| Member OK | `[MEMBER_OK] Member is validated.` |
| Member suspended | `[MEMBER_SUSPENDED] Member is suspended.` |
| Member not found | `[MEMBER_FAIL] Member not found.` |
| Member bad input | `[MEMBER_FAIL] Invalid member number (must be 9 digits).` |
| Date accepted | `[DATE_OK] Date accepted.` |
| Date rejected | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` |
| Code accepted | `[CODE_OK] Service confirmed.` |
| Code bad format | `[CODE_FAIL] Invalid service code (must be 6 digits).` |
| Code not found | `[CODE_FAIL] Service code not found.` |
| Record saved | `[RECORD_SAVED] Service record saved successfully.` |
| Cancelled | `[CANCELLED] Cancelled. No record saved.` |
| Save failed | `[RECORD_FAIL] Save failed.` |
| Back | `[NAV] Back` |
| Logout | `[NAV] Logout` |
| Directory OK | `[DIR_OK] Provider Directory created: <path>` |
| Directory fail | `[DIR_FAIL] Could not write directory. Check output folder.` |
| Batch OK | `[BATCH_OK] Reports generated in: <folder>` |
| Batch fail | `[BATCH_FAIL] Invalid date or could not create output folder.` |
| CRUD OK | `[CRUD_OK] ...` |
| CRUD fail | `[CRUD_FAIL] ...` |
| Update OK | `[UPDATE_OK] ...` |
| Update fail | `[UPDATE_FAIL] ...` |

---

## Test Results

| ID | Category | Name | Expected Tag | Actual Results | Pass/Fail |
|----|----------|------|-------------|----------------|-----------|
| BB-PV-01 | — | Login — success | `[LOGIN_SUCCESS] Login successful.` | [LOGIN_SUCCESS] Login successful. | PASS |
| BB-PV-02 | — | Login — wrong PIN | `[LOGIN_FAIL] Incorrect PIN.` | [LOGIN_FAIL] Incorrect PIN. | PASS |
| BB-PV-03 | — | Login — not found | `[LOGIN_FAIL] Provider not found.` | [LOGIN_FAIL] Provider not found. | PASS |
| BB-PV-04 | — | Login — bad number (short) | `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` | [LOGIN_FAIL] Invalid provider number (must be 9 digits). | PASS |
| BB-PV-05 | — | Login — bad number (non-digit) | `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` | [LOGIN_FAIL] Invalid provider number (must be 9 digits). | PASS |
| BB-PV-06 | — | Login — bad PIN (short) | `[LOGIN_FAIL] Invalid PIN (must be 4 digits).` | [LOGIN_FAIL] Invalid PIN (must be 4 digits). | PASS |
| BB-PV-07 | — | Member — validated | `[MEMBER_OK] Member is validated.` | [MEMBER_OK] Member is validated. | PASS |
| BB-PV-08 | — | Member — suspended | `[MEMBER_SUSPENDED] Member is suspended.` | [MEMBER_SUSPENDED] Member is suspended. | PASS |
| BB-PV-09 | — | Member — not found | `[MEMBER_FAIL] Member not found.` | [MEMBER_FAIL] Member not found. | PASS |
| BB-PV-10 | — | Member — bad input (short) | `[MEMBER_FAIL] Invalid member number (must be 9 digits).` | [MEMBER_FAIL] Invalid member number (must be 9 digits). | PASS |
| BB-PV-11 | — | Date — bad month | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | [DATE_FAIL] Invalid date format. Use MM-DD-YYYY. | PASS |
| BB-PV-12 | — | Date — wrong separator | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | [DATE_FAIL] Invalid date format. Use MM-DD-YYYY. | PASS |
| BB-PV-13 | — | Date — non-digit | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | [DATE_FAIL] Invalid date format. Use MM-DD-YYYY. | PASS |
| BB-PV-14 | — | Date — accepted | `[DATE_OK] Date accepted.` | [DATE_OK] Date accepted. | PASS |
| BB-PV-15 | — | Code — too short | `[CODE_FAIL] Invalid service code (must be 6 digits).` | [CODE_FAIL] Invalid service code (must be 6 digits). | PASS |
| BB-PV-16 | — | Code — not found | `[CODE_FAIL] Service code not found.` | [CODE_FAIL] Service code not found. | PASS |
| BB-PV-17 | — | Code — accepted | `[CODE_OK] Service confirmed.` | [CODE_OK] Service confirmed. | PASS |
| BB-PV-18 | — | Cancel — no record written | `[CANCELLED] Cancelled. No record saved.` | [CANCELLED] Cancelled. No record saved. | PASS |
| BB-PV-19 | — | Happy path — record saved | `[RECORD_SAVED] Service record saved successfully.` | [RECORD_SAVED] Service record saved successfully. | PASS |
| BB-PV-20 | — | Happy path — blank comments | `[RECORD_SAVED] Service record saved successfully.` | [RECORD_SAVED] Service record saved successfully. | PASS |
| BB-PV-21 | — | Navigation — back at member | `[NAV] Back` | [NAV] Back | PASS |
| BB-PV-22 | — | Navigation — back at date | `[NAV] Back` | [NAV] Back | PASS |
| BB-PV-23 | — | Navigation — back at code | `[NAV] Back` | [NAV] Back | PASS |
| BB-PV-24 | — | Navigation — logout from menu | `[NAV] Logout` | [NAV] Logout | PASS |
| UC05-01 | — | Directory generation — success | `[DIR_OK] Provider Directory created:` | [DIR_OK] Provider Directory created: data/output/provider_directory_20260428.txt | PASS |
| UC04-01 | — | Batch — valid date | `[BATCH_OK] Reports generated in:` | [BATCH_OK] Reports generated in: data/output/reports/week_20250421 | PASS |
| UC04-02 | — | Batch — invalid date | `[BATCH_FAIL] Invalid date or could not create output folder.` | [BATCH_FAIL] Invalid date or could not create output folder. | PASS |
| OP-MEM-01 | — | Member add — success | `[CRUD_OK] Member 900000001 added.` | [CRUD_OK] Member 900000001 added. | PASS |
| OP-MEM-02 | — | Member add — duplicate | `[CRUD_FAIL] Member 111111111 already exists.` | [CRUD_FAIL] Member 111111111 already exists. | PASS |
| OP-MEM-03 | — | Member add — invalid number | `[CRUD_FAIL] Member number must be 9 digits.` | [CRUD_FAIL] Member number must be 9 digits. | PASS |
| OP-MEM-04 | — | Member update — success | `[CRUD_OK] Member 111111111 updated.` | [CRUD_OK] Member 111111111 updated. | PASS |
| OP-MEM-05 | — | Member update — not found | `[CRUD_FAIL] Member 000000001 not found.` | [CRUD_FAIL] Member 000000001 not found. | PASS |
| OP-MEM-06 | — | Member delete — success | `[CRUD_OK] Member 111111111 deleted.` | [CRUD_OK] Member 111111111 deleted. | PASS |
| OP-MEM-07 | — | Member delete — not found | `[CRUD_FAIL] Member 000000001 not found.` | [CRUD_FAIL] Member 000000001 not found. | PASS |
| OP-PRV-01 | — | Provider add — success | `[CRUD_OK] Provider 900000002 added.` | [CRUD_OK] Provider 900000002 added. | PASS |
| OP-PRV-02 | — | Provider add — duplicate | `[CRUD_FAIL] Provider 123456789 already exists.` | [CRUD_FAIL] Provider 123456789 already exists. | PASS |
| OP-PRV-03 | — | Provider add — invalid PIN | `[CRUD_FAIL] PIN must be 4 digits.` | [CRUD_FAIL] PIN must be 4 digits. | PASS |
| OP-PRV-04 | — | Provider update — success | `[CRUD_OK] Provider 123456789 updated.` | [CRUD_OK] Provider 123456789 updated. | PASS |
| OP-PRV-05 | — | Provider delete — success | `[CRUD_OK] Provider 123456789 deleted.` | [CRUD_OK] Provider 123456789 deleted. | PASS |
| OP-ACM-01 | — | Acme import — default path success | `[UPDATE_OK] Import complete.` | [UPDATE_OK] Import complete. Updated: 4, Skipped: 0. | PASS |
| OP-ACM-02 | — | Acme import — missing file | `[UPDATE_FAIL] Cannot open update file:` | [UPDATE_FAIL] Cannot open update file: data/nonexistent_file.csv | PASS |

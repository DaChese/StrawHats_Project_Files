# Black-Box Test Plan — Provider/Validation Module
## Strict String-Match Edition

Test IDs: **BB-PV-XX**
Pass criteria: the exact tag line listed in "Expected Output" must appear verbatim in stdout.
"Actual Results" and "Pass/Fail" are filled in after execution.

---

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

---

## Locked CSV Schema

| File | Header (exact, frozen) |
|------|------------------------|
| `providers.csv` | `providerNumber,name,pin` |
| `members.csv` | `memberNumber,name,status` |
| `services.csv` | `code,name,fee` |
| `service_records.csv` | `timestamp,dateOfService,providerNumber,memberNumber,serviceCode,fee,comments` |

---

## Test Cases

| ID | Category | Input Sequence | Expected Output (exact tag line) | Actual Results | Pass/Fail |
|----|----------|----------------|----------------------------------|----------------|-----------|
| BB-PV-01 | Login — success | Provider#: `123456789`, PIN: `1234` | `[LOGIN_SUCCESS] Login successful.` | | |
| BB-PV-02 | Login — wrong PIN | Provider#: `123456789`, PIN: `0000` | `[LOGIN_FAIL] Incorrect PIN.` | | |
| BB-PV-03 | Login — not found | Provider#: `000000000`, PIN: `1234` | `[LOGIN_FAIL] Provider not found.` | | |
| BB-PV-04 | Login — bad number (short) | Provider#: `12345`, PIN: `1234` | `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` | | |
| BB-PV-05 | Login — bad number (non-digit) | Provider#: `12345678A`, PIN: `1234` | `[LOGIN_FAIL] Invalid provider number (must be 9 digits).` | | |
| BB-PV-06 | Login — bad PIN (short) | Provider#: `123456789`, PIN: `12` | `[LOGIN_FAIL] Invalid PIN (must be 4 digits).` | | |
| BB-PV-07 | Member — validated | Logged in; Member#: `111111111` | `[MEMBER_OK] Member is validated.` | | |
| BB-PV-08 | **Member — suspended** | Logged in; Member#: `222222222` | `[MEMBER_SUSPENDED] Member is suspended.` | | |
| BB-PV-09 | Member — not found | Logged in; Member#: `999999998` | `[MEMBER_FAIL] Member not found.` | | |
| BB-PV-10 | Member — bad input (short) | Logged in; Member#: `1234` | `[MEMBER_FAIL] Invalid member number (must be 9 digits).` | | |
| BB-PV-11 | **Date — bad month** | Logged in, validated member; Date: `13-01-2026` | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | | |
| BB-PV-12 | **Date — wrong separator** | Logged in, validated member; Date: `04/21/2026` | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | | |
| BB-PV-13 | **Date — non-digit** | Logged in, validated member; Date: `hello` | `[DATE_FAIL] Invalid date format. Use MM-DD-YYYY.` | | |
| BB-PV-14 | Date — accepted | Logged in, validated member; Date: `04-21-2026` | `[DATE_OK] Date accepted.` | | |
| BB-PV-15 | **Code — too short** | Logged in, validated member, valid date; Code: `1234` | `[CODE_FAIL] Invalid service code (must be 6 digits).` | | |
| BB-PV-16 | **Code — not found** | Logged in, validated member, valid date; Code: `999999` | `[CODE_FAIL] Service code not found.` | | |
| BB-PV-17 | Code — accepted | Logged in, validated member, valid date; Code: `100001` | `[CODE_OK] Service confirmed.` | | |
| BB-PV-18 | **Cancel — no record written** | Full flow to confirm; Confirm: `n` | `[CANCELLED] Cancelled. No record saved.` | | |
| BB-PV-19 | **Happy path — record saved** | Full flow; Confirm: `y`, Comments: `Annual checkup` | `[RECORD_SAVED] Service record saved successfully.` | | |
| BB-PV-20 | Happy path — blank comments | Full flow; Confirm: `y`, Comments: *(Enter)* | `[RECORD_SAVED] Service record saved successfully.` | | |
| BB-PV-21 | Navigation — back at member | Logged in; Member#: `b` | `[NAV] Back` | | |
| BB-PV-22 | Navigation — back at date | Logged in, validated member; Date: `b` | `[NAV] Back` | | |
| BB-PV-23 | Navigation — back at code | Logged in, validated member, valid date; Code: `b` | `[NAV] Back` | | |
| BB-PV-24 | Navigation — logout from menu | Logged in; Choice: `q` | `[NAV] Logout` | | |

---

## Scenario Walkthroughs (for screenshot evidence)

### Scenario A — Suspended member blocks service entry (BB-PV-08)
```
[LOGIN_SUCCESS] Login successful.

  [1] Submit Service Record
  [2] Request Provider Directory (UC05)
  [q] Log out
  Choice: 1
  Member # (9 digits, 'b' back): 222222222
  [MEMBER_SUSPENDED] Member is suspended.

  [1] Submit Service Record
  ...
```
Date/code prompts must NOT appear after `[MEMBER_SUSPENDED]`.

### Scenario B — Invalid date rejected, re-prompts (BB-PV-11, BB-PV-12, BB-PV-13, BB-PV-14)
```
  Member # (9 digits, 'b' back): 111111111
  [MEMBER_OK] Member is validated.
  Date of service (MM-DD-YYYY, 'b' retry member): 13-01-2026
  [DATE_FAIL] Invalid date format. Use MM-DD-YYYY.
  Date of service (MM-DD-YYYY, 'b' retry member): 04/21/2026
  [DATE_FAIL] Invalid date format. Use MM-DD-YYYY.
  Date of service (MM-DD-YYYY, 'b' retry member): 04-21-2026
  [DATE_OK] Date accepted.
```

### Scenario C — Invalid service code rejected, re-prompts (BB-PV-15, BB-PV-16, BB-PV-17)
```
  Service code (6 digits, 'b' retry date): 1234
  [CODE_FAIL] Invalid service code (must be 6 digits).
  Service code (6 digits, 'b' retry date): 999999
  [CODE_FAIL] Service code not found.
  Service code (6 digits, 'b' retry date): 100001
  [CODE_OK] Service confirmed.
```

### Scenario D — Cancel flow, no record written (BB-PV-18)
```
  [CODE_OK] Service confirmed.
  [SERVICE_FOUND] Dietitian Consultation — fee: $50.00
  Confirm service? (y/n): n
  [CANCELLED] Cancelled. No record saved.
```
Verify `service_records.csv` row count did NOT increase.

### Scenario E — Happy path, record appended (BB-PV-19)
```
  Confirm service? (y/n): y
  Comments (press Enter to skip, max 100 chars): Annual checkup
  [RECORD_SAVED] Service record saved successfully.
```
Verify `service_records.csv` has exactly one new row with correct fields.

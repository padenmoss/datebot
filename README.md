Here is a self-contained C++ program that implements the custom calendar you described and prints the date in +%F style (YYYY-MM-DD).

Calendar rules used

• 14 months, each with exactly 26 days → 364 regular days.
• The remaining day(s) of the solar year are handled as special days:
 ○ New Year’s Day falls on Halloween (31 October) every year.
 ○ Leap Day falls on All Saints’ Day (1 November) in leap years.

• A year in this system runs from one Halloween to the next.
• Leap years follow the normal Gregorian rule (divisible by 4, except centuries not divisible by 400).
• Month numbers run 01–14; day numbers run 01–26.
• The special days themselves are reported as month 00 (or you can change that easily).

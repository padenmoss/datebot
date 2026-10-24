#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <sstream>
#include <stdexcept>

// ------------------------------------------------------------------
// Gregorian helpers
// ------------------------------------------------------------------
bool is_gregorian_leap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

// Julian Day Number (proleptic Gregorian)
long long jdn(int y, int m, int d) {
    long long a = (14 - m) / 12;
    long long yy = y + 4800 - a;
    long long mm = m + 12 * a - 3;
    return d + (153 * mm + 2) / 5 + 365 * yy
         + yy / 4 - yy / 100 + yy / 400 - 32045;
}

// ------------------------------------------------------------------
// Custom calendar
// ------------------------------------------------------------------
struct CustomDate {
    int year;
    int month;          // 1-14 or 0 for the two special days
    int day;            // 1-26 or 0/1 for the special days
    bool is_new_year;   // Halloween  → xx-00-00
    bool is_leap_day;   // All Saints → xx-00-01 (only in leap years)
};

CustomDate to_custom(int gy, int gm, int gd) {
    // Reject non-existent Gregorian dates
    if (gm == 2 && gd == 29 && !is_gregorian_leap(gy))
        throw std::runtime_error("Invalid Gregorian date");

    // --------------------------------------------------------------
    // Special days belong to the custom year that is ending
    // --------------------------------------------------------------
    if (gm == 10 && gd == 31) {                 // Halloween = New Year
        CustomDate cd;
        cd.year = gy;
        cd.month = 0;
        cd.day = 0;
        cd.is_new_year = true;
        cd.is_leap_day = false;
        return cd;
    }
    if (gm == 11 && gd == 1 && is_gregorian_leap(gy)) { // All Saints = Leap Day
        CustomDate cd;
        cd.year = gy;
        cd.month = 0;
        cd.day = 1;
        cd.is_new_year = false;
        cd.is_leap_day = true;
        return cd;
    }

    // --------------------------------------------------------------
    // Ordinary days
    // Custom year Y starts the day after the special day(s) of
    // Gregorian year (Y-1) and runs until 30 October of Gregorian year Y.
    // --------------------------------------------------------------
    int cy;
    if (gm >= 11) {
        // 2 Nov … 31 Dec → custom year gy+1
        cy = gy + 1;
    } else {
        // 1 Jan … 30 Oct → custom year gy
        cy = gy;
    }

    // First regular day of custom year cy
    // = day after the special day(s) of Gregorian year (cy-1)
    int prev = cy - 1;
    long long first_regular;
    if (is_gregorian_leap(prev)) {
        first_regular = jdn(prev, 11, 2);     // after Leap Day
    } else {
        first_regular = jdn(prev, 11, 1);     // after New Year only
    }

    long long today = jdn(gy, gm, gd);
    long long offset = today - first_regular; // 0 → 01-01

    if (offset < 0) {
        throw std::runtime_error("Internal offset error");
    }

    CustomDate cd;
    cd.year = cy;
    cd.month = static_cast<int>(offset / 26) + 1;
    cd.day   = static_cast<int>(offset % 26) + 1;
    cd.is_new_year = false;
    cd.is_leap_day = false;

    if (cd.month > 14) {
        cd.month = 14;
        cd.day = 26;
    }
    return cd;
}

// ------------------------------------------------------------------
// Formatting (+%F style)
// ------------------------------------------------------------------
std::string format(const CustomDate& cd) {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(4) << cd.year << '-';

    if (cd.is_new_year) {
        os << "00-00";
    } else if (cd.is_leap_day) {
        os << "00-01";
    } else {
        os << std::setw(2) << cd.month << '-'
           << std::setw(2) << cd.day;
    }
    return os.str();
}

// ------------------------------------------------------------------
int main() {
    std::cout << "Enter Gregorian date as YYYY-MM-DD (empty = today):\n";
    std::string line;
    std::getline(std::cin, line);

    int y, m, d;
    if (line.empty()) {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        y = now->tm_year + 1900;
        m = now->tm_mon + 1;
        d = now->tm_mday;
    } else {
        if (sscanf(line.c_str(), "%d-%d-%d", &y, &m, &d) != 3) {
            std::cerr << "Invalid format. Use YYYY-MM-DD\n";
            return 1;
        }
    }

    try {
        CustomDate cd = to_custom(y, m, d);
        std::cout << "Custom (+%F): " << format(cd) << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}

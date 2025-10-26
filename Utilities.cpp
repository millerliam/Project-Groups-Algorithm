#include "Utilities.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace std;

string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c){ return tolower(c); });
    return result;
}

int convertSkillLevel(const string& skill) {
    string lower_skill = toLowerCase(skill);
    if (lower_skill == "beginner") {
        return 1;
    } else if (lower_skill == "intermediate") {
        return 2;
    } else if (lower_skill == "advanced") {
        return 3;
    } else {
        throw invalid_argument("Invalid skill level: " + skill);
    }
    return -1; // Add a default return value to satisfy the compiler about returning an int on all paths
}

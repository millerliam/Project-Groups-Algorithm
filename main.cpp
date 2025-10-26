#include "TeamBuilder.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

// Function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

// Function to parse CSV and return a vector of Students
vector<Student> readCSV(const string& filename) {
    vector<Student> students;
    ifstream file(filename);
    string line;
    if (file.is_open()) {
        getline(file, line); // Skip the header line
        while (getline(file, line)) {
            stringstream ss(line);
            Student student;
            string skill, token, doNotWorkWith, workWith;

            getline(ss, student.username, ',');
            student.username = toLowerCase(trim(student.username));

            getline(ss, skill, ',');
            student.programming_skill = convertSkillLevel(trim(skill));
            getline(ss, skill, ',');
            student.debugging_skill = convertSkillLevel(trim(skill));
            getline(ss, skill, ',');
            student.algorithm_skill = convertSkillLevel(trim(skill));

            // Read ';' as a separator for 'do not want to work with'
            getline(ss, doNotWorkWith, ',');
            stringstream dnwwStream(doNotWorkWith);
            while (getline(dnwwStream, token, ';')) {
                string trimmed_token = trim(token);
                if (!trimmed_token.empty()) {
                    student.dont_want_to_work_with.push_back(toLowerCase(trimmed_token));
                }
            }

            // Read ';' as a separator for 'want to work with'
            getline(ss, workWith, ',');
            stringstream wwStream(workWith);
            while (getline(wwStream, token, ';')) {
                string trimmed_token = trim(token);
                if (!trimmed_token.empty()) {
                    student.want_to_work_with.push_back(toLowerCase(trimmed_token));
                }
            }

            students.push_back(student);
        }
        file.close();
    } else {
        cerr << "Error opening file: " << filename << endl;
    }
    return students;
}


int main() {
    string filename;
    int team_size;
    bool prioritize_preferences;

    cout << "Enter the CSV file name (1 for Pref1, 2 for Pref2, 3 for Pref3): ";
    int file_choice;
    cin >> file_choice;

    // Set filename based on user choice
    switch (file_choice) {
        case 1:
            filename = "data/Assignment4 Roster Pref1.csv";
            break;
        case 2:
            filename = "data/Assignment4 Roster Pref2.csv";
            break;
        case 3:
            filename = "data/Assignment4 Roster Pref3.csv";
            break;
        default:
            cout << "Invalid choice. Exiting.";
            return 1;
    }

    
    cout << "Enter the team size (3 or 4): ";
    cin >> team_size;

    cout << "Prioritize preferences (1) or skill balance (0)?: ";
    cin >> prioritize_preferences;

    // Read students from the CSV file
    vector<Student> students = readCSV(filename);

    // Check if any students were read
    if (students.empty()) {
        cerr << "No students found. Exiting." << endl;
        return 1;
    }

    TeamBuilder teamBuilder(students, team_size, !prioritize_preferences);

    // Form teams based on the whether the user chose to group by skill or preferences
    teamBuilder.formTeams(prioritize_preferences);

    // Print the teams and their scores
    teamBuilder.printTeamsAndScores();

    // Write the teams to a CSV file
    try {
        teamBuilder.writeTeamsToFile("teams_output.csv");
    } catch (const exception& e) {
        cerr << "Error writing teams to file: " << e.what() << endl;
        return 1;
    }

    return 0;
}

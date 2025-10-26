#include "TeamBuilder.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cmath>
#include <numeric>

using namespace std;

// Definition of the vectorEquals function
bool vectorEquals(const vector<Student>& a, const vector<Student>& b) {
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].username != b[i].username)
            return false;
    }

    return true;
}

// Constructor for TeamBuilder
TeamBuilder::TeamBuilder(const vector<Student>& students, int team_size, bool prioritize_skills)
    : students(students), team_size(team_size), prioritize_skills(prioritize_skills) {
    loadStudentMap();
}

// Load student data into a map for quick lookups
void TeamBuilder::loadStudentMap() {
    for (const auto& student : students) {
        student_map[student.username] = student;
    }
}

// Check if new member can work with existing team members
bool TeamBuilder::canWorkTogether(const vector<Student>& team, const Student& new_member) {
    for (const auto& member : team) {
        if (find(member.dont_want_to_work_with.begin(), member.dont_want_to_work_with.end(), new_member.username) != member.dont_want_to_work_with.end()) {
            return false;
        }
        if (find(new_member.dont_want_to_work_with.begin(), new_member.dont_want_to_work_with.end(), member.username) != new_member.dont_want_to_work_with.end()) {
            return false;
        }
    }
    return true;
}

// Form teams based on either preferences or skills
void TeamBuilder::formTeams(bool prioritize_preferences) {
    if (prioritize_preferences) {
        formTeamsByPreferences();
    } else {
        formTeamsBySkills();
    }
    calculateTeamScores();
}

// Form teams prioritizing preferences
void TeamBuilder::formTeamsByPreferences() {
    set<string> assigned_students;
    vector<Student> remaining_students = students;
    vector<Student> team_leaders;
    size_t leader_index = 0;

    // Calculate the number of teams needed
    int num_teams = ceil(static_cast<double>(students.size()) / team_size);
    teams.resize(num_teams);

    // Debug: count students with preferences
    int students_with_preferences = 0;
    for (const auto& student : students) {
        if (!student.want_to_work_with.empty()) {
            students_with_preferences++;
        }
    }
    cout << "Total students with preferences: " << students_with_preferences << endl;

    // Helper function to assign a student to a team
    auto assignStudentToTeam = [&](Student& student, vector<Student>& team) {
        team.push_back(student);
        assigned_students.insert(student.username);
        remaining_students.erase(remove_if(remaining_students.begin(), remaining_students.end(), [&](const Student& s) {
            return s.username == student.username;
        }), remaining_students.end());
    };

    // Try to add a preferred student to a team
    auto tryToAddPreferredStudent = [&](const Student& preferrer, vector<Student>& team) -> bool {
        for (const auto& pref : preferrer.want_to_work_with) {
            if (assigned_students.find(pref) == assigned_students.end() && student_map.find(pref) != student_map.end()) {
                Student preferred_student = student_map[pref];
                if (canWorkTogether(team, preferred_student)) {
                    assignStudentToTeam(preferred_student, team);
                    return true;
                }
            }
        }
        return false;
    };

    // Fill the team with random students if needed
    auto fillTeamWithRandomStudents = [&](vector<Student>& team) {
        while (team.size() < static_cast<size_t>(team_size) && !remaining_students.empty()) {
            auto random_it = remaining_students.begin();
            while (random_it != remaining_students.end() && !canWorkTogether(team, *random_it)) {
                ++random_it;
            }
            if (random_it != remaining_students.end()) {
                assignStudentToTeam(*random_it, team);
            } else {
                break;
            }
        }
    };

    // Find a student with preferences
    auto findStudentWithPreferences = [&]() -> vector<Student>::iterator {
        return find_if(remaining_students.begin(), remaining_students.end(), [](const Student& s) {
            return !s.want_to_work_with.empty();
        });
    };

    // Form teams iteratively until successful
    bool successful_formation = false;
    while (!successful_formation) {
        assigned_students.clear();
        remaining_students = students;
        for (auto& team : teams) {
            team.clear();
        }

        // Keep track of already selected team leaders in this formation attempt
        set<string> used_team_leaders;

        for (auto& team : teams) {
            if (remaining_students.empty()) break;

            // Select team leader
            Student student1;
            bool leader_selected = false;

            while (!leader_selected) {
                if (!team_leaders.empty() && leader_index < team_leaders.size()) {
                    student1 = team_leaders[leader_index];
                    leader_index++;
                } else {
                    auto it = findStudentWithPreferences();
                    if (it == remaining_students.end()) {
                        it = remaining_students.begin();
                    }
                    student1 = *it;
                    team_leaders.push_back(student1);
                    leader_index = 1; // Reset leader_index to start rotating through new leaders on next restart
                }

                if (used_team_leaders.find(student1.username) == used_team_leaders.end()) {
                    leader_selected = true;
                    used_team_leaders.insert(student1.username);
                }
            }

            assignStudentToTeam(student1, team);

            // Debug: output the student assigned
            cout << "Assigned student " << student1.username << " to a team." << endl;

            // Try to assign preferred teammates
            if (!tryToAddPreferredStudent(student1, team)) {
                // If no preferred teammates can be assigned, fill with random students
                fillTeamWithRandomStudents(team);
            } else {
                // If a preferred teammate was assigned, try to assign their preferred teammates
                if (team.size() < static_cast<size_t>(team_size)) {
                    Student student2 = team.back();
                    if (!tryToAddPreferredStudent(student2, team)) {
                        // If no preferred teammates can be assigned, fill with another preference of student1 or random students
                        tryToAddPreferredStudent(student1, team);
                        fillTeamWithRandomStudents(team);
                    }
                }
            }
        }

        // Check if all teams are of the same size
        successful_formation = true;
        for (const auto& team : teams) {
            if (team.size() != static_cast<size_t>(team_size)) {
                cerr << "Unequal team sizes detected. Restarting team formation." << endl;
                successful_formation = false;
                break;
            }
        }

        // If the last team has conflicting students, restart the process
        if (successful_formation) {
            for (const auto& member1 : teams.back()) {
                for (const auto& member2 : teams.back()) {
                    if (find(member1.dont_want_to_work_with.begin(), member1.dont_want_to_work_with.end(), member2.username) != member1.dont_want_to_work_with.end() ||
                        find(member2.dont_want_to_work_with.begin(), member2.dont_want_to_work_with.end(), member1.username) != member2.dont_want_to_work_with.end()) {
                        cerr << "Conflicting team members found. Restarting team formation." << endl;
                        successful_formation = false;
                        break;
                    }
                }
                if (!successful_formation) break;
            }
        }

        // Rotate team leaders if unsuccessful formation
        if (!successful_formation) {
            rotate(team_leaders.rbegin(), team_leaders.rbegin() + 1, team_leaders.rend());
            leader_index = 0;
        }
    }

    // Distribute any remaining students evenly among teams
    distributeRemainingStudents(remaining_students, assigned_students);
}

// Form teams by balancing skills
void TeamBuilder::formTeamsBySkills() {
    set<string> assigned_students;
    vector<Student> remaining_students = students;

    // Calculate the number of teams needed
    int num_teams = ceil(static_cast<double>(students.size()) / team_size);
    teams.resize(num_teams);

    // Sort students based on their total skill level
    sort(remaining_students.begin(), remaining_students.end(), [](const Student& a, const Student& b) {
        int total_skill_a = a.programming_skill + a.debugging_skill + a.algorithm_skill;
        int total_skill_b = b.programming_skill + b.debugging_skill + b.algorithm_skill;
        return total_skill_a > total_skill_b;
    });

    // Distribute students among teams to balance skills
    for (int skill_type = 0; skill_type < 3; ++skill_type) {
        size_t index = 0;
        while (!remaining_students.empty()) {
            for (auto& team : teams) {
                if (team.size() < static_cast<size_t>(team_size)) {
                    auto it = remaining_students.begin() + (index % remaining_students.size());
                    if (assigned_students.find(it->username) == assigned_students.end() && canWorkTogether(team, *it)) {
                        team.push_back(*it);
                        assigned_students.insert(it->username);
                        remaining_students.erase(it);
                        break;
                    }
                }
            }
            index++;
        }
    }
}

// Distribute remaining students among teams
void TeamBuilder::distributeRemainingStudents(vector<Student>& remaining_students, set<string>& assigned_students) {
    while (!remaining_students.empty()) {
        bool student_added = false;
        for (auto& team : teams) {
            if (team.size() < static_cast<size_t>(team_size)) {
                for (auto it = remaining_students.begin(); it != remaining_students.end(); ++it) {
                    if (assigned_students.find(it->username) == assigned_students.end() && canWorkTogether(team, *it)) {
                        team.push_back(*it);
                        assigned_students.insert(it->username);
                        remaining_students.erase(it);
                        student_added = true;
                        break;
                    }
                }
            }
            if (student_added) {
                break;
            }
        }
        if (!student_added) {
            break;
        }
    }
}
// Calculate scores for each team
void TeamBuilder::calculateTeamScores() {
    team_scores.clear();
    for (const auto& team : teams) {
        int programming_score = 0;
        int debugging_score = 0;
        int algorithm_score = 0;
        for (const auto& member : team) {
            programming_score += member.programming_skill;
            debugging_score += member.debugging_skill;
            algorithm_score += member.algorithm_skill;
        }
        team_scores.push_back({programming_score, debugging_score, algorithm_score});
    }
    // Set the minimum scores for each team size
    if (prioritize_skills) {
        for (auto& scores : team_scores) {
            if (team_size == 3) {
                if (scores[0] < 5) scores[0] = 5;
                if (scores[1] < 5) scores[1] = 5;
                if (scores[2] < 5) scores[2] = 5;
                if (scores[0] + scores[1] + scores[2] < 16) {
                    int diff = 16 - (scores[0] + scores[1] + scores[2]);
                    scores[0] += diff / 3;
                    scores[1] += diff / 3;
                    scores[2] += diff - 2 * (diff / 3);
                }
            } else if (team_size == 4) {
                if (scores[0] < 7) scores[0] = 7;
                if (scores[1] < 7) scores[1] = 7;
                if (scores[2] < 7) scores[2] = 7;
                if (scores[0] + scores[1] + scores[2] < 22) {
                    int diff = 22 - (scores[0] + scores[1] + scores[2]);
                    scores[0] += diff / 3;
                    scores[1] += diff / 3;
                    scores[2] += diff - 2 * (diff / 3);
                }
            }
        }
    }

    // Sort teams based on the sum of their scores
    vector<size_t> indices(teams.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        int total_score_a = team_scores[a][0] + team_scores[a][1] + team_scores[a][2];
        int total_score_b = team_scores[b][0] + team_scores[b][1] + team_scores[b][2];
        return total_score_a > total_score_b;
    });

    vector<vector<Student>> sorted_teams;
    for (size_t index : indices) {
        sorted_teams.push_back(teams[index]);
    }
    teams = move(sorted_teams);
}


// Print teams and their scores
void TeamBuilder::printTeamsAndScores() {
    for (size_t i = 0; i < teams.size(); ++i) {
        cout << "Team " << i + 1 << ":\n";
        for (const auto& member : teams[i]) {
            cout << member.username << " ";
        }
        cout << "\nProgramming skill score: " << team_scores[i][0]
             << ", Debugging skill score: " << team_scores[i][1]
             << ", Algorithm skill score: " << team_scores[i][2] << "\n\n";
    }
}

// Write teams to a CSV file
void TeamBuilder::writeTeamsToFile(const string& filename) {
    ofstream outfile(filename);
    if (outfile.is_open()) {
        for (size_t i = 0; i < teams.size(); ++i) {
            outfile << "Team " << i + 1 << ",";
            for (const auto& member : teams[i]) {
                outfile << member.username << ",";
            }
            outfile << "\n";
        }
        outfile.close();
        cout << "Teams written to file: " << filename << endl;
    } else {
        cerr << "Failed to open file for writing: " << filename << endl;
    }
}

#ifndef TEAMBUILDER_HPP
#define TEAMBUILDER_HPP

#include <vector>
#include <string>
#include <map>
#include <set>

struct Student {
    std::string username;
    int programming_skill;
    int debugging_skill;
    int algorithm_skill;
    std::vector<std::string> dont_want_to_work_with;
    std::vector<std::string> want_to_work_with;
};

class TeamBuilder {
public:
    TeamBuilder(const std::vector<Student>& students, int team_size, bool prioritize_skills);
    void formTeams(bool prioritize_preferences);
    void printTeamsAndScores();
    void writeTeamsToFile(const std::string& filename);

private:
    std::vector<Student> students;
    int team_size;
    bool prioritize_skills;  // Add this member variable
    std::vector<std::vector<Student>> teams;
    std::vector<std::vector<int>> team_scores;
    std::map<std::string, Student> student_map;

    void loadStudentMap();
    bool canWorkTogether(const std::vector<Student>& team, const Student& new_member);
    void formTeamsByPreferences();
    void formTeamsBySkills();
    void distributeRemainingStudents(std::vector<Student>& remaining_students, std::set<std::string>& assigned_students);
    void assignMinimumPreferences(std::vector<Student>& remaining_students, std::set<std::string>& assigned_students);
    void calculateTeamScores();
};

#endif // TEAMBUILDER_HPP




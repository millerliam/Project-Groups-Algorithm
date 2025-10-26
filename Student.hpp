#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <vector>
#include <string>

// Define the Student struct to store student data
struct Student {
    std::string username;
    int programming_skill;
    int debugging_skill;
    int algorithm_skill;
    std::vector<std::string> dont_want_to_work_with;
    std::vector<std::string> want_to_work_with;
};

#endif // STUDENT_HPP


#ifndef PARSER_H
#define PARSER_H

#include "policy.h"
#include "process.h"
#include "scheduler.h"  
#include <string>
#include <vector>

struct LevelsConfig {
    std::vector<Policy> policies;
    std::vector<int> quanta;
};

class Parser {
public:
    static LevelsConfig parseLevelsString(const std::string &s);
    static std::vector<Process> readInput(const std::string &path);
    static void writeOutput(const std::string &path,
                            const std::vector<Process*>& procs,
                            const std::vector<ExecSlice>& timeline,
                            const Summary& sm);
};

#endif 

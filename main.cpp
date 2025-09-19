#include "parser.h"
#include "scheduler.h"
#include <iostream>
#include <vector>
#include <string>

static void printUsage() {
    std::cerr << "Usage:\n"
              << "  mlfq --levels \"RR(1),RR(3),RR(4),SJF\" --input in.txt --output out.txt [--boost K]\n";
}

int main(int argc, char** argv) {
    std::string levelsStr;
    std::string inPath;
    std::string outPath = "out.txt";
    int boost = -1;

    int i = 1;
    while (i < argc) {
        std::string a = argv[i];
        if (a == "--levels") {
            if (i + 1 < argc) {
                levelsStr = argv[i + 1];
                i = i + 2;
            } else {
                printUsage();
                return 1;
            }
        } else if (a == "--input") {
            if (i + 1 < argc) {
                inPath = argv[i + 1];
                i = i + 2;
            } else {
                printUsage();
                return 1;
            }
        } else if (a == "--output") {
            if (i + 1 < argc) {
                outPath = argv[i + 1];
                i = i + 2;
            } else {
                printUsage();
                return 1;
            }
        } else if (a == "--boost") {
            if (i + 1 < argc) {
                boost = std::stoi(argv[i + 1]);
                i = i + 2;
            } else {
                printUsage();
                return 1;
            }
        } else {
            i = i + 1;
        }
    }

    if (levelsStr.empty() || inPath.empty()) {
        printUsage();
        return 1;
    }

    LevelsConfig cfg = Parser::parseLevelsString(levelsStr);
    std::vector<QueueLevel> levels;
    int j = 0;
    int n = static_cast<int>(cfg.policies.size());
    while (j < n) {
        levels.emplace_back(cfg.policies[j], cfg.quanta[j]);
        j = j + 1;
    }

    MlfqScheduler sched(levels, boost);
    auto procs = Parser::readInput(inPath);
    sched.loadProcesses(procs);
    sched.run();
    auto summary = sched.computeSummary();
    Parser::writeOutput(outPath, sched.processes(), sched.timeline(), summary);

    std::cout << "Simulation complete. Output written to: " << outPath << "\n";
    return 0;
}

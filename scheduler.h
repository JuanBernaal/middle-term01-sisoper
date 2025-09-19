#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queueLevel.h"
#include <vector>
#include <string>

struct ExecSlice {
    std::string label;
    int start{0};
    int end{0};
};

struct Summary {
    double avgWt{0.0};
    double avgCt{0.0};
    double avgRt{0.0};
    double avgTat{0.0};
};

class MlfqScheduler {
public:
    MlfqScheduler(std::vector<QueueLevel> levels, int boostInterval = -1);

    void loadProcesses(const std::vector<Process>& plist);
    void run();

    const std::vector<ExecSlice>& timeline() const { return timeline_; }
    Summary computeSummary() const;

    const std::vector<Process*>& processes() const { return procsPtr_; }

private:
    std::vector<QueueLevel> levels_;
    std::vector<Process> procs_;      
    std::vector<Process*> procsPtr_;   
    int time_{0};
    int boostInterval_{-1};
    int nextBoost_{-1};
    std::vector<ExecSlice> timeline_;

    void admitArrivals();    
    void globalBoost();      
    int highestNonEmptyLevel() const;
    int determineRunFor(Process* p, QueueLevel& q) const;
};

#endif 

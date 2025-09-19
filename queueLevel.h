
#ifndef QUEUELEVEL_H
#define QUEUELEVEL_H

#include "policy.h"
#include "process.h"
#include <deque>
#include <vector>

class QueueLevel {
public:
    QueueLevel() = default;
    QueueLevel(Policy p, int q) : policy_(p), quantum_(q) {}

    bool empty() const { return ready_.empty(); }
    int size() const { return static_cast<int>(ready_.size()); }

    void push(Process* p) { ready_.push_back(p); }

    Process* pop();

    Policy policy() const { return policy_; }
    int quantum() const { return quantum_; }

    void sortByRemaining(); 
    void sortByBurst();    

private:
    Policy policy_{Policy::Rr};
    int quantum_{1};
    std::deque<Process*> ready_;
};

#endif

#include "queueLevel.h"
#include <algorithm>

void QueueLevel::sortByRemaining() {
    std::vector<Process*> v;
    int n = static_cast<int>(ready_.size());
    int i = 0;
    while (i < n) {
        v.push_back(ready_.front());
        ready_.pop_front();
        i = i + 1;
    }
    std::sort(v.begin(), v.end(), [](Process* a, Process* b) {
        return a->getRemaining() < b->getRemaining();
    });
    int j = 0;
    while (j < static_cast<int>(v.size())) {
        ready_.push_back(v[j]);
        j = j + 1;
    }
}

void QueueLevel::sortByBurst() {
    std::vector<Process*> v;
    int n = static_cast<int>(ready_.size());
    int i = 0;
    while (i < n) {
        v.push_back(ready_.front());
        ready_.pop_front();
        i = i + 1;
    }
    std::sort(v.begin(), v.end(), [](Process* a, Process* b) {
        return a->getBurstTime() < b->getBurstTime();
    });
    int j = 0;
    while (j < static_cast<int>(v.size())) {
        ready_.push_back(v[j]);
        j = j + 1;
    }
}

Process* QueueLevel::pop() {
    if (policy_ == Policy::Sjf) {
        sortByBurst();
    } else if (policy_ == Policy::Stcf) {
        sortByRemaining();
    }
    if (ready_.empty()) {
        return nullptr;
    }
    Process* p = ready_.front();
    ready_.pop_front();
    return p;
}

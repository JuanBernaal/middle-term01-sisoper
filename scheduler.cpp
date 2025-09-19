#include "scheduler.h"
#include <algorithm>

MlfqScheduler::MlfqScheduler(std::vector<QueueLevel> levels, int boostInterval)
    : levels_(std::move(levels)), boostInterval_(boostInterval) {
    if (boostInterval_ > 0) {
        nextBoost_ = 0;
    } else {
        nextBoost_ = -1;
    }
}

void MlfqScheduler::loadProcesses(const std::vector<Process>& plist) {
    procs_ = plist;
    procsPtr_.clear();
    int i = 0;
    int n = static_cast<int>(procs_.size());
    while (i < n) {
        procs_[i].setAdmitted(false);
        procs_[i].setCurrentLevel(0);
        procsPtr_.push_back(&procs_[i]);
        i = i + 1;
    }
    std::sort(procsPtr_.begin(), procsPtr_.end(), [](Process* a, Process* b) {
        if (a->getArrivalTime() != b->getArrivalTime()) {
            return a->getArrivalTime() < b->getArrivalTime();
        }
        return a->getLabel() < b->getLabel();
    });
}

void MlfqScheduler::admitArrivals() {
    int i = 0;
    int n = static_cast<int>(procs_.size());
    while (i < n) {
        Process* p = &procs_[i];
        if (!p->isAdmitted()) {
            if (p->getArrivalTime() <= time_) {
                p->setCurrentLevel(0);
                levels_[0].push(p);
                p->setAdmitted(true);
            }
        }
        i = i + 1;
    }
}

int MlfqScheduler::highestNonEmptyLevel() const {
    int i = 0;
    int n = static_cast<int>(levels_.size());
    while (i < n) {
        if (!levels_[i].empty()) return i;
        i = i + 1;
    }
    return -1;
}

int MlfqScheduler::determineRunFor(Process* p, QueueLevel& q) const {
    Policy pol = q.policy();
    if (pol == Policy::Sjf || pol == Policy::Fifo) {
        return p->getRemaining(); 
    }
    if (pol == Policy::Stcf) {
        return 1; 
    }
    int qn = q.quantum();
    int rem = p->getRemaining();
    if (rem < qn) return rem;
    return qn;
}

void MlfqScheduler::globalBoost() {
    if (boostInterval_ <= 0) return;
    if (time_ < nextBoost_) return;

    int lvl = 1;
    int n = static_cast<int>(levels_.size());
    while (lvl < n) {
        std::vector<Process*> toMove;
        int sz = levels_[lvl].size();
        int k = 0;
        while (k < sz) {
            Process* p = levels_[lvl].pop();
            if (p != nullptr) {
                p->setCurrentLevel(0);
                toMove.push_back(p);
            }
            k = k + 1;
        }
        int j = 0;
        int m = static_cast<int>(toMove.size());
        while (j < m) {
            levels_[0].push(toMove[j]);
            j = j + 1;
        }
        lvl = lvl + 1;
    }
    nextBoost_ = time_ + boostInterval_;
}

void MlfqScheduler::run() {
    int i = 0;
    int n = static_cast<int>(procs_.size());
    while (i < n) {
        procs_[i].setAdmitted(false);
        procs_[i].setCurrentLevel(0);
        procs_[i].setFirstStart(-1);
        procs_[i].setCompletion(-1);
        procs_[i].setResponse(-1);
        procs_[i].setWaiting(0);
        procs_[i].setTurnAround(0);
        i = i + 1;
    }

    timeline_.clear();
    time_ = 0;

    bool anyRemaining = n > 0;
    while (anyRemaining) {
        admitArrivals();
        globalBoost();

        int lvl = highestNonEmptyLevel();
        if (lvl == -1) {
            int minAt = -1;
            int j = 0;
            while (j < n) {
                if (!procs_[j].isAdmitted()) {
                    int at = procs_[j].getArrivalTime();
                    if (minAt < 0 || at < minAt) {
                        minAt = at;
                    }
                }
                j = j + 1;
            }
            if (minAt >= 0 && time_ < minAt) {
                time_ = minAt;
            } else {
                anyRemaining = false;
            }
        } else {
            Process* p = levels_[lvl].pop();
            if (p != nullptr) {
                if (p->getFirstStart() < 0) {
                    p->setFirstStart(time_);
                    int rt = time_ - p->getArrivalTime();
                    p->setResponse(rt);
                }

                int runFor = determineRunFor(p, levels_[lvl]);

                ExecSlice rec{p->getLabel(), time_, time_ + runFor};
                timeline_.push_back(rec);

                p->consume(runFor);
                time_ = time_ + runFor;

                admitArrivals(); 
                globalBoost();

                if (p->finished()) {
                    p->setCompletion(time_);
                } else {
                    if (levels_[lvl].policy() == Policy::Rr) {
                        if (runFor == levels_[lvl].quantum()) {
                            int newLvl = lvl;
                            if (lvl + 1 < static_cast<int>(levels_.size())) {
                                newLvl = lvl + 1;
                            }
                            p->setCurrentLevel(newLvl);
                            levels_[newLvl].push(p);
                        } else {
                            p->setCurrentLevel(lvl);
                            levels_[lvl].push(p);
                        }
                    } else {
                        p->setCurrentLevel(lvl);
                        levels_[lvl].push(p);
                    }
                }
            }
        }

        bool stillHas = false;
        int t = 0;
        while (t < n) {
            if (procs_[t].getCompletion() < 0) {
                stillHas = true;
            }
            t = t + 1;
        }
        anyRemaining = stillHas;
    }

    int k = 0;
    while (k < n) {
        if (procs_[k].getCompletion() >= 0) {
            int tat = procs_[k].getCompletion() - procs_[k].getArrivalTime();
            procs_[k].setTurnAround(tat);
            int wt = tat - procs_[k].getBurstTime();
            procs_[k].setWaiting(wt);
        }
        k = k + 1;
    }
}

Summary MlfqScheduler::computeSummary() const {
    Summary s{};
    int n = static_cast<int>(procs_.size());
    if (n == 0) {
        return s;
    }
    double sumWt = 0.0;
    double sumCt = 0.0;
    double sumRt = 0.0;
    double sumTat = 0.0;

    int i = 0;
    while (i < n) {
        const Process& p = procs_[i];
        sumWt += p.getWaiting();
        sumCt += p.getCompletion();
        sumRt += p.getResponse();
        sumTat += p.getTurnAround();
        i = i + 1;
    }
    s.avgWt = sumWt / n;
    s.avgCt = sumCt / n;
    s.avgRt = sumRt / n;
    s.avgTat = sumTat / n;
    return s;
}

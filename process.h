#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
public:
    Process() = default;

    Process(std::string l, int bt, int at, int pr, int inputQ = -1)
        : label(std::move(l)),
          burstTime(bt),
          arrivalTime(at),
          priority(pr),
          inputQueue(inputQ),
          remaining(bt),
          firstStart(-1),
          completion(-1),
          response(-1),
          waiting(0),
          turnAround(0),
          admitted(false),
          currentLevel(0) {}
  
    const std::string& getLabel() const { return label; }
    int getBurstTime() const { return burstTime; }
    int getArrivalTime() const { return arrivalTime; }
    int getPriority() const { return priority; }
    int getInputQueue() const { return inputQueue; }

    int getRemaining() const { return remaining; }
    int getFirstStart() const { return firstStart; }
    int getCompletion() const { return completion; }
    int getResponse() const { return response; }
    int getWaiting() const { return waiting; }
    int getTurnAround() const { return turnAround; }
    bool isAdmitted() const { return admitted; }
    int getCurrentLevel() const { return currentLevel; }

    void setAdmitted(bool v) { admitted = v; }
    void setCurrentLevel(int lvl) { currentLevel = lvl; }
    void setFirstStart(int t) { firstStart = t; }
    void setCompletion(int t) { completion = t; }
    void setResponse(int t) { response = t; }
    void setWaiting(int w) { waiting = w; }
    void setTurnAround(int t) { turnAround = t; }

    void consume(int t) { remaining -= t; }
    bool finished() const { return remaining <= 0; }

private:
    std::string label;
    int burstTime{0};
    int arrivalTime{0};
    int priority{3};
    int inputQueue{-1}; 

    int remaining{0};
    int firstStart{-1};
    int completion{-1};
    int response{-1};
    int waiting{0};
    int turnAround{0};

    bool admitted{false};
    int currentLevel{0};
};

#endif 

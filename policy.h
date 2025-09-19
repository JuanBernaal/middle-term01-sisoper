#ifndef POLICY_H
#define POLICY_H

#include <string>

enum class Policy { Rr, Sjf, Stcf, Fifo };

inline Policy parsePolicy(const std::string &name) {
    if (name == "RR") return Policy::Rr;
    if (name == "SJF") return Policy::Sjf;
    if (name == "STCF") return Policy::Stcf;
    if (name == "FIFO") return Policy::Fifo;
    return Policy::Rr; 
}

#endif 

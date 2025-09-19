#include "parser.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cctype>

namespace {
    std::string trim(const std::string &s) {
        size_t i = 0;
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
            i = i + 1;
        }
        size_t j = s.size();
        while (j > i && std::isspace(static_cast<unsigned char>(s[j - 1]))) {
            j = j - 1;
        }
        return s.substr(i, j - i);
    }
}

LevelsConfig Parser::parseLevelsString(const std::string &s) {
    LevelsConfig cfg{};
    std::stringstream ss(s);
    std::string token;
    bool more = static_cast<bool>(std::getline(ss, token, ','));
    while (more) {
        std::string t = trim(token);
        if (t.rfind("RR(", 0) == 0 && t.size() >= 5 && t.back() == ')') {
            std::string inside = t.substr(3, t.size() - 4);
            int q = std::stoi(inside);
            cfg.policies.push_back(Policy::Rr);
            cfg.quanta.push_back(q);
        } else if (t == "SJF") {
            cfg.policies.push_back(Policy::Sjf);
            cfg.quanta.push_back(0);
        } else if (t == "STCF") {
            cfg.policies.push_back(Policy::Stcf);
            cfg.quanta.push_back(0);
        } else if (t == "FIFO") {
            cfg.policies.push_back(Policy::Fifo);
            cfg.quanta.push_back(0);
        }
        more = static_cast<bool>(std::getline(ss, token, ','));
    }
    return cfg;
}

std::vector<Process> Parser::readInput(const std::string &path) {
    std::ifstream in(path);
    std::vector<Process> procs;

    if (!in.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo de entrada: " << path << "\n";
        return procs;
    }

    std::string line;
    bool more = static_cast<bool>(std::getline(in, line));
    while (more) {
        std::string t = trim(line);
        if (!t.empty() && t[0] != '#') {
            bool onlyDashes = true;
            size_t k = 0;
            while (k < t.size()) {
                if (t[k] != '-') {
                    onlyDashes = false;
                }
                k = k + 1;
            }
            if (!onlyDashes && t.find(';') != std::string::npos) {
                std::stringstream ls(t);
                std::string label, bt, at, q, pr;
                bool a = static_cast<bool>(std::getline(ls, label, ';'));
                bool b = false, c = false;
                if (a) b = static_cast<bool>(std::getline(ls, bt, ';'));
                if (b) c = static_cast<bool>(std::getline(ls, at, ';'));

                int qVal = -1;
                int prVal = 3;

                if (c) {
                    std::string qOrPr;
                    bool d = static_cast<bool>(std::getline(ls, qOrPr, ';'));
                    if (d) {
                        std::string prMaybe;
                        bool e = static_cast<bool>(std::getline(ls, prMaybe, ';'));
                        if (e) {
                            pr = trim(prMaybe);
                            q = trim(qOrPr);
                        } else {
                            pr = trim(qOrPr);
                        }
                    }
                }

                if (!q.empty()) {
                    std::string tq = trim(q);
                    if (!tq.empty() && tq != "-") qVal = std::stoi(tq);
                }
                if (!pr.empty()) {
                    std::string tp = trim(pr);
                    if (!tp.empty()) prVal = std::stoi(tp);
                }

                std::string tl = trim(label);
                if (!tl.empty()) {
                    int burst = std::stoi(trim(bt));
                    int atime = std::stoi(trim(at));
                    Process p(tl, burst, atime, prVal, qVal); 
                    procs.push_back(p);
                }
            }
        }
        more = static_cast<bool>(std::getline(in, line));
    }
    return procs;
}

void Parser::writeOutput(const std::string &path,
                         const std::vector<Process*>& procs,
                         const std::vector<ExecSlice>& 
                         const Summary& sm) {
    std::ofstream out(path);
    out << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";

    int i = 0;
    int n = static_cast<int>(procs.size());
    while (i < n) {
        const Process* p = procs[i];
        out << p->getLabel() << ';'
            << p->getBurstTime() << ';'
            << p->getArrivalTime() << ';'
            << (p->getInputQueue() >= 0 ? std::to_string(p->getInputQueue()) : std::string("-")) << ';'
            << p->getPriority() << ';'
            << p->getWaiting() << ';'
            << p->getCompletion() << ';'
            << p->getResponse() << ';'
            << p->getTurnAround() << '\n';
        i = i + 1;
    }

    out.setf(std::ios::fixed);
    out << std::setprecision(1);
    out << "WT=" << sm.avgWt
        << "; CT=" << sm.avgCt
        << "; RT=" << sm.avgRt
        << "; TAT=" << sm.avgTat
        << ";\n";
}

#ifndef GEORG_SRC_READER_HPP_
#define GEORG_SRC_READER_HPP_

#include "main.hpp"
#include "GeorgEngine.hpp"
#include "GeorgInterface.hpp"


struct Reader{
    std::map<std::string, int> varnames;
    std::vector<std::string> namevars;
    std::map<std::string, std::string> parsedFuncs;

    int readcmd(std::string cmd, decentEngine &D);

    int regenState(std::string state, std::vector<std::string> &cmdsout, Interface::drawList &drawer, Interface::dispList &display, State &S);

    int createExpr(std::string exprtype, std::string polystr, expression &C);//use this to subsittute in the user defined functions 
};

#endif // GEORG_SRC_READER_HPP_
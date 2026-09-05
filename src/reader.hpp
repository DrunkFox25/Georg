#pragma once

#include "main.hpp"
#include "GeorgEngine.hpp"
#include "GeorgInterface.hpp"


struct Reader{
    map<string, int> varnames;
    vector<string> namevars;
    map<string, string> parsedFuncs;

    int readcmd(string cmd, decentEngine &D);

    int regenState(string state, vector<string> &cmdsout, Interface::drawList &drawer, Interface::dispList &display, State &S);

    int createExpr(string exprtype, string polystr, expression &C);//maybe at some point don't expand functions, just leave them as is, like in polystack
};
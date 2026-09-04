#pragma once

#include "main.hpp"
#include "GeorgEngine.hpp"


struct Reader{
    map<string, int> varnames;
    vector<string> namevars;
    map<string, string> parsedFuncs;

    struct drawcmd{
        int type;
        vector<expression> vars;
        QColor color = QColor();//invalid by default
        int width = -1;
        int style = -1;//Qt::PenStyle would use the enum but no invalid value
        //enum {label_none, label_string, label_exp} labeltype = label_none;
        expression labelexp;
        string labelstr;
    };

    struct disp{
        int type;
        vector<string> name;
        vector<int> vars;
        vector<bool> modify;
        vector<pair<double, double>> ranges;
        rect<double> crange;
    };

    int readcmd(string cmd, decentEngine &D);

    int regenState(string state, vector<string> &cmdsout, vector<Reader::drawcmd> &drawer, vector<Reader::disp> &display, State &S);

    int createExpr(string exprtype, string polystr, expression &C);//maybe at some point don't expand functions, just leave them as is, like in polystack
};
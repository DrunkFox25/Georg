#include "expression.hpp"


void expression::update(const std::vector<cplx> &in, cplx &value, std::vector<cplx> &dir){
    std::vector<cplx> val(numvars);
    for(int i = 0; i < numvars; i++) val[i] = in[opnums[i]];
    evalValAndDir(val, value, dir);
}

cplx expression::eval(const std::vector<cplx> &in){
    std::vector<cplx> val(numvars);
    for(int i = 0; i < numvars; i++) val[i] = in[opnums[i]];
    return poly<cplx>::eval(val);
}

expression::expression(){}
expression::expression(int val) P({val, {}}), numvars(0){}
expression::expression(double val, P({val, {}}), numvars(0) {}
expression::expression(cplx val) P({val, {}}), numvars(0) {}
expression::expression(cplx coeff, int vari) P({val, {1}}), numvars(1), opnums({vari}) {}


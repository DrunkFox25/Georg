#pragma once

#include "util.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

struct expression : public poly<cplx>{//this is temporary for now
	std::vector<int> opnums;////must be in increasing order or it will break Eigen
	void update(const std::vector<cplx> &in, cplx &value, std::vector<cplx> &dir){
		std::vector<cplx> val(numvars);
		for(int i = 0; i < numvars; i++) val[i] = in[opnums[i]];
		evalValAndDir(val, value, dir);
	}

	cplx eval(const std::vector<cplx> &in){
		std::vector<cplx> val(numvars);
		for(int i = 0; i < numvars; i++) val[i] = in[opnums[i]];
		return poly<cplx>::eval(val);
	}

	expression(){}

	expression(int val){P.push_back({val, {}});}
	expression(double val){P.push_back({val, {}});}
	expression(cplx val){P.push_back({val, {}});}
};

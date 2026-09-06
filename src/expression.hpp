#pragma once

#include "util.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

struct expression : public poly<cplx>{//this is temporary for now
	//make it support seperate mode where it is a string
	//make it support general expressions
	//also something maybe like varstack in reader.cpp
	std::vector<int> opnums;////must be in increasing order or it will break Eigen
	void update(const std::vector<cplx> &in, cplx &value, std::vector<cplx> &dir);
	cplx eval(const std::vector<cplx> &in);

	expression();
	
	expression(int val);
	expression(double val);
	expression(cplx val);

	expression(cplx coeff, int vari);
};

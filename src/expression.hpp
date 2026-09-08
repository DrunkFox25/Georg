#ifndef GEORG_SRC_EXPRESSION_HPP_
#define GEORG_SRC_EXPRESSION_HPP_

#include "util.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>



struct expression{
	//todo: optimize the living shit out of this

	void set(ASTTree &tree/*change this to whatever the lib that shall not be named outputs */, int optimize_flags);

	cplx eval(std::vector<cplx> &in);

	void evalDir(std::vector<cplx> &in, cplx &out, std::vector<cplx> dir){}//maybe make this only one of the dirivitives, but have the tree with eval set up
};

#endif // GEORG_SRC_EXPRESSION_HPP_
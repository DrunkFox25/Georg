

#include "expression.hpp"

//this code should be moved out:

/*
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
};*/

struct constraint : public expression{
	std::vector<int> opnums;

    constraint();
    constraint(cplx coeff);
    constraint(cplx coeff, int varnum);
};


struct dynamicString : public expression{//like the printf function but a string, for purposes of labeling ig
	std::vector<int> opnums;

    dynamicString();
    dynamicString(QJsonValue v);
};
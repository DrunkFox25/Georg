#include "GeorgEngine.hpp"

using namespace std;

/*
testing:
https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,source:'//+Type+your+code+here,+or+load+an+example.%0Aint+square(int+num)+%7B%0A++++return+num+*+num%3B%0A%7D'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:50,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:executor,i:(argsPanelShown:'0',compilationPanelShown:'0',compiler:g162,compilerName:'',compilerOutShown:'0',execArgs:'',execStdin:'',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'',overrides:!(),runtimeTools:!((name:heaptrack,options:!((name:graph,value:yes)))),source:1,stdinPanelShown:'0',wrap:'1'),l:'5',n:'0',o:'Executor+x86-64+gcc+16.2+(C%2B%2B,+Editor+%231)',t:'0')),k:50,l:'4',n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4
*/









decentEngine::decentEngine(State &s) : S(s){
	gen.seed(rd());
}

double decentEngine::update(){
	S.updateVals();

	double maxdist = 0;

	int cnt = 0;
	for(int coni = 0; coni < S.n; coni++){
		b_vals[coni] = -S.constrVal[coni];

		maxdist = max(maxdist, abs(S.constrVal[coni]));

		for(int i = 0; i < S.constrOpNums[coni].size(); i++){
			if(!S.fixed[S.constrOpNums[coni][i]]) nzval[cnt++] = S.constrDir[coni][i];
		}
	}

	return maxdist;
}

void decentEngine::regen(){
	renamed.resize(S.numvars, -1); int unfixed = 0;
	for(int i = 0; i < S.numvars; i++){
		if(!S.fixed[i]) renamed[i] = unfixed++;
	}

	b_vals.resize(S.n);
	x_vals.resize(S.n);

	int nnz = 0;
	rowptr.resize(S.n+1); rowptr[0] = 0;
	for(int coni = 0; coni < S.n; coni++){
		for(auto &opnum : S.constrOpNums[coni]){
			if(renamed[opnum] != -1){
				colind.push_back(renamed[opnum]);
				nnz++;
			}
		}
		rowptr[coni+1] = nnz;
	}

	nzval.resize(nnz);

	A_Trans = make_unique<Eigen::Map<Eigen::SparseMatrix<cplx, Eigen::ColMajor>>>(S.n, S.n, nnz, rowptr.data(), colind.data(), nzval.data());

	solver.setPivotThreshold(1.0);
	solver.analyzePattern(*A_Trans);
}

void decentEngine::addNoise(uniform_real_distribution<double> db){
	for(int i = 0; i < S.numvars; i++){
		if(!S.fixed[i]) S.vars[i] += cplx(db(gen), db(gen));
	}

	return;
}

void decentEngine::addNoise(double db){
	return addNoise(uniform_real_distribution<double>(-db, db));
}

int decentEngine::descend(){
	solver.factorize(*A_Trans);
	if(solver.info() != Eigen::Success){
		cerr << "Solver failed: " << solver.info() << "\n" << flush;
		cerr << "n: " << S.n << "\ncolind: " << colind << "\nrowptr: " << rowptr << "\nnzval: " << nzval << "\n" << flush;
		return solver.info();
	}
	
	Eigen::Map<Eigen::VectorXcd>(x_vals.data(), x_vals.size()) = solver.transpose().solve(Eigen::Map<Eigen::VectorXcd>(b_vals.data(), b_vals.size()));

	for(int i = 0; i < S.numvars; i++){
		if(renamed[i] != -1) S.vars[i] += x_vals[renamed[i]];
	}

	for(int i = 0; i < S.numvars; i++){
		if(abs(S.vars[i].imag()) < 1e-30) S.vars[i].imag(0);
	}

	return Eigen::Success;
}






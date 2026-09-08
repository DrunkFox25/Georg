#include "expression.hpp"







//old code to use:

struct poly{
	int numvars = 0;
	std::vector<std::pair<T, std::vector<int>>> P;

	poly(){}

	poly(int n) : numvars(n){}

	
	cplx eval(const std::vector &in);

	
	cplx dir(const std::vector &in, const int resp);

	
	void evalValAndDir(const std::vector &in, cplx &val, std::vector &dir);// need to make this better

	
	cplx operator()(const std::vector &in){return eval(in);}

	void add_term(cplx coeff, const std::vector<int>& terms);
};

struct orderedPoly : poly{
	using poly::poly;

	void compress();
};


poly operator+(const poly& lhs, const poly& rhs);


poly& operator+=(poly& lhs, const poly& rhs);


orderedPoly operator+(const orderedPoly& lhs, const orderedPoly& rhs);


orderedPoly& operator+=(orderedPoly& lhs, const orderedPoly& rhs);


poly operator*(const poly& lhs, const poly& rhs);


orderedPoly operator*(const orderedPoly& lhs, const orderedPoly& rhs);







T2 poly::eval(const std::vector<cplx> &in){
	T2 out = 0;
	for(auto &[coeff, terms] : P){
		if(coeff == (T2)0) continue;
		T2 monome = 1;
		for(int i = 0; i < numvars; i++) monome *= pow(in[i], terms[i]);
		out += monome*coeff;
	}
	return out;
}

T2 poly::dir(const std::vector<cplx> &in, const int resp){
	T2 out = 0;
	for(auto &[coeff, terms] : P){
		if(terms[resp] == 0) continue;
		if(coeff == (T2)0) continue;
		T2 monome = 1;
		for(int i = 0; i < resp; i++) monome *= pow(in[i], terms[i]);
		for(int i = resp+1; i < numvars; i++) monome *= pow(in[i], terms[i]);
		out += monome*coeff*terms[resp]*pow(in[resp], terms[resp]-1);
	}
	return out;
}

void poly::evalValAndDir(const std::vector<cplx> &in, T2 &val, std::vector<cplx> &dir){
	val = 0;
	for(auto &x : dir) x = 0;

	for(auto &[coeff, terms] : P){
		if(coeff == (T2)0) continue;
		T2 monome = coeff;
		for(int i = 0; i < numvars; i++) monome *= pow(in[i], terms[i]);

		if(monome != (T2)0){
			val += monome;
			for(int i = 0; i < numvars; i++){
				if(terms[i]) dir[i] += ((T2)terms[i])*monome/((T2)in[i]);
			}
			continue;
		}

		monome = coeff;
		int a = -1;
		bool all0 = false;
		for(int i = 0; i < numvars; i++){
			if(!terms[i]) continue;
			if(in[i] != (T2)0){monome *= pow(in[i], terms[i]); continue;}

			if(terms[i]-1){all0 = true; break;}
			if(a == -1){all0 = true; break;}
			a = i;
		}

		if(all0) continue;
		if(a != -1){dir[a] += monome; continue;}

		return;//if a==-1 this escaped lower bound of how close double can be to 0
	}

	return;
}

void poly::add_term(T coeff, const std::vector<int>& terms){P.push_back({coeff, terms});}



poly operator+(const poly& lhs, const poly& rhs){
	assert(lhs.numvars == rhs.numvars);
	poly out(lhs.numvars);
	out.P.resize(lhs.P.size()+rhs.P.size(), {(T)1, std::vector<int>(out.numvars)});
	std::copy(lhs.P.begin(), lhs.P.end(), out.P.begin());
	std::copy(rhs.P.begin(), rhs.P.end(), out.P.begin()+lhs.P.size());
	return out;
}


poly& operator+=(poly& lhs, const poly& rhs){
	assert(lhs.numvars == rhs.numvars);
	lhs.P.resize(lhs.P.size()+rhs.P.size(), {(T)1, std::vector<int>(lhs.numvars)});
	std::copy(rhs.P.begin(), rhs.P.end(), lhs.P.begin()+lhs.P.size());
	return lhs;
}


orderedPoly operator+(const orderedPoly& lhs, const orderedPoly& rhs){
	assert(lhs.numvars == rhs.numvars);
	orderedPoly out(lhs.numvars);
	out.P.resize(lhs.P.size()+rhs.P.size(), {(T)1, std::vector<int>(out.numvars)});

	int a = 0, b = 0, c = 0;
	while(a != lhs.P.size() && b != rhs.P.size()){
		if(lhs.P[a].second < rhs.P[b].second) out.P[c++] = lhs.P[a++];
		else if(lhs.P[a].second > rhs.P[b].second) out.P[c++] = rhs.P[b++];
		else{out.P[c] = lhs.P[a++]; out.P[c++].first += rhs.P[b++].first;}
	}
	if(a == lhs.P.size()){std::copy(rhs.P.begin()+b, rhs.P.end(), out.P.begin()+c); c += rhs.P.size()-b;}
	else{std::copy(lhs.P.begin()+a, lhs.P.end(), out.P.begin()+c); c += lhs.P.size()-a;}
	out.P.resize(c);
	return out;
}


orderedPoly& operator+=(orderedPoly& lhs, const orderedPoly& rhs){return (lhs = lhs+rhs);}


poly operator*(const poly& lhs, const poly& rhs){
	assert(lhs.numvars == rhs.numvars);
	poly out(lhs.numvars);
	out.P.resize(lhs.P.size()*rhs.P.size(), {(T)1, std::vector<int>(out.numvars)});
	for(int i = 0; i < lhs.P.size(); i++){
		for(int j = 0; j < rhs.P.size(); j++){
			out.P[i*rhs.P.size()+j].first = lhs.P[i].first*rhs.P[j].first;
			for(int k = 0; k < out.numvars; k++) out.P[i*rhs.P.size()+j].second[k] = lhs.P[i].second[k]+rhs.P[j].second[k];
		}
	}
	return out;
}


orderedPoly operator*(const orderedPoly& lhs, const orderedPoly& rhs){
	assert(lhs.numvars == rhs.numvars);
	int pos = 0;
	orderedPoly out(lhs.numvars);
	if(lhs.P.empty() || rhs.P.empty()) return out;

	out.P.resize(lhs.P.size()*rhs.P.size(), {(T)1, std::vector<int>(out.numvars)});

	std::vector<int> curr(lhs.P.size(), 0);

	int sz = 1;
	while(sz < lhs.P.size()) sz *= 2;
	std::vector<int> bin(2*sz, -1);//bin tree
	for(int i = sz; i < sz+lhs.P.size(); i++) bin[i] = i-sz;
	for(int i = sz-1; i > 0; i--) bin[i] = bin[2*i];

	std::vector<int> vars(out.numvars);
	while(bin[1] != -1){
		int best = bin[1];
		for(int k = 0; k < out.numvars; k++) vars[k] = lhs.P[best].second[k]+rhs.P[curr[best]].second[k];
		out.P[pos++] = {lhs.P[best].first*rhs.P[curr[best]].first, vars};
		curr[best]++;
		if(curr[best] >= rhs.P.size()){
			curr[best] = -1;
			bin[sz+best] = -1;
		}
		int i = (sz+best)/2;
		while(i){
			bool dir = true;
			if(bin[2*i] == -1){bin[i] = bin[2*i+1]; i /= 2; continue;}
			if(bin[2*i+1] == -1){bin[i] = bin[2*i]; i /= 2; continue;}
			for(int k = 0; k < out.numvars; k++){
				int a = lhs.P[bin[2*i]].second[k]+rhs.P[curr[bin[2*i]]].second[k];
				int b = lhs.P[bin[2*i+1]].second[k]+rhs.P[curr[bin[2*i+1]]].second[k];
				if(a != b){dir = (a<b); break;}
			}
			if(dir) bin[i] = bin[2*i];
			else bin[i] = bin[2*i+1];
			i /= 2;
		}
	}
	return out;
}

void orderedPoly::compress(){
	if(this->P.empty()) return;

	int j = 0;
	for(int i = 1; i < this->P.size(); i++){
		if(this->P[i].second == this->P[j].second) this->P[j].first += this->P[i].first;
		else{
			if(this->P[j].first != (T)0) j++;
			if(i != j) this->P[j] = this->P[i];
		}
	}
	if(this->P[j].first != (T)0) j++;

	this->P.resize(j);

	return;
}








//only need to do part from ast tree
int Reader::createExpr(std::string exprtype, std::string polystr, expression &C){//maybe at some point don't expand functions, just leave them as is, like in polystack
    if(exprtype != "POLY"){Log << "fuck you; it's not a bug yet; so don't even try with them general expressions yet\n" << std::flush; return 1;}
    

    int numvars = namevars.size();

    struct polyword{
        char type;
        union{
            char c;
            int varindex;
            int varstackindex;
            cplx val;
        };

        polyword(char t, cplx v) : type(t), val(v){}
        polyword(char t, char C) : type(t), c(C){}
        polyword(char t, int v) : type(t), varindex(v){}
    };

    typedef std::vector<polyword> simpleexpr;

    std::vector<simpleexpr> varstack;

    polystr += '\0';

    int out = 0;

    std::function<int(std::string::iterator&, const std::vector<int>&)> readpoly;
    readpoly = [&](std::string::iterator &it, const std::vector<int> &fstack){//sry I rly had no other viable choice
        simpleexpr curr;
        std::string currWord = "";
        bool gonnabefunc = false;
        std::string::iterator funcit;


        while(true){
            char c = *it++;

            if(!currWord.empty() && (isalpha(c) ? isdigit(currWord[0]) : !(c == '.' || isdigit(c)))){
                //Log << "word: " << currWord << "\n" << std::flush;
                if(isalpha(currWord[0])){
                    if(varnames.count(currWord)) curr.push_back(polyword(2, varnames[currWord]));
                    else if(parsedFuncs.count(currWord)){gonnabefunc = true; funcit = parsedFuncs[currWord].begin();}
                    else{Log << "dumbass can't even remeber thier own variable names\n" << std::flush; out = 2;}
                }
                else if(currWord[0] == '$'){
                    int index = stoi(currWord.substr(1))-1;
                    if(index == -1){Log << "blud doesn't know what not 0 indexed means, from here the read is corrupted\n" << std::flush; out = 6; index = 0;};
                    curr.push_back(polyword(3, fstack[index]));
                }
                else{
                    curr.push_back(polyword(0, stoc(currWord)));
                }
                currWord.clear();
            }

            if(isspace(c)){}
            else if(isop(c)) curr.push_back(polyword(1, c));
            else if(c == '('){
                if(gonnabefunc){
                    std::vector<int> funcstack;
                    while(*(it-1) != ')') funcstack.push_back(readpoly(it, fstack));
                    curr.push_back(polyword(3, readpoly(funcit, funcstack)));
                }
                else{
                    curr.push_back(polyword(3, readpoly(it, fstack)));
                }
            }
            else if(c == ')' || c == ',' || c == '\0') break;
            else currWord += c;
        }

        varstack.push_back(curr);
        return varstack.size()-1;
    };

    if(out != 0) return out;

    auto it = polystr.begin();
    std::vector<int> fstack;
    readpoly(it, fstack);

    if(it != polystr.end()){Log << "fuckass doesn't know how to use parethisys. how tf do you spell that tho actually?\n"; return 8;}

    std::vector<int>& varsused = C.opnums;

    for(auto &expr : varstack){//if you uncomment these you get a clean af log of varstack
        //Log << "{";
        for(auto &word : expr){

            //Log << (int)word.type << ": ";
            //if(word.type == 0) Log << word.val;
            //else if(word.type == 1) Log << word.c;
            //else Log << word.varindex;
            //Log << ", ";

            if(word.type == 2) varsused.push_back(word.varindex);
        }
        //Log << "},\n" << std::flush;
    }

    sort(varsused.begin(), varsused.end());
    varsused.erase(unique(varsused.begin(), varsused.end()), varsused.end());

    int numvarsused = varsused.size();

    std::vector<int> invOpNum(numvars, -1);
    for(int j = 0; j < numvarsused; j++) invOpNum[varsused[j]] = j;

    for(auto &expr : varstack){
        for(auto &word : expr){
            if(word.type == 2) word.varindex = invOpNum[word.varindex];
        }
        expr.push_back(polyword(-1, '\0'));
    }

    std::vector<orderedPoly> polystack(varstack.size(), orderedPoly(numvarsused));

    for(int j = 0; j < varstack.size(); j++){
        std::vector<polyword> &expr = varstack[j];

        int k = 0;
        while(k < expr.size()-1){
            cplx coeff = 1;
            std::vector<int> stackindex = {};
            std::vector<int> purevars(numvarsused, 0);
            
            for(; expr[k].type == 1; k++){
                if(expr[k].c == '-') coeff *= -1;
            }
            while(k < expr.size()-1){
                if(expr[k].type == 0){
                    if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                        if(expr[k+2].type != 0){Log << "dfsfgjfhgnbfdghfgbv\n" << std::flush; return 4;}
                        coeff *= pow(expr[k].val, expr[k+2].val);
                        k += 2;
                    }
                    else coeff *= expr[k].val;
                }
                else if(expr[k].type == 2){
                    if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                        if(expr[k+2].type != 0){Log << "dfsfgjfhgnbfdghfgbv\n" << std::flush; return 4;}
                        purevars[expr[k].varindex] += (int)expr[k+2].val.real();
                        k += 2;
                    }
                    else purevars[expr[k].varindex]++;
                }
                else if(expr[k].type == 3){
                    if(expr[k+1].type == 1 && expr[k+1].c == '^'){
                        if(expr[k+2].type != 0){Log << "dfsfgjfhgnbfdghfgbv\n" << std::flush; return 4;}
                        int cnt = expr[k+2].val.real();
                        while(cnt--) stackindex.push_back(expr[k].varstackindex);
                        k += 2;
                    }
                    else stackindex.push_back(expr[k].varstackindex);
                }
                else if(expr[k].type == 1){
                    if(expr[k].c == '-' || expr[k].c == '+') break;
                    if(expr[k].c == '^'){Log << "bruh\n" << std::flush; return 5;}
                    if(expr[k].c == '*'){}
                    if(expr[k].c == '/'){Log << "divison by error, if you couldn't tell divison is not implemented yet you bafoon\n" << std::flush; return 3;}
                }
                else{
                    Log << "this branch of the if/else is impossible to reach, congratz\n" << std::flush;
                    return 7;
                }
                k++;
            }

            orderedPoly out(numvarsused);
            out.add_term(coeff, purevars);
            for(auto &i : stackindex) out = out*polystack[i];
            polystack[j] += out;
        }
        polystack[j].compress();
    }

    C.numvars = C.opnums.size();
    C.P = polystack[polystack.size()-1].P;

    Log << "expr successfully read\n" << std::flush;
    
    return 0;
}

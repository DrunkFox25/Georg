//this code is currently unused

template<typename T = double>
struct poly{
	int numvars = 0;
	std::vector<std::pair<T, std::vector<int>>> P;

	poly(){}

	poly(int n) : numvars(n){}

	template<typename T2>
	T2 eval(const std::vector<T2> &in);

	template<typename T2>
	T2 dir(const std::vector<T2> &in, const int resp);

	template<typename T2>
	void evalValAndDir(const std::vector<T2> &in, T2 &val, std::vector<T2> &dir);// need to make this better

	template<typename T2>
	T2 operator()(const std::vector<T2> &in){return eval(in);}

	void add_term(T coeff, const std::vector<int>& terms);

	/*poly<T>& operator=(const orderedPoly<T>& rhs){
		P = rhs.P;
		numvars = rhs.numvars;
		return *this;
	}*/
};

template<typename T = double>
struct orderedPoly : poly<T>{
	using poly<T>::poly;

	void compress();
};

template<typename T>
poly<T> operator+(const poly<T>& lhs, const poly<T>& rhs);

template<typename T>
poly<T>& operator+=(poly<T>& lhs, const poly<T>& rhs);

template<typename T>
orderedPoly<T> operator+(const orderedPoly<T>& lhs, const orderedPoly<T>& rhs);

template<typename T>
orderedPoly<T>& operator+=(orderedPoly<T>& lhs, const orderedPoly<T>& rhs);

template<typename T>
poly<T> operator*(const poly<T>& lhs, const poly<T>& rhs);

template<typename T>
orderedPoly<T> operator*(const orderedPoly<T>& lhs, const orderedPoly<T>& rhs);


template<typename T>
std::ostream& operator<<(std::ostream &out, poly<T> &P);














template<typename T>
template<typename T2>
T2 poly<T>::eval(const std::vector<T2> &in){
	T2 out = 0;
	for(auto &[coeff, terms] : P){
		if(coeff == (T2)0) continue;
		T2 monome = 1;
		for(int i = 0; i < numvars; i++) monome *= pow(in[i], terms[i]);
		out += monome*coeff;
	}
	return out;
}

template<typename T>
template<typename T2>
T2 poly<T>::dir(const std::vector<T2> &in, const int resp){
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

template<typename T>
template<typename T2>
void poly<T>::evalValAndDir(const std::vector<T2> &in, T2 &val, std::vector<T2> &dir){
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

template<typename T>
void poly<T>::add_term(T coeff, const std::vector<int>& terms){P.push_back({coeff, terms});}

template<typename T>
std::ostream& operator<<(std::ostream &out, poly<T> &P){
	std::vector<std::string> vars;
	if(P.numvars <= 26){
		for(int i = 0; i < P.numvars; i++) vars[i] = ('a'+i);
	}
	else{
		for(int i = 0; i < P.numvars; i++) vars[i] = ("x"+i);
	}
	out << "[";
	for(int i = 0; i < P.numvars-1; i++) out << vars[i] << ",";
	out << "](";
	for(auto &[coeff, terms] : P.P){
		out << coeff;
		for(int i = 0; i < P.numvars; i++){
			if(terms[i] == 0) continue;
			out << "*" << vars[i] << "^" << terms[i];
		}
		out << " + ";
	}
	out << ")";
	return out;
}

template<typename T>
poly<T> operator+(const poly<T>& lhs, const poly<T>& rhs){
	assert(lhs.numvars == rhs.numvars);
	poly<T> out(lhs.numvars);
	out.P.resize(lhs.P.size()+rhs.P.size(), {(T)1, std::vector<int>(out.numvars)});
	std::copy(lhs.P.begin(), lhs.P.end(), out.P.begin());
	std::copy(rhs.P.begin(), rhs.P.end(), out.P.begin()+lhs.P.size());
	return out;
}

template<typename T>
poly<T>& operator+=(poly<T>& lhs, const poly<T>& rhs){
	assert(lhs.numvars == rhs.numvars);
	lhs.P.resize(lhs.P.size()+rhs.P.size(), {(T)1, std::vector<int>(lhs.numvars)});
	std::copy(rhs.P.begin(), rhs.P.end(), lhs.P.begin()+lhs.P.size());
	return lhs;
}

template<typename T>
orderedPoly<T> operator+(const orderedPoly<T>& lhs, const orderedPoly<T>& rhs){
	assert(lhs.numvars == rhs.numvars);
	orderedPoly<T> out(lhs.numvars);
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

template<typename T>
orderedPoly<T>& operator+=(orderedPoly<T>& lhs, const orderedPoly<T>& rhs){return (lhs = lhs+rhs);}

template<typename T>
poly<T> operator*(const poly<T>& lhs, const poly<T>& rhs){
	assert(lhs.numvars == rhs.numvars);
	poly<T> out(lhs.numvars);
	out.P.resize(lhs.P.size()*rhs.P.size(), {(T)1, std::vector<int>(out.numvars)});
	for(int i = 0; i < lhs.P.size(); i++){
		for(int j = 0; j < rhs.P.size(); j++){
			out.P[i*rhs.P.size()+j].first = lhs.P[i].first*rhs.P[j].first;
			for(int k = 0; k < out.numvars; k++) out.P[i*rhs.P.size()+j].second[k] = lhs.P[i].second[k]+rhs.P[j].second[k];
		}
	}
	return out;
}

template<typename T>
orderedPoly<T> operator*(const orderedPoly<T>& lhs, const orderedPoly<T>& rhs){
	assert(lhs.numvars == rhs.numvars);
	int pos = 0;
	orderedPoly<T> out(lhs.numvars);
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

template<typename T>
void orderedPoly<T>::compress(){
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
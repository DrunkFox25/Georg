#include <bits/stdc++.h>

#define ll long long
#define cplx std::complex<double>

#define special 0x7FF041524156494E// hehe
//#define isNaN(x) !(~(*((ll*)(&x)) | 0x800FFFFFF))


std::random_device rd;

int modexp(long long x, int expo, int mod){
	long long out = 1;
	while(expo){
		if(expo&1){out *= x; out %= mod;}
		expo /= 2;
		x *= x; x %= mod;
	}
	return out;
}

long long modInv(long long x, long long mod){
	x %= mod;
	if(x == 1) return 1;
	return (((x-modInv(mod, x))*mod+1)/x);//can break here, but only with big numbers ie. x*mod > 2^63
}

template<int mod>
struct primeModBinom{
	int sz = 1;
	std::vector<int> fact = {1};

	void resize(int n){
		fact.resize(n);

		long long curr = fact[sz-1];

		while(sz < n){
			curr *= sz;
			curr %= mod;
			fact[sz++] = curr;
		}

		return;
	}

	int binom(int n, int k){
		if(k > n || k < 0) return 0;

		if(sz < n+1) resize(n+1);

		return (fact[n]*modInv(((long long)fact[k])*fact[n-k], mod))%mod;
	}

	primeModBinom(){}

	primeModBinom(int n){resize(n);}
};

struct primes{
	int n;
	std::vector<int> P;
	std::vector<bool> isPrime;

	primes(int N) : n(N){
		isPrime.resize(n+1, true);
		isPrime[0] = isPrime[1] = false;

		for(int i = 2; i < n; i++){
			if(!isPrime[i]) continue;
			P.push_back(i);
			for(int j = i*i; j < n; j += i) isPrime[j] = false;//not doing more advanced version as this allows speedups
		}
	}

	std::vector<std::pair<int, int>> factorize(int x){
		std::vector<std::pair<int, int>> out;
		for(auto &p : P){
			if(x%p) continue;
			int expo = 0;
			while(!(x%p)){x /= p; expo++;}
			out.push_back({p, expo});
		}
		if(x != 1) out.push_back({x, 1});
		return out;
	}
};


bool isprimeMillerRabin(int n){//only works up to ~3e9
	int d = n-1, s = 0;
	while(d%2 != 1){d /= 2; s++;}

	for(auto& p : {2,3,5,7}){
		if(!(n%p)) return false;
		long long k = modexp(p,d,n);

		if(k == 1) continue;
		while(k != 1 && k != n-1) k *= k;
		if(k == 1) return false;
	}
	return true;
}

template<typename T>
std::ostream& operator<<(std::ostream &out, std::vector<T> &vec){
	for(auto &x : vec) out << x << " ";
	return out;
}



//just some code lying around
struct point{//need to template this
	union{
		struct {int x, y;};
		std::array<int, 2> coords;
	};

	point(){}

	point(int X, int Y){x = X; y = Y;}

	constexpr int sqNorm() const {
		return x*x+y*y;
	}
};


std::istream& operator>>(std::istream& in, point& p){
	return in >> p.x >> p.y;
}

std::ostream& operator<<(std::ostream& out, point& p){
	return out << "(" << p.x << ", " << p.y << ")";
}



point operator*(const point& lhs, const int& rhs){
	return point(lhs.x*rhs, lhs.y*rhs);
}

point operator-(const point& lhs, const point& rhs){
	return point(lhs.x-rhs.x, lhs.y-rhs.y);
}

point operator-(const point& p){
	return point(-p.x, -p.y);
}

constexpr bool operator<(const point& lhs, const point &rhs){return lhs.coords < rhs.coords;}

constexpr int signedArea(const point &A, const point &B, const point &C){//returns twice the signed area, positive if cc wise
	return A.x*B.y-B.x*A.y+B.x*C.y-C.x*B.y+C.x*A.y-A.x*C.y;
}

constexpr int crossProduct(const point &A, const point &B){//positive if cc wise
	return A.x*B.y-B.x*A.y;
}

constexpr int sqDist(const point &A, const point &B){
	return (A.x-B.x)*(A.x-B.x) + (A.y-B.y)*(A.y-B.y);
}

constexpr bool polarComp(const point &A, const point &B){//A<B
	int temp = crossProduct(A, B);

	if(temp == 0) return (A.sqNorm() < B.sqNorm());
	return temp > 0;
}

void polarSort(std::vector<point>::iterator begin, std::vector<point>::iterator end, const point &center){//cc wise
	std::sort(begin, end, [&center](const auto &rhs, const auto &lhs){return polarComp(rhs-center, lhs-center);});
}

int splitOverDirFromLine(std::vector<point> &points, const point &center, const point &dir){
	int a = 0, b = points.size()-1;
	while(a != b){
		if(signedArea(center, dir, points[a]) >= 0) a++;
		else{std::swap(points[a], points[b]); b--;}
	}
	return a;
}

int fullPolar(std::vector<point> &points, const point &center, const point &dir){//cc wise
	int a = splitOverDirFromLine(points, center, dir);

    polarSort(points.begin(), points.begin()+a, center);
    polarSort(points.begin()+a, points.end(), center);

	return a;
}




template<typename T>
struct numbering{
	int sz = 0;
	std::map<T, int> m;

	int operator()(T x){
		if(!m.count(x)) m[x] = sz++;
		return sz;
	}

	int operator[](T x){
		if(!m.count(x)) return -1;
		return m[x];
	}
};

template<typename T = double>
struct poly{
	int numvars;
	std::vector<std::pair<T, std::vector<int>>> P;

	poly(){}

	poly(int n) : numvars(n){}

	template<typename T2>
	T2 eval(const std::vector<T2> &in){
		T2 out = 0;
		for(auto &[coeff, terms] : P){
			if(coeff == (T2)0) continue;
			T2 monome = 1;
			for(int i = 0; i < numvars; i++) monome *= pow(in[i], terms[i]);
			out += monome*coeff;
		}
		return out;
	}

	template<typename T2>
	T2 dir(const std::vector<T2> &in, const int resp){
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

	template<typename T2>
	void evalValAndDir(const std::vector<T2> &in, T2 &val, std::vector<T2> &dir){
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

	template<typename T2>
	T2 operator()(const std::vector<T2> &in){return eval(in);}
};



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

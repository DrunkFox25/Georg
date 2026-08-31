#pragma once

#include <bits/stdc++.h>

#define ll long long
#define cplx std::complex<double>

#define special 0x7FF041524156494E// hehe
//#define isNaN(x) !(~(*((ll*)(&x)) | 0x800FFFFFF))

inline std::random_device rd;

template<typename T>
std::ostream& operator<<(std::ostream &out, std::vector<T> &vec);

template<typename T>
std::istream& operator>>(std::istream &in, std::vector<T> &vec);

struct point{//need to template this
	union{
		struct {int x, y;};
		std::array<int, 2> coords;
	};

	point(){}

	point(int X, int Y) : x(X), y(Y){}

	constexpr int sqNorm() const {
		return x*x+y*y;
	}
};

std::istream& operator>>(std::istream& in, point& p);

std::ostream& operator<<(std::ostream& out, point& p);

point operator*(const point& lhs, const int& rhs);

point operator-(const point& lhs, const point& rhs);

point operator-(const point& p);

constexpr bool operator<(const point& lhs, const point &rhs);

constexpr int signedArea(const point &A, const point &B, const point &C);//returns twice the signed area, positive if cc wise

constexpr int crossProduct(const point &A, const point &B);//positive if cc wise

constexpr int sqDist(const point &A, const point &B);


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
	T2 eval(const std::vector<T2> &in);

	template<typename T2>
	T2 dir(const std::vector<T2> &in, const int resp);

	template<typename T2>
	void evalValAndDir(const std::vector<T2> &in, T2 &val, std::vector<T2> &dir);// need to make this better

	template<typename T2>
	T2 operator()(const std::vector<T2> &in){return eval(in);}
};



template<typename T>
std::ostream& operator<<(std::ostream &out, poly<T> &P);


//defs



template<typename T>
std::ostream& operator<<(std::ostream &out, std::vector<T> &vec){
	for(auto &x : vec) out << x << " ";
	return out;
}

template<typename T>
std::istream& operator>>(std::istream &in, std::vector<T> &vec){
	for(auto &x : vec) in >> x;
	return in;
}

inline std::istream& operator>>(std::istream& in, point& p){
	return in >> p.x >> p.y;
}

inline std::ostream& operator<<(std::ostream& out, point& p){
	return out << "(" << p.x << ", " << p.y << ")";
}

inline point operator*(const point& lhs, const int& rhs){
	return point(lhs.x*rhs, lhs.y*rhs);
}

inline point operator-(const point& lhs, const point& rhs){
	return point(lhs.x-rhs.x, lhs.y-rhs.y);
}

inline point operator-(const point& p){
	return point(-p.x, -p.y);
}

inline constexpr bool operator<(const point& lhs, const point &rhs){return lhs.coords < rhs.coords;}

inline constexpr int signedArea(const point &A, const point &B, const point &C){
	return A.x*B.y-B.x*A.y+B.x*C.y-C.x*B.y+C.x*A.y-A.x*C.y;
}

inline constexpr int crossProduct(const point &A, const point &B){
	return A.x*B.y-B.x*A.y;
}

inline constexpr int sqDist(const point &A, const point &B){
	return (A.x-B.x)*(A.x-B.x) + (A.y-B.y)*(A.y-B.y);
}

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

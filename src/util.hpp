#pragma once

#include <bits/stdc++.h>

#define ll long long
#define cplx std::complex<double>

#define special 0x7FF041524156494E// hehe
//#define isNaN(x) !(~(*((ll*)(&x)) | 0x800FFFFFF))

inline std::random_device rd;

inline std::ostream* log_def;
#define Log *log_def//don't forget to flush or the widget never updates
#define Set_Log(x) log_def = x

//can't use for auto due to bools:


template<typename T>
std::ostream& operator<<(std::ostream &out, const std::vector<T> &vec){
	for(size_t i = 0; i < vec.size(); i++){
		if(i != 0) out << " ";
		out << vec[i];
	}
	return out;
}

template<typename T>
std::istream& operator>>(std::istream &in, std::vector<T> &vec){
	for(size_t i = 0; i < vec.size(); i++) in >> vec[i];
	return in;
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

	size_t size(){return sz;}
};

template<typename T>
struct binumbering{
	std::map<T, int> m;
	std::vector<T> v;

	void operator()(T x){
		if(!m.count(x)){m[x] = v.size(); v.push_back(x);}
	}

	int operator[](T x){
		if(!m.count(x)) return -1;
		return m[x];
	}

	int operator[](int x){
		return v[x];
	}

	size_t size(){return v.size();}
};

cplx stoc(std::string str);//Formats: just a double, (a,b) a+bi a,b a;b a b

int isop(char c);

std::stringstream getFile(std::string fname);

struct TeeStream : public std::ostream{
    struct TeeBuf : public std::streambuf{
        std::streambuf* m_buf1;
        std::streambuf* m_buf2;

        TeeBuf(std::streambuf* buf1, std::streambuf* buf2);

        virtual int_type overflow(int_type c) override;

        virtual int sync() override;
    };

    TeeBuf m_tbuf;

    TeeStream(std::ostream& stream1, std::ostream& stream2);

	TeeStream(std::ostream* stream1, std::ostream* stream2);
};






#include "points.hpp"
#include "poly.hpp"
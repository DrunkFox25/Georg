#include "util.hpp"

cplx stoc(std::string str){
	//holy fuck typing str,std,stod was painful. kept typing wrong one

	//Log << "stoc call: " << str << "\n" << std::flush;

	bool hasi = (str.find('i') != std::string::npos);
    str = std::regex_replace(str, std::regex("[,;+]"), " ");
    str = std::regex_replace(str, std::regex("[()i]"), "");
    str = std::regex_replace(str, std::regex("-"), " -");
	//should just be two space seperated numbers
	
	std::size_t stopi = 0;
	double val = std::stod(str, &stopi);
	str = str.substr(stopi);

	if(std::all_of(str.begin(), str.end(), [](unsigned char ch){return std::isspace(ch);})){
		if(!hasi) return cplx(val, 0);
		else return cplx(0, val);
	}

	return cplx(val, stod(str));
}

int isop(char c){
	return (c == '^' || c == '+' || c == '*' || c == '-' || c == '/');
}

std::stringstream getFile(std::string fname){
    std::ifstream file(fname);
    if (!file.is_open()) Log << "Error: Could not open the file: " << fname << "\n" << std::flush;
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer;
}

TeeStream::TeeBuf::TeeBuf(std::streambuf* buf1, std::streambuf* buf2) : m_buf1(buf1), m_buf2(buf2) {}

std::streambuf::int_type TeeStream::TeeBuf::overflow(std::streambuf::int_type c){
    if (c == traits_type::eof()) {
        return traits_type::not_eof(c);
    }
    
    bool ok1 = m_buf1->sputc(c) != traits_type::eof();
    bool ok2 = m_buf2->sputc(c) != traits_type::eof();

    return (ok1 && ok2) ? c : traits_type::eof();
}

int TeeStream::TeeBuf::sync(){
    int res1 = m_buf1->pubsync();
    int res2 = m_buf2->pubsync();
    return (res1 == 0 && res2 == 0) ? 0 : -1;
}

TeeStream::TeeStream(std::ostream& stream1, std::ostream& stream2) : std::ostream(&m_tbuf), m_tbuf(stream1.rdbuf(), stream2.rdbuf()) {}

TeeStream::TeeStream(std::ostream* stream1, std::ostream* stream2) : std::ostream(&m_tbuf), m_tbuf(stream1->rdbuf(), stream2->rdbuf()) {}
//this code is currently unused


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

template<typename T>
struct rect{
	T left, right, down, up;

	rect(){}

	rect(T l, T r, T d, T u) : left(l), right(r), down(d), up(u) {}
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

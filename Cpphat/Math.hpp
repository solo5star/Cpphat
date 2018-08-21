#ifndef CPPHAT_MATH_H_
#define CPPHAT_MATH_H_

class Point {
public:
	int X, Y;

	Point(int _x = 0, int _y = 0);

	Point operator+(const Point& op);
	Point& operator+=(const Point& op);
	Point operator-(const Point& op);
	Point& operator-=(const Point& op);

	bool operator<(const Point& op);
	bool operator<=(const Point& op);
	bool operator>(const Point& op);
	bool operator>=(const Point& op);

	int to1D(int width);
	static Point from1D(int d, int width);
};

#endif
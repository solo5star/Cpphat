#include "Math.hpp"

Point::Point(int _x, int _y)
	: X(_x)
	, Y(_y)
{ }

Point Point::operator+(const Point& op) {
	Point tmp(op);
	tmp.X += X;
	tmp.Y += Y;
	return tmp;
}

Point& Point::operator+=(const Point& op) {
	X += op.X;
	Y += op.Y;
	return *this;
}

Point Point::operator-(const Point& op) {
	Point tmp(op);
	tmp.X -= X;
	tmp.Y -= Y;
	return tmp;
}

Point& Point::operator-=(const Point& op) {
	X -= op.X;
	Y -= op.Y;
	return *this;
}

bool Point::operator<(const Point& op) {
	return X < op.X && Y < op.Y;
}

bool Point::operator<=(const Point& op) {
	return X <= op.X && Y <= op.Y;
}

bool Point::operator>(const Point& op) {
	return X > op.X && Y > op.Y;
}

bool Point::operator>=(const Point& op) {
	return X >= op.X && Y >= op.Y;
}

int Point::to1D(int width) {
	return Y * width + X;
}

Point Point::from1D(int d, int width) {
	int x = d % width;
	int y = d / width;
	return Point(x, y);
}
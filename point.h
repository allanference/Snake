/*
 * Copyright (c) 2013 Ahmed Samy  <f.fallen45@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef POINT_H
#define POINT_H

#include <array>
#include <ostream>

template<typename T>
class TPoint
{
private:
	T m_x, m_y;

public:

	TPoint()
		: m_x(0), m_y(0)
	{
	}
	TPoint(const T& x, const T& y)
		: m_x(x), m_y(y)
	{
	}

	T x() const { return m_x; }
	T y() const { return m_y; }

	TPoint<T>& operator=(const TPoint<T>& other) {
		m_x = other.m_x;
		m_y = other.m_y;
		return *this;
	}

	bool operator==(const TPoint<T>& other) const {
		return other.m_x == m_x && other.m_y == m_y;	
	}
	bool operator!=(const TPoint<T>& other) const {
		return other.m_x != m_x && other.m_y != m_y;
	}
	bool operator<(const TPoint<T>& other) const {
		return m_x < other.m_x && m_y < other.m_y;
	}

	void checkBounds(int maxX, int maxY)
	{
		// Check screen bounds.
		// This is for offscreen movements, i.e
		// If the position hits the maximum width
		// the position is reset so that it'd start
		// at the beginning of the width, and so on.
		if (m_x < 0)	m_x = maxX;	// Far Left
		if (m_x > maxX)	m_x = 0;	// Far Right
		if (m_y < 0)	m_y = maxY;	// Far Down
		if (m_y > maxY)	m_y = 0;	// Far Up
	}
};

typedef TPoint<int> Point;

extern std::ostream& operator<<(std::ostream& os, const Point& p);

#endif


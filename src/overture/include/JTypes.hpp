/*
 * JVector.hpp
 *
 *  Created on: 13 nov. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_JVECTOR_HPP_
#define OVERTURE_JVECTOR_HPP_


struct Vector2;
struct Quadrilateral;


struct Vector2 {
	Vector2();
	Vector2( float x, float y );


	void print();

	float extrapolateX( Vector2 other, float atY );
	float extrapolateY( Vector2 other, float atX );
	float angle();

	bool isInside( float x, float y, float w, float h );
	bool isInside( Vector2 pos, Vector2 size );


	float x;
	float y;


	Vector2 operator++(int) {
		x += 1.0f, y += 1.0f;
		return *this;
	}
	Vector2& operator++() {
		x += 1.0f, y += 1.0f;
		return *this;
	}

	Vector2 operator--(int) {
		x -= 1.0f, y -= 1.0f;
		return *this;
	}
	Vector2& operator--() {
		x -= 1.0f, y -= 1.0f;
		return *this;
	}

    Vector2& operator=(const float& rhs) {
    	x = rhs, y = rhs;
    	return *this;
    }

    Vector2& operator*=(const Vector2& rhs) {
        x *= rhs.x, y *= rhs.y;
        return *this;
    }
    Vector2& operator*=(const float& rhs) {
        x *= rhs, y *= rhs;
        return *this;
    }

    friend Vector2 operator*(Vector2 lhs, const Vector2& rhs) {
    	lhs *= rhs;
    	return lhs;
    }
    friend Vector2 operator*(Vector2 lhs, const float& rhs) {
    	lhs *= rhs;
    	return lhs;
    }

    Vector2& operator+=(const Vector2& rhs) {
    	x += rhs.x, y += rhs.y;
    	return *this;
    }
    Vector2& operator+=(const float& rhs) {
    	x += rhs, y += rhs;
    	return *this;
    }

    Vector2& operator-=(const Vector2& rhs) {
    	x -= rhs.x, y -= rhs.y;
    	return *this;
    }
    Vector2& operator-=(const float& rhs) {
    	x -= rhs, y -= rhs;
    	return *this;
    }

    friend Vector2 operator+(Vector2 lhs, const Vector2& rhs) {
    	lhs += rhs;
    	return lhs;
    }
    friend Vector2 operator+(Vector2 lhs, const float& rhs) {
    	lhs += rhs;
    	return lhs;
    }

    friend Vector2 operator-(Vector2 lhs, const Vector2& rhs) {
    	lhs -= rhs;
    	return lhs;
    }
    friend Vector2 operator-(Vector2 lhs, const float& rhs) {
    	lhs -= rhs;
    	return lhs;
    }
};



struct Quadrilateral {
	Quadrilateral();
	Quadrilateral( float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float p3x, float p3y );
	Quadrilateral( float posx, float posy, float width, float height );
	Quadrilateral( Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3 );

	void flipX( float offset );

	bool hasValues();

	// Top left
	Vector2 p0;
	// Top right
	Vector2 p1;
	// Bottom right
	Vector2 p2;
	// Bottom left
	Vector2 p3;

	Quadrilateral& operator*=(const float& f) {
		p0 *= f, p1 *= f, p2 *= f, p3 *= f;
		return *this;
	}
	Quadrilateral& operator+=(const float& f) {
		p0 += f, p1 += f, p2 += f, p3 += f;
		return *this;
	}
	Quadrilateral& operator-=(const float& f) {
		p0 -= f, p1 -= f, p2 -= f, p3 -= f;
		return *this;
	}

	Quadrilateral& operator*=(const Vector2& f) {
		p0 *= f, p1 *= f, p2 *= f, p3 *= f;
		return *this;
	}
	Quadrilateral& operator+=(const Vector2& f) {
		p0 += f, p1 += f, p2 += f, p3 += f;
		return *this;
	}
	Quadrilateral& operator-=(const Vector2& f) {
		p0 -= f, p1 -= f, p2 -= f, p3 -= f;
		return *this;
	}
};

typedef Quadrilateral Quad;




#endif /* OVERTURE_JVECTOR_HPP_ */

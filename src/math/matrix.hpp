/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

#ifndef HEADER_WINDSTILLE_MATH_MATRIX_HPP
#define HEADER_WINDSTILLE_MATH_MATRIX_HPP

#include <glm/glm.hpp>

#include "math/vector3.hpp"

typedef glm::mat4 Matrix;

#if 0

//: 4x4 Matrix.
class Matrix
{
  //! Construction:
public:
  //: Constructs a 4x4 matrix.
  Matrix();

  Matrix(const Matrix &copy);

  Matrix(const float *matrix);

  Matrix(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33);

  /** Returns identity matrix */
  static Matrix identity();

  //! Attributes:
public:
  float matrix[16];

  //: Operator that returns the matrix cell at the given index.
  float &operator[](int i) { return matrix[i]; }

  //: Operator that returns the matrix cell at the given index.
  const float &operator[](int i) const { return matrix[i]; }

  inline float&       operator()(int row, int col) { return matrix[4*col + row]; }
  inline const float& operator()(int row, int col) const { return matrix[4*col + row]; }

  //: Operator that returns the matrix cell at the given index.
  float &operator[](unsigned int i) { return matrix[i]; }

  //: Operator that returns the matrix cell at the given index.
  const float &operator[](unsigned int i) const { return matrix[i]; }

  //: Returns the x coordinate for the point (0,0,0) multiplied with this matrix.
  float get_origin_x() const;

  //: Returns the y coordinate for the point (0,0,0) multiplied with this matrix.
  float get_origin_y() const;

  //: Returns the z coordinate for the point (0,0,0) multiplied with this matrix.
  float get_origin_z() const;

  //! Operations:
public:
  //: Copy assignment operator.
  Matrix &operator =(const Matrix &copy);

  //: Equality operator.
  bool operator==(const Matrix &other) const;

  //: Not-equal operator.
  bool operator!=(const Matrix &other) const;

  //: Multiply two matrices.
  Matrix multiply(const Matrix &matrix) const;

  //: Calculate the inverse of this matrix
  Matrix inverse() const;

  /** Calculate the inverse of this matrix in a fast way, only works
   *  for matrices that represent rotation and translation */
  Matrix fast_inverse() const;

  //: Callculate the determinat of this matrix
  float determinat() const;

  //: Multiply a vector with this matrix
  Vector3 multiply(const Vector3& vector) const;

  //: Transpose this matrix and return the result
  Matrix transpose() const;

  //: Multiply the matrix with the given scale/translate/rotate matrix
  Matrix scale(float x, float y, float z) const;
  Matrix translate(float x, float y, float z) const;
  Matrix rotate(float angle, float x, float y, float z) const;

  //! Implementation:
private:
};

std::ostream& operator<<(std::ostream& s, const Matrix& m);

#endif

#endif

/* EOF */

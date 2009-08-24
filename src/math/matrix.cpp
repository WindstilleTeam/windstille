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

#include <assert.h>
#include <string.h>
#include <iomanip>
#include <math.h>
#include "vector3.hpp"

/////////////////////////////////////////////////////////////////////////////
// Matrix construction:

Matrix::Matrix()
{
  memset(matrix, 0, sizeof(float[16]));
}

Matrix
Matrix::identity()
{
  Matrix matrix;

  matrix.matrix[0]  = 1.0;
  matrix.matrix[5]  = 1.0;
  matrix.matrix[10] = 1.0;
  matrix.matrix[15] = 1.0;
	
  return matrix;
}

Matrix::Matrix(const Matrix &copy)
{
  for (int i = 0; i < 16; ++i)
    matrix[i] = copy.matrix[i];
}

Matrix::Matrix(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13, 
               float m20, float m21, float m22, float m23, 
               float m30, float m31, float m32, float m33)
{
  Matrix& m = *this;

  m(0,0) = m00;
  m(1,0) = m10;
  m(2,0) = m20;
  m(3,0) = m30;

  m(0,1) = m01;
  m(1,1) = m11;
  m(2,1) = m21;
  m(3,1) = m31;

  m(0,2) = m02;
  m(1,2) = m12;
  m(2,2) = m22;
  m(3,2) = m32;

  m(0,3) = m03;
  m(1,3) = m13;
  m(2,3) = m23;
  m(3,3) = m33;
}

Matrix::Matrix(const float* init_matrix)
{
  for (int i = 0; i < 16; ++i)
    matrix[i] = init_matrix[i];
}

/////////////////////////////////////////////////////////////////////////////
// Matrix attributes:

float Matrix::get_origin_x() const
{
  return matrix[12];
}

float Matrix::get_origin_y() const
{
  return matrix[13];
}

float Matrix::get_origin_z() const
{
  return matrix[14];
}

/////////////////////////////////////////////////////////////////////////////
// Matrix operations:

bool Matrix::operator==(const Matrix &other) const 
{
  for (int i=0; i<16; i++)
    if (matrix[i] != other.matrix[i]) return false;
  return true;
}

bool Matrix::operator!=(const Matrix &other) const
{
  for (int i=0; i<16; i++)
    if (matrix[i] != other.matrix[i]) return true;
  return false;
}

Matrix &Matrix::operator =(const Matrix &copy)
{
  for (int i=0; i<16; i++)
    matrix[i] = copy.matrix[i];
  return *this;
}

Matrix Matrix::multiply(const Matrix &mult) const
{
  Matrix result;
  for (int x=0; x<4; x++)
    {
      for (int y=0; y<4; y++)
        {
          result.matrix[x+y*4] =
            matrix[x   ] * mult.matrix[y*4  ] +
            matrix[x+ 4] * mult.matrix[y*4+1] +
            matrix[x+ 8] * mult.matrix[y*4+2] +
            matrix[x+12] * mult.matrix[y*4+3];
        }
    }
  return result;
}

Vector3
Matrix::multiply(const Vector3& v) const
{ // assuming Vector3 is a homogenous vector (x,y,z,1)
  // FIXME: Test me
  const Matrix& m = *this;

  float x = m(0,0)*v.x + m(0,1)*v.y + m(0,2)*v.z;
  float y = m(1,0)*v.x + m(1,1)*v.y + m(1,2)*v.z;
  float z = m(2,0)*v.x + m(2,1)*v.y + m(2,2)*v.z;

  //float w = m(3,0)*1 + m(3,1)*1 + m(3,2)*1 + m(3,3)*1 ; // since we
  //are multiplying with a rot matrix this stays 1 and can be ignored
  //for now

  return Vector3(x, y, z);
}

Matrix
Matrix::scale(float x, float y, float z)
{
  Matrix m = Matrix::identity();
  m[0]  = x;
  m[5]  = y;
  m[10] = z;
  return multiply(m);
}

Matrix
Matrix::translate(float x, float y, float z)
{
  Matrix m = Matrix::identity();
  m[12] = x;
  m[13] = y;
  m[14] = z;
  return multiply(m);
}

Matrix
Matrix::rotate(float angle, float x, float y, float z)
{
  float len2 = x*x+y*y+z*z;
  if (len2 != 1.0)
    {
      float len = sqrtf(len2);
      x /= len;
      y /= len;
      z /= len;
    }

  float c = cosf(angle * 3.14159265f / 180.0f);
  float s = sinf(angle * 3.14159265f / 180.0f);

  Matrix matrix = Matrix::identity();
  matrix[0]  = x*x*(1-c)+c;
  matrix[1]  = y*x*(1-c)+z*s;
  matrix[2]  = x*z*(1-c)-y*s;

  matrix[4]  = x*y*(1-c)-z*s;
  matrix[5]  = y*y*(1-c)+c;
  matrix[6]  = y*z*(1-c)+x*s;

  matrix[8]  = x*z*(1-c)+y*s;
  matrix[9]  = y*z*(1-c)-x*s;
  matrix[10] = z*z*(1-c)+c;

  return multiply(matrix);
}
// http://www.cvl.iis.u-tokyo.ac.jp/~miyazaki/tech/teche23.html
// http://gpwiki.org/index.php/MathGem:Fast_Matrix_Inversion

Matrix
Matrix::inverse() const
{
  float det = determinat();
  if (det != 0)
    {
      Matrix b;
#if 0
      const Matrix& m = *this;

      // FIXME: wrong start index, should be 0, is 1
      b(0,0) = m(2,2)*m(3,3)*m(4,4) + m(2,3)*m(3,4)*m(4,2) + m(2,4)*m(3,2)*m(4,3) - m(2,2)*m(3,4)*m(4,3) - m(2,3)*m(3,2)*m(4,4) - m(2,4)*m(3,3)*m(4,2);
      b(0,1) = m(1,2)*m(3,4)*m(4,3) + m(1,3)*m(3,2)*m(4,4) + m(1,4)*m(3,3)*m(4,2) - m(1,2)*m(3,3)*m(4,4) - m(1,3)*m(3,4)*m(4,2) - m(1,4)*m(3,2)*m(4,3);
      b(0,2) = m(1,2)*m(2,3)*m(4,4) + m(1,3)*m(2,4)*m(4,2) + m(1,4)*m(2,2)*m(4,3) - m(1,2)*m(2,4)*m(4,3) - m(1,3)*m(2,2)*m(4,4) - m(1,4)*m(2,3)*m(4,2);
      b(0,3) = m(1,2)*m(2,4)*m(3,3) + m(1,3)*m(2,2)*m(3,4) + m(1,4)*m(2,3)*m(3,2) - m(1,2)*m(2,3)*m(3,4) - m(1,3)*m(2,4)*m(3,2) - m(1,4)*m(2,2)*m(3,3);
                                                                                                                                                              
      b(1,0) = m(2,1)*m(3,4)*m(4,3) + m(2,3)*m(3,1)*m(4,4) + m(2,4)*m(3,3)*m(4,1) - m(2,1)*m(3,3)*m(4,4) - m(2,3)*m(3,4)*m(4,1) - m(2,4)*m(3,1)*m(4,3);
      b(1,1) = m(1,1)*m(3,3)*m(4,4) + m(1,3)*m(3,4)*m(4,1) + m(1,4)*m(3,1)*m(4,3) - m(1,1)*m(3,4)*m(4,3) - m(1,3)*m(3,1)*m(4,4) - m(1,4)*m(3,3)*m(4,1);
      b(1,2) = m(1,1)*m(2,4)*m(4,3) + m(1,3)*m(2,1)*m(4,4) + m(1,4)*m(2,3)*m(4,1) - m(1,1)*m(2,3)*m(4,4) - m(1,3)*m(2,4)*m(4,1) - m(1,4)*m(2,1)*m(4,3);
      b(1,3) = m(1,1)*m(2,3)*m(3,4) + m(1,3)*m(2,4)*m(3,1) + m(1,4)*m(2,1)*m(3,3) - m(1,1)*m(2,4)*m(3,3) - m(1,3)*m(2,1)*m(3,4) - m(1,4)*m(2,3)*m(3,1);
                                                                                                                                                              
      b(2,0) = m(2,1)*m(3,2)*m(4,4) + m(2,2)*m(3,4)*m(4,1) + m(2,4)*m(3,1)*m(4,2) - m(2,1)*m(3,4)*m(4,2) - m(2,2)*m(3,1)*m(4,1) - m(2,4)*m(3,2)*m(4,1);
      b(2,1) = m(1,1)*m(3,4)*m(4,2) + m(1,2)*m(3,1)*m(4,4) + m(1,4)*m(3,2)*m(4,1) - m(1,1)*m(3,2)*m(4,4) - m(1,2)*m(3,4)*m(4,1) - m(1,4)*m(3,1)*m(4,2);
      b(2,2) = m(1,1)*m(2,2)*m(4,4) + m(1,2)*m(2,4)*m(4,1) + m(1,4)*m(2,1)*m(4,2) - m(1,1)*m(2,4)*m(4,2) - m(1,2)*m(2,1)*m(4,4) - m(1,4)*m(2,2)*m(4,1);
      b(2,3) = m(1,1)*m(2,4)*m(3,2) + m(1,2)*m(2,1)*m(3,4) + m(1,4)*m(2,2)*m(3,1) - m(1,1)*m(2,2)*m(3,4) - m(1,2)*m(2,4)*m(3,1) - m(1,4)*m(2,1)*m(3,2);
                                                                                                                                                              
      b(3,0) = m(2,1)*m(3,3)*m(4,2) + m(2,2)*m(3,1)*m(4,3) + m(2,3)*m(3,2)*m(4,1) - m(2,1)*m(3,2)*m(4,3) - m(2,2)*m(3,3)*m(4,1) - m(2,3)*m(3,1)*m(4,2);
      b(3,1) = m(1,1)*m(3,2)*m(4,3) + m(1,2)*m(3,3)*m(4,1) + m(1,3)*m(3,1)*m(4,2) - m(1,1)*m(3,3)*m(4,2) - m(1,2)*m(3,1)*m(4,3) - m(1,3)*m(3,2)*m(4,1);
      b(3,2) = m(1,1)*m(2,3)*m(4,2) + m(1,2)*m(2,1)*m(4,3) + m(1,3)*m(2,2)*m(4,1) - m(1,1)*m(2,2)*m(4,3) - m(1,2)*m(2,3)*m(4,1) - m(1,3)*m(2,1)*m(4,2);
      b(3,3) = m(1,1)*m(2,2)*m(3,3) + m(1,2)*m(2,3)*m(3,1) + m(1,3)*m(2,1)*m(3,2) - m(1,1)*m(2,3)*m(3,2) - m(1,2)*m(2,1)*m(3,3) - m(1,3)*m(2,2)*m(3,1);
#endif
      return b;
    }
  else
    {
      assert(!"Matrix can't be inverted");
    }
}

float
Matrix::determinat() const
{
  const Matrix& m = *this;
  return
    m(0,0)*m(1,1)*m(2,2)*m(3,3) + m(0,0)*m(1,2)*m(2,3)*m(3,1) + m(0,0)*m(1,3)*m(2,1)*m(3,2) +
    m(0,1)*m(1,0)*m(2,3)*m(3,2) + m(0,1)*m(1,2)*m(2,0)*m(3,3) + m(0,1)*m(1,3)*m(2,2)*m(3,0) +
    m(0,2)*m(1,0)*m(2,1)*m(3,3) + m(0,2)*m(1,1)*m(2,3)*m(3,0) + m(0,2)*m(1,3)*m(2,0)*m(3,1) +
    m(0,3)*m(1,0)*m(2,2)*m(3,1) + m(0,3)*m(1,1)*m(2,0)*m(3,2) + m(0,3)*m(1,2)*m(2,1)*m(3,0) -
    m(0,0)*m(1,1)*m(2,3)*m(3,2) - m(0,0)*m(1,2)*m(2,1)*m(3,3) - m(0,0)*m(1,3)*m(2,2)*m(3,1) -
    m(0,1)*m(1,0)*m(2,2)*m(3,3) - m(0,1)*m(1,2)*m(2,3)*m(3,0) - m(0,1)*m(1,3)*m(2,0)*m(3,2) -
    m(0,2)*m(1,0)*m(2,3)*m(3,1) - m(0,2)*m(1,1)*m(2,0)*m(3,3) - m(0,2)*m(1,3)*m(2,1)*m(3,0) -
    m(0,3)*m(1,0)*m(2,1)*m(3,2) - m(0,3)*m(1,1)*m(2,2)*m(3,0) - m(0,3)*m(1,2)*m(2,0)*m(3,1);
}

Matrix
Matrix::fast_inverse() const
{
  //const Matrix& m = *this;
  Matrix ret;

  assert(!"implement me");

  return ret;
}

Matrix
Matrix::transpose() const
{
  const Matrix& m = *this;
  Matrix ret;

  ret(0,0) = m(0,0);
  ret(1,0) = m(0,1);
  ret(2,0) = m(0,2);
  ret(3,0) = m(0,3);

  ret(0,1) = m(1,0);
  ret(1,1) = m(1,1);
  ret(2,1) = m(1,2);
  ret(3,1) = m(1,3);

  ret(0,2) = m(2,0);
  ret(1,2) = m(2,1);
  ret(2,2) = m(2,2);
  ret(3,2) = m(2,3);

  ret(0,3) = m(3,0);
  ret(1,3) = m(3,1);
  ret(2,3) = m(3,2);
  ret(3,3) = m(3,3);

  return ret;
}

std::ostream& operator<<(std::ostream& s, const Matrix& m)
{
  std::ios_base::fmtflags oldflags = s.flags();
  s << std::fixed << std::setprecision(3)
    << "[" << m[ 0] << ", " << m[ 4] << ", " << m[ 8] << ", " << m[12] << "\n"
    << " " << m[ 1] << ", " << m[ 5] << ", " << m[ 9] << ", " << m[13] << "\n"
    << " " << m[ 2] << ", " << m[ 6] << ", " << m[10] << ", " << m[14] << "\n"
    << " " << m[ 3] << ", " << m[ 7] << ", " << m[11] << ", " << m[15] << "]\n";
  s.flags(oldflags);
  return s;
}

/* EOF */

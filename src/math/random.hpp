/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_WINDSTILLE_MATH_RANDOM_HPP
#define HEADER_WINDSTILLE_MATH_RANDOM_HPP

/** */
class Random
{
private:
  /* Period parameters */
  static const unsigned long N = 624;
  static const unsigned long M = 397;
  static const unsigned long MATRIX_A   = 0x9908b0dfUL;   /* constant vector a */
  static const unsigned long UPPER_MASK = 0x80000000UL; /* most significant w-r bits */
  static const unsigned long LOWER_MASK = 0x7fffffffUL; /* least significant r bits */

  unsigned long mt[N]; /* the array for the state vector  */
  unsigned long mti;

public:
  Random(unsigned long seed = 5489UL);

  /** generates a random number on [0,0xffffffff]-interval */
  unsigned long rand();

  long rand(long range);
  long rand(int start, int end);

  /** Returns float random number between [start, end] */
  double drand();

  double drand(double range);

  /** Returns a random number between \a start and \a end */
  double drand(double start, double end);

  float frand(float start, float end);
  float frand(float range);
  float frand();

  /** Returns either 1 or -1 */
  int sign();
private:
  Random (const Random&);
  Random& operator= (const Random&);
};

extern Random rnd;

#endif

/* EOF */

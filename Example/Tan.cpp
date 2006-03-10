/* -----------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-06 Bradley M. Bell

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
------------------------------------------------------------------------ */

/*
$begin Tan.cpp$$
$spell
	tan
	tan
$$

$section The AD tan Function: Example and Test$$

$index tan, AD example$$
$index example, AD tan$$
$index test, AD tan$$

$code
$verbatim%Example/Tan.cpp%0%// BEGIN PROGRAM%// END PROGRAM%1%$$
$$

$end
*/
// BEGIN PROGRAM

# include <CppAD/CppAD.h>
# include <cmath>

bool Tan(void)
{	bool ok = true;

	using CppAD::AD;
	using CppAD::NearEqual;

	// declare independent variables and start tape recording
	size_t n  = 1;
	double x0 = 0.5;
	CppADvector< AD<double> > x(n);
	x[0]      = x0;
	CppAD::Independent(x);

	// dependent variable vector 
	size_t m = 1;
	CppADvector< AD<double> > y(m);
	y[0] = CppAD::tan(x[0]);

	// create f: x -> y and stop tape recording
	CppAD::ADFun<double> f(x, y); 

	// check value 
	double check = std::sin(x0) / std::cos(x0);
	ok &= NearEqual(y[0] , check,  1e-10 , 1e-10);

	// forward computation of first partial w.r.t. x[0]
	CppADvector<double> dx(n);
	CppADvector<double> dy(m);
	dx[0] = 1.;
	dy    = f.Forward(1, dx);
	check = 1. / (std::cos(x0) * std::cos(x0));
	ok   &= NearEqual(dy[0], check, 1e-10, 1e-10);

	// reverse computation of derivative of y[0]
	CppADvector<double> w(m);
	w[0]  = 1.;
	dx    = f.Reverse(1, w);
	ok   &= NearEqual(dx[0], check, 1e-10, 1e-10);

	return ok;
}

// END PROGRAM
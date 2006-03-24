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
$begin Piecewise.cpp$$

$section Piecewise Linear Interpolation: Example and Test$$

$index piecewise, linear interpolate$$
$index linear, piecewise interpolate$$
$index interpolate, piecewise linear$$

$code
$verbatim%Example/Piecewise.cpp%0%// BEGIN PROGRAM%// END PROGRAM%1%$$
$$

$end
*/
// BEGIN PROGRAM
# include <CppAD/CppAD.h>
# include <cassert>
# include <cmath>

namespace {
	double ArgumentValue[] = {
		.0 ,
		.2 ,
		.4 ,
		.8 ,
		1.
	};
	double FunctionValue[] = {
		std::sin( ArgumentValue[0] ) ,
		std::sin( ArgumentValue[1] ) ,
		std::sin( ArgumentValue[2] ) ,
		std::sin( ArgumentValue[3] ) ,
		std::sin( ArgumentValue[4] )
	};
	size_t TableLength = 5;

	size_t Index(const double &x)
	{	// determine the index j such that x is between
		// ArgumentValue[j] and ArgumentValue[j+1] 
		static size_t j = 0;
		while ( x < ArgumentValue[j] && j > 0 )
			j--;
		while ( x > ArgumentValue[j+1] && j < TableLength - 1)
			j++;

		assert( j > 0 && j < TableLength - 1 );
		return j;
	}

	double Argument(const double &x)
	{	size_t j = Index(x);
		return ArgumentValue[j];
	}
	double Function(const double &x)
	{	size_t j = Index(x);
		return FunctionValue[j];
	}

	double Slope(const double &x)
	{	size_t j  = Index(x);
		double dx = ArgumentValue[j+1] - ArgumentValue[j];
		double dy = FunctionValue[j+1] - FunctionValue[j];
		return dy / dx;
	}
	CppADCreateDiscrete(double, Argument)
	CppADCreateDiscrete(double, Function)
	CppADCreateDiscrete(double, Slope)
}


bool Piecewise(void)
{	bool ok = true;

	using CppAD::AD;
	using CppAD::NearEqual;

	// declare independent variable vector and start recording
	size_t n = 1;
	CppADvector< AD<double> > X(n);
	X[0] = .4 * ArgumentValue[1] + .6 * ArgumentValue[2];
	CppAD::Independent(X);

	// evaluate piecewise linear interpolant at X[0]
	AD<double> A = Argument(X[0]); 
	AD<double> F = Function(X[0]);
	AD<double> S = Slope(X[0]);
	AD<double> I = F + (X[0] - A) * S;

	// dependent variable vector
	size_t m = 1;
	CppADvector< AD<double> > Y(m);
	Y[0] = I;

	// define f: X -> Y and stop tape recording
	CppAD::ADFun<double> f(X, Y);

	// vectors for arguments to the fucntion object f
	CppADvector<double> x(n);   // argument values
	CppADvector<double> y(m);   // function values 
	CppADvector<double> dx(n);  // differentials in x space
	CppADvector<double> dy(m);  // differentials in y space

	// to check function value we use the fact that X[0] is between 
	// ArgumentValue[1] and ArgumentValue[2]
	x[0]          = Value(X[0]);
	double delta  = ArgumentValue[2] - ArgumentValue[1];
	double check  = FunctionValue[2] * (x[0] - ArgumentValue[1]) / delta
	              + FunctionValue[1] * (ArgumentValue[2] - x[0]) / delta; 
	ok  &= NearEqual(Y[0], check, 1e-10, 1e-10);

	// evaluate f where x has different value
	x[0]   = .7 * ArgumentValue[2] + .3 * ArgumentValue[3];
	y      = f.Forward(0, x);

	// check function value 
	delta  = ArgumentValue[3] - ArgumentValue[2];
	check  = FunctionValue[3] * (x[0] - ArgumentValue[2]) / delta
	              + FunctionValue[2] * (ArgumentValue[3] - x[0]) / delta; 
	ok  &= NearEqual(y[0], check, 1e-10, 1e-10);

	// evaluate partials w.r.t. x[0] 
	dx[0] = 1.;
	dy    = f.Forward(1, dx);

	// check that the derivative is the slope
	check = (FunctionValue[3] - FunctionValue[2])
	      / (ArgumentValue[3] - ArgumentValue[2]);
	ok   &= NearEqual(dy[0], check, 1e-10, 1e-10);

	return ok;
}

// END PROGRAM
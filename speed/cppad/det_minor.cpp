/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-06 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin cppad_det_minor.cpp$$
$spell
	vector Vector
	typedef
	cppad
	Lu
	CppAD
	det
	hpp
	const
	CppADvector
	bool
	srand
$$

$section CppAD Speed: Gradient of Determinant Using Expansion by Minors$$

$index cppad, speed minor$$
$index speed, cppad minor$$
$index minor, speed cppad$$

$head compute_det_minor$$
$index compute_det_minor$$
Routine that computes the gradient of determinant using CppAD:
$codep */
# include <cppad/cppad.hpp>
# include <speed/det_by_minor.hpp>
# include <speed/uniform_01.hpp>

void compute_det_minor(
	size_t                     size     , 
	size_t                     repeat   , 
	CppADvector<double>       &matrix   ,
	CppADvector<double>       &gradient )
{
	// -----------------------------------------------------
	// setup
	using CppAD::AD;
	typedef AD<double>          Scalar; 
	typedef CppADvector<Scalar> Vector; 

	// object for computing determinant
	CppAD::det_by_minor<Scalar>    Det(size);

	// number of elements in matrix
	size_t length = size * size;

	// domain space vector
	Vector   A( size * size );

	// range space vector
	Vector            detA(1);
	
	// vectors of reverse mode weights 
	CppADvector<double> w(1);
	w[0] = 1.;

	size_t i;
	// ------------------------------------------------------

	while(repeat--)
	{	// get the next matrix
		CppAD::uniform_01(length, matrix);
		for( i = 0; i < size * size; i++)
			A[i] = matrix[i];

		// declare independent variables
		Independent(A);

		// compute the determinant
		detA[0] = Det(A);

		// create function object f : A -> detA
		CppAD::ADFun<double> f(A, detA);

		// evaluate and return gradient using reverse mode
		gradient = f.Reverse(1, w);
	}
	return;
}
/* $$

$head correct_det_minor$$
$index correct_det_minor$$
Routine that tests the correctness of the result computed by compute_det_minor:
$codep */
# include <speed/det_grad_33.hpp>

bool correct_det_minor(void)
{	size_t size   = 3;
	size_t repeat = 1;
	CppADvector<double> matrix(size * size);
	CppADvector<double> gradient(size * size);

	compute_det_minor(size, repeat, matrix, gradient);

	bool ok = det_grad_33(matrix, gradient);
	return ok;
}
/* $$

$head speed_det_minor$$
$index speed_det_minor$$
Routine that links compute_det_minor to $cref/speed_test/$$:

$codep */
void speed_det_minor(size_t size, size_t repeat)
{	CppADvector<double> matrix(size * size);
	CppADvector<double> gradient(size * size);

	compute_det_minor(size, repeat, matrix, gradient);
	
	return;
}
/* $$
$end
*/

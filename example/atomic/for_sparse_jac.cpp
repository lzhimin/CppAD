// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-16 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin atomic_for_sparse_jac.cpp$$

$section Atomic Operation Forward Jacobian Sparsity: Example and Test$$
$mindex sparsity$$

$head Purpose$$
This example demonstrates calculation of the forward Jacobian sparsity pattern
for an atomic operation.

$head function$$
For this example, the atomic function
$latex f : \B{R}^2 \rightarrow \B{R}^2$$ is defined by
$latex \[
f( x_0, x_1) = \left( \begin{array}{c}
	x_0 * x_0 \\
	x_0 * x_1
\end{array} \right)
\] $$

$nospell

$head Start  Class Definition$$
$srccode%cpp% */
# include <cppad/cppad.hpp>
namespace {          // isolate items below to this file
using CppAD::vector; // abbreviate as vector
//
class atomic_for_sparse_jac : public CppAD::atomic_base<double> {
/* %$$
$head Constructor $$
$srccode%cpp% */
	public:
	// constructor (could use const char* for name)
	atomic_for_sparse_jac(const std::string& name) :
	CppAD::atomic_base<double>(name)
	{ }
	private:
/* %$$
$head forward$$
$srccode%cpp% */
	// forward mode routine called by CppAD
	virtual bool forward(
		size_t                    p ,
		size_t                    q ,
		const vector<bool>&      vx ,
		vector<bool>&            vy ,
		const vector<double>&    tx ,
		vector<double>&          ty
	)
	{	size_t n = tx.size() / (q + 1);
		size_t m = ty.size() / (q + 1);
		assert( n == 2 );
		assert( m == 2 );

		// return flag
		bool ok = q == 0;
		if( ! ok )
			return ok;

		// check for defining variable information
		// This case must always be implemented
		if( vx.size() > 0 )
		{	vy[0] = vx[0];
			vy[1] = vx[0] || vy[0];
		}

		// Order zero forward mode.
		// This case must always be implemented
		// f(x) = [ x_0 * x_0 ]
		//        [ x_0 * x_1 ]
		assert( p <= 0 );
		if( p <= 0 )
		{	ty[0] = tx[0] * tx[0];
			ty[1] = tx[0] * tx[1];
		}
		return ok;
	}
/* %$$
$head for_sparse_jac$$
$srccode%cpp% */
	// forward Jacobian sparsity routine called by CppAD
	virtual bool for_sparse_jac(
		size_t                     q ,
		const CppAD::vectorBool&   r ,
		CppAD::vectorBool&         s )
	{	// This function needed becasue we are using ForSparseJac
		// with afun.option( CppAD::atomic_base<double>::pack_sparsity_enum )
		size_t n = r.size() / q;
		size_t m = s.size() / q;
		assert( n == 2 );
		assert( m == 2 );

		// f'(x) = [ 2 * x_0 ,   0 ]
		//         [     x_1 , x_0 ]

		// sparsity for first row of S(x) = f'(x) * R
		size_t i = 0;
		for(size_t j = 0; j < q; j++)
			s[ i * q + j ] = r[ 0 * q + j ];

		// sparsity for second row of S(x) = f'(x) * R
		i = 1;
		for(size_t j = 0; j < q; j++)
			s[ i * q + j ] = r[ 0 * q + j ] | r[ 1 * q + j];

		return true;
	}
}; // End of atomic_for_sparse_jac class

/* %$$
$head Use Atomic Function$$
$srccode%cpp% */
bool use_atomic_for_sparse_jac(bool x_1_variable)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	double eps = 10. * CppAD::numeric_limits<double>::epsilon();
	//
	// Create the atomic for_sparse_jac object
	atomic_for_sparse_jac afun("atomic_for_sparse_jac");
	//
	// Create the function f(u)
	//
	// domain space vector
	size_t n  = 2;
	double x_0 = 0.75;
	double x_1 = 2.00;
	vector< AD<double> > au(n);
	au[0] = x_0;
	au[1] = x_1;

	// declare independent variables and start tape recording
	CppAD::Independent(au);

	// range space vector
	size_t m = 2;
	vector< AD<double> > ay(m);

	// call user function
	vector< AD<double> > ax(n);
	ax[0] = au[0];
	if( x_1_variable )
		ax[1] = au[1];
	else
		ax[1] = x_1;
	afun(ax, ay);        // y = [ x[0] * x[0] ,  x[0] * x[1] ] ^T

	// create f: u -> y and stop tape recording
	CppAD::ADFun<double> f;
	f.Dependent (au, ay);  // f(u) = y
	//
	// check function value
	double check = x_0 * x_0;
	ok &= NearEqual( Value(ay[0]) , check,  eps, eps);
	check = x_0 * x_1;
	ok &= NearEqual( Value(ay[1]) , check,  eps, eps);

	// check zero order forward mode
	size_t q;
	vector<double> xq(n), yq(m);
	q     = 0;
	xq[0] = x_0;
	xq[1] = x_1;
	yq    = f.Forward(q, xq);
	check = x_0 * x_0;
	ok &= NearEqual(yq[0] , check,  eps, eps);
	check = x_0 * x_1;
	ok &= NearEqual(yq[1] , check,  eps, eps);

	// forward sparse Jacobian
	afun.option( CppAD::atomic_base<double>::pack_sparsity_enum );
	CppAD::vectorBool r(n * n), s(m * n);
	// r = identity matrix
	for(size_t i = 0; i < n; i++)
		for(size_t j = 0; j < n; j++)
			r[ i * n + j] = i == j;
	s = f.ForSparseJac(n, r);

	// check result
	CppAD::vectorBool check_s(m * n);
	check_s[ 0 * n + 0 ] = true;
	check_s[ 0 * n + 1 ] = false;
	check_s[ 1 * n + 0 ] = true;
	check_s[ 1 * n + 1 ] = x_1_variable;
	for(size_t i = 0; i < m * n; i++)
		ok &= s[ i ] == check_s[ i ];
	//
	return ok;
}
}  // End empty namespace
/* %$$
$head Test with x_1 Both a Variable and a Parameter$$
$srccode%cpp% */
bool for_sparse_jac(void)
{	bool ok = true;
	// test with x_1 a variable
	ok     &= use_atomic_for_sparse_jac(true);
	// test with x_1 a parameter
	ok     &= use_atomic_for_sparse_jac(false);
	return ok;
}
/* %$$
$$ $comment end nospell$$
$end
*/
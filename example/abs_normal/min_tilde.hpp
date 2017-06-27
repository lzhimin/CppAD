# ifndef CPPAD_EXAMPLE_ABS_NORMAL_MIN_TILDE_HPP
# define CPPAD_EXAMPLE_ABS_NORMAL_MIN_TILDE_HPP
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-17 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin min_tilde$$
$spell
	jac
	Jacobian
	maxitr
	qp
$$
$section Minimize Abs-normal First Order Approximation$$

$head Under Construction$$

$head Syntax$$
$icode%ok% = min_tilde(
	%level%, %n%, %m%, %s%,
	%g_hat%, %g_jac%, %bound%, %epsilon%, %maxitr%, %delta_x%
)%$$
$pre
$$
see $cref/prototype/qp_box/Prototype/$$

$head Purpose$$
Given a current that abs-normal representation at a point
$latex \hat{x} \in \B{R}^n$$,
and a $latex \Delta x \in \B{R}^n$$,
this routine minimizes the abs-normal
$cref/approximation for f(x)
	/abs_normal_fun
	/Abs-normal Approximation
	/Approximating f(x)
/$$
over a trust region.

$head Cutting Plane Method$$
The algorithm used is a cutting plane method which is only guaranteed
to work when the objective is convex.

$head DblVector$$
is a $cref SimpleVector$$ class with elements of type $code double$$.

$head SizeVector$$
is a $cref SimpleVector$$ class with elements of type $code size_t$$.

$head f$$
We use the notation $icode f$$ for the original function; see
$cref/f/abs_normal_fun/f/$$.

$head level$$
This value is less that or equal three.
If $icode%level% == 0%$$,
no tracing of the optimization is printed.
If $icode%level% >= 1%$$,
a trace of each iteration of $code min_tilde$$ is printed.
If $icode%level% >= 2%$$,
a trace of the $cref qp_box$$ sub-problem is printed.
If $icode%level% == 3%$$,
a trace of the $cref qp_interior$$ sub-problem is printed.

$head n$$
This is the dimension of the domain space for $icode f$$; see
$cref/n/abs_normal_fun/f/n/$$.

$head m$$
This is the dimension of the range space for $icode f$$; see
$cref/m/abs_normal_fun/f/m/$$. This must be one so that $latex f$$
is an objective function.

$head s$$
This is the number of absolute value terms in $icode f$$; see

$head g$$
We use the notation $icode g$$ for the abs-normal representation of $icode f$$;
see $cref/g/abs_normal_fun/g/$$.

$head g_hat$$
This vector has size $icode%m% + %s%$$ and is the value of
$icode g(x, u)$$ at $latex x = \hat{x}$$ and $latex u = a( \hat{x} )$$.

$head g_jac$$
This vector has size $codei%(%m% + %s%) * (%n% + %s%)%$$ and is the Jacobian of
$latex g(x, u)$$ at $latex x = \hat{x}$$ and $latex u = a( \hat{x} )$$.

$head bound$$
This vector has size $icode n$$
and we denote its value by $latex b \in \B{R}^n$$.
The trust region is defined as the set of $latex x$$ such that
$latex \[
	| x_j - \hat{x}_j | \leq b_j
\]$$
for $latex j = 0 , \ldots , n-1$$,
where $latex x$$ is the point that we are approximating $latex f(x)$$.

$head epsilon$$
This is the convergence criteria for the optimal solution in terms
of the infinity norm for $latex x$$.

$head maxitr$$
This is a vector with size greater than or equal 2.
The value $icode%maxitr%[0]%$$ is the maximum number of
$code min_tilde$$ iterations to try before giving up on convergence.
The value $icode%maxitr%[1]%$$ is the maximum number of iterations in
the $cref/qp_interior/qp_interior/maxitr/$$ sub-problems.


$head delta_x$$
This vector $latex \Delta x$$ has size $icode n$$.
It is the approximate minimizer
of the abs-normal approximation for $latex f(x)$$ over the trust region
is $latex x = \hat{x} + \Delta x$$.

$children%example/abs_normal/min_tilde.cpp
%$$
$head Example$$
The file $cref min_tilde.cpp$$ contains an example and test of
$code min_tilde$$.
It returns true if the test passes and false otherwise.

$head Prototype$$
$srcfile%example/abs_normal/min_tilde.hpp%
	0%// BEGIN PROTOTYPE%// END PROTOTYPE%
1%$$

$end
-----------------------------------------------------------------------------
*/
# include <cppad/cppad.hpp>
# include "qp_box.hpp"
# include "eval_tilde.hpp"

namespace CppAD { // BEGIN_CPPAD_NAMESPACE

// BEGIN PROTOTYPE
template <class DblVector, class SizeVector>
bool min_tilde(
	size_t           level   ,
	size_t           n       ,
	size_t           m       ,
	size_t           s       ,
	const DblVector& g_hat   ,
	const DblVector& g_jac   ,
	const DblVector& bound   ,
	double           epsilon ,
	SizeVector       maxitr  ,
	DblVector&       delta_x )
// END PROTOTYPE
{	using std::fabs;
	bool ok = true;
	//
	CPPAD_ASSERT_KNOWN(
		level <= 3,
		"min_tilde: level is not less that or equal 3"
	);
	CPPAD_ASSERT_KNOWN(
		maxitr.size() == 2,
		"min_tilde: size of maxitr not equal to 2"
	);
	CPPAD_ASSERT_KNOWN(
		m == 1,
		"min_tilde: m is not equal to 1"
	);
	CPPAD_ASSERT_KNOWN(
		delta_x.size() == n,
		"min_tilde: size of delta_x not equal to n"
	);
	CPPAD_ASSERT_KNOWN(
		bound.size() == n,
		"min_tilde: size of bound not equal to n"
	);
	CPPAD_ASSERT_KNOWN(
		g_hat.size() == m + s,
		"min_tilde: size of g_hat not equal to m + s"
	);
	CPPAD_ASSERT_KNOWN(
		g_jac.size() == (m + s) * (n + s),
		"min_tilde: size of g_jac not equal to (m + s)*(n + s)"
	);
	CPPAD_ASSERT_KNOWN(
		bound.size() == n,
		"min_tilde: size of bound is not equal to n"
	);
	if( level > 0 )
	{	std::cout << "start min_tilde\n";
		CppAD::abs_normal_print_mat("bound", n, 1, bound);
		CppAD::abs_normal_print_mat("g_hat", m + s, 1, g_hat);
		CppAD::abs_normal_print_mat("g_jac", m + s, n + s, g_jac);

	}
	// partial y(x, u) w.r.t x (J in reference)
	DblVector py_px(n);
	for(size_t j = 0; j < n; j++)
		py_px[ j ] = g_jac[ j ];
	//
	// partial y(x, u) w.r.t u (Y in reference)
	DblVector py_pu(s);
	for(size_t j = 0; j < s; j++)
		py_pu[ j ] = g_jac[ n + j ];
	//
	// partial z(x, u) w.r.t x (Z in reference)
	DblVector pz_px(s * n);
	for(size_t i = 0; i < s; i++)
	{	for(size_t j = 0; j < n; j++)
		{	pz_px[ i * n + j ] = g_jac[ (n + s) * (i + m) + j ];
		}
	}
	// partial z(x, u) w.r.t u (L in reference)
	DblVector pz_pu(s * s);
	for(size_t i = 0; i < s; i++)
	{	for(size_t j = 0; j < s; j++)
		{	pz_pu[ i * s + j ] = g_jac[ (n + s) * (i + m) + n + j ];
		}
	}
	// initailize delta_x
	for(size_t j = 0; j < n; j++)
		delta_x[j] = 0.0;
	//
	// value of approximation for g(x, u) at current delta_x
	DblVector g_tilde = CppAD::eval_tilde(n, m, s, g_hat, g_jac, delta_x);
	//
	// value of sigma at current delta_x; i.e., sign( z(x, u) )
	CppAD::vector<int> sigma(s);
	for(size_t i = 0; i < s; i++)
		sigma[i] = CppAD::sign( g_tilde[m + i] );
	//
	// current set of cutting planes
	DblVector C(maxitr[0] * n), c(maxitr[0]);
	//
	// maximum absolute value in C
	double max_element = 0.0;
	//
	// maximum bound
	double max_bound = 0.0;
	for(size_t j = 0; j < n; j++)
		max_bound = std::max(max_bound, std::fabs(bound[j]));
	//
	size_t n_plane = 0;
	for(size_t itr = 0; itr < maxitr[0]; itr++)
	{
		// Equation (5), Propostion 3.1 of reference
		// dy_dx = py_px + py_pu * Sigma * (I - pz_pu * Sigma)^-1 * pz_px
		//
		// tmp_ss = I - pz_pu * Sigma
		DblVector tmp_ss(s * s);
		for(size_t i = 0; i < s; i++)
		{	for(size_t j = 0; j < s; j++)
				tmp_ss[i * s + j] = - pz_pu[i * s + j] * sigma[j];
			tmp_ss[i * s + i] += 1.0;
		}
		// tmp_sn = (I - pz_pu * Sigma)^-1 * pz_px
		double logdet;
		DblVector tmp_sn(s * n);
		LuSolve(s, n, tmp_ss, pz_px, tmp_sn, logdet);
		//
		// tmp_sn = Sigma * (I - pz_pu * Sigma)^-1 * pz_px
		for(size_t i = 0; i < s; i++)
		{	for(size_t j = 0; j < n; j++)
				tmp_sn[i * n + j] *= sigma[i];
		}
		// dy_dx = py_px + py_pu * Sigma * (I - pz_pu * Sigma)^-1 * pz_px
		DblVector dy_dx(n);
		for(size_t j = 0; j < n; j++)
		{	dy_dx[j] = py_px[j];
			for(size_t k = 0; k < s; k++)
				dy_dx[j] += py_pu[k] * tmp_sn[ k * n + j];
		}
		// value of hyperplane at delta_x
		double plane_at_zero = g_tilde[0];
		// value of hyperplane at 0
		for(size_t j = 0; j < n; j++)
			plane_at_zero -= dy_dx[j] * delta_x[j];
		//
		// add a cutting plane with value g_tilde[0] at delta_x
		// and derivative dy_dx
		c[n_plane] = plane_at_zero;
		for(size_t j = 0; j < n; j++)
		{	C[n_plane * n + j] = dy_dx[j];
			max_element = std::max( max_element, std::fabs( dy_dx[j] ) );
		}
		++n_plane;
		//
		// variables for cutting plane problem are (dx, w)
		// c[i] + C[i,:]*dx <= w
		DblVector c_box(n_plane), C_box(n_plane * (n + 1));
		for(size_t i = 0; i < n_plane; i++)
		{	c_box[i] = c[i];
			for(size_t j = 0; j < n; j++)
				C_box[i * (n+1) + j] = C[i * n + j];
			C_box[i *(n+1) + n] = -1.0;
		}
		// w is the objective
		DblVector g_box(n + 1), G_box((n+1) * (n+1));
		for(size_t i = 0; i < g_box.size(); i++)
			g_box[i] = 0.0;
		g_box[n] = 1.0;
		for(size_t i = 0; i < G_box.size(); i++)
			G_box[i] = 0.0;
		//
		// xin_box
		double win = - std::numeric_limits<double>::infinity();
		for(size_t i = 0; i < n_plane; i++)
			win = std::max(win, c[i]);
		win = win + max_element;
		DblVector xin_box(n + 1);
		for(size_t j = 0; j < n; j++)
			xin_box[j] = 0.0;
		xin_box[n] = win;
		//
		// a_box and b_box
		DblVector a_box(n+1), b_box(n+1);
		for(size_t j = 0; j < n; j++)
		{	a_box[j] = - bound[j];
			b_box[j] = + bound[j];
		}
		a_box[n] = g_tilde[0] - n * max_element * max_bound;
		b_box[n] = win + max_element;
		//
		// solve the cutting plane problem
		DblVector xout_box(n + 1);
		double eps = 99.0 * std::numeric_limits<double>::epsilon();
		size_t level_box = 0;
		if( level > 0 )
			level_box = level - 1;
		ok &= CppAD::qp_box(
			level_box,
			a_box,
			b_box,
			c_box,
			C_box,
			g_box,
			G_box,
			eps,
			maxitr[1],
			xin_box,
			xout_box
		);
		if( ! ok )
		{	if( level > 0 )
			{	CppAD::abs_normal_print_mat("delta_x", n, 1, delta_x);
				std::cout << "end min_tilde: ok = false\n";
			}
			return false;
		}
		//
		// check for convergence
		double max_diff = 0.0;
		for(size_t j = 0; j < n; j++)
		{	double diff = delta_x[j] - xout_box[j];
			max_diff    = std::max( max_diff, std::fabs(diff) );
		}
		//
		// check for descent in value of approximation objective
		DblVector delta_new(n);
		for(size_t j = 0; j < n; j++)
			delta_new[j] = xout_box[j];
		DblVector g_new = CppAD::eval_tilde(n, m, s, g_hat, g_jac, delta_new);
		if( g_new[0] < g_tilde[0] )
		{	g_tilde = g_new;
			delta_x = delta_new;
		}
		if( level > 0 )
		{	std::cout << "itr = " << itr << ", max_diff = " << max_diff
				<< ", y_cur = " << g_tilde[0] << ", y_new = " << g_new[0]
				<< "\n";
			CppAD::abs_normal_print_mat("delta_new", n, 1, delta_new);
		}
		if( max_diff < epsilon )
		{	if( level > 0 )
			{	CppAD::abs_normal_print_mat("delta_x", n, 1, delta_x);
				std::cout << "end min_tilde: ok = true\n";
			}
			return true;
		}
	}
	if( level > 0 )
	{	CppAD::abs_normal_print_mat("delta_x", n, 1, delta_x);
		std::cout << "end min_tilde: ok = false\n";
	}
	return false;
}
} // END_CPPAD_NAMESPACE

# endif
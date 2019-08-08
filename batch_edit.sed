# ----------------------------------------------------------------------------
# None of the lists below can have white space or a dollar sign in an entry.
#
# list of directories that are added to the repository by batch_edit.sh
# new_directories='
#   include/cppad/example/atomic_two
# '
# list of files that are deleted by batch_edit.sh
# delete_files='
# '
# List of files that are not edited by the sed commands in this file
# (with the possible exception of the extra_seds commands).
# The files in bin/devel.sh ignore_files are automatically in this list.
# ignore_files='
# '
# list of files and or directories that are moved to new names
# move_paths='
#   include/cppad/example/eigen_cholesky.hpp
#   include/cppad/example/eigen_mat_inv.hpp
#   include/cppad/example/eigen_mat_mul.hpp
# '
# list of sed commands that map old file and directory names to new names.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# move_seds='
#   s|/example/|/example//atomic_two/|
# '
# list of files that get edited by the extra_seds command
# extra_files='
# '
# list of sed commands that are applied to the extra files,
# after the other sed commands in this file.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# extra_seds='
# '
# ----------------------------------------------------------------------------
# Put other sed commands below here and without # at start of line
s|cppad/example/eigen_cholesky.hpp|cppad/example/atomic_two/eigen_cholesky.hpp|
s|cppad/example/eigen_mat_inv.hpp|cppad/example/atomic_two/eigen_mat_inv.hpp|
s|cppad/example/eigen_mat_mul.hpp|cppad/example/atomic_two/eigen_mat_mul.hpp|
#
s|CPPAD_EXAMPLE_EIGEN_MAT_MUL_HPP|CPPAD_EXAMPLE_ATOMIC_TWO_EIGEN_MAT_MUL_HPP|
s|CPPAD_EXAMPLE_EIGEN_MAT_INV_HPP|CPPAD_EXAMPLE_ATOMIC_TWO_EIGEN_MAT_INV_HPP|
s|CPPAD_EXAMPLE_EIGEN_CHOLESKY_HPP|CPPAD_EXAMPLE_ATOMIC_TWO_EIGEN_CHOLESKY_HPP|
#
s|atomic_eigen_cholesky.hpp|atomic_two_eigen_cholesky.hpp|
s|atomic_eigen_mat_inv.hpp|atomic_two_eigen_mat_inv.hpp|
s|atomic_eigen_mat_mul.hpp|atomic_two_eigen_mat_mul.hpp|

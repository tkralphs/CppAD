#! /bin/bash -e
# $Id$
# -----------------------------------------------------------------------------
# CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell
#
# CppAD is distributed under multiple licenses. This distribution is under
# the terms of the 
#                     Eclipse Public License Version 1.0.
#
# A copy of this license is included in the COPYING file of this distribution.
# Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
# -----------------------------------------------------------------------------
if [ $0 != "bin/check_copyright.sh" ]
then
	echo "bin/check_copyright.sh: must be executed from its parent directory"
	exit 1
fi
if [ -d '.git' ]
then
	scc='git'
elif [ -d '.svn' ]
then
	scc='svn'
else
	echo 'check_copright.sh: cannot find .git or .svn'
	exit 1
fi
if ! ${scc}_commit.sh list > /dev/null
then
	${scc}_commit.sh list
fi
list=`${scc}_commit.sh list | sed -e '/makefile.in$/d'`
cat << EOF > check_copyright.1.$$
# Change copyright second year to current year
s/Copyright (C) \\([0-9]*\\)-[0-9][0-9] Bradley M. Bell/Copyright (C) \\1-15 Bradley M. Bell/
s/Copyright (C) \\([0-9]*\\)-20[0-9][0-9] Bradley M. Bell/Copyright (C) \\1-15 Bradley M. Bell/
EOF
ok=true
for file in $list
do
	if [ -e $file ]
	then
		sed -f check_copyright.1.$$ $file > check_copyright.2.$$
		if ! diff $file check_copyright.2.$$ > /dev/null
		then
			echo '----------------------------------------------------'
			echo "check_copyright.sh: automatic changes to $file:"
			if diff $file check_copyright.2.$$
			then
				echo 'bin/check_copyright.sh: program error'
				rm check_copyright.*.$$
				exit 1
			fi
			ok='no'
			if [ -x $file ]
			then
				mv check_copyright.2.$$ $file
				chmod +x $file
			else
				mv check_copyright.2.$$ $file
			fi
		fi
	fi
done
if [ "$ok" == 'no' ]
then
	rm check_copyright.*.$$
	exit 1
fi
rm check_copyright.*.$$
echo 'check_copyright.sh: OK'
exit 0

proc max {a b} { if {$b > $a} { return $b } else { return $a } }

# Compute the edit distance/script to change string M into N.
#
# Implements Myers et al. O(NP) LCS Algorithm with edit-script storage, ca. 1989.
# NOTE: The original algorithm used 1-based strings (the first element of the
# string was referenced with 1).  This implementation has been changed to use
# 0-based strings as are natively found in TCL and many other languages.
proc diff {m n {a +} {r -}} {
	set M		[string length $m]
	set N		[string length $n]
	set D		[expr {$N - $M}]

	if {$D < 0} {	;# N < M
		# The algorithm will only work if N >= M.  To preserve this
		# requirement for correct operation, reverse the order of the input
		# strings and the meaning of the corresponding operations
		return [diff $n $m $r $a]
	}

	array set edit	[list]		;# a list of edit possible scripts ({+,-,=}*)

	array set fp [list]

	set p -1	;# p is the maximum distance from the diagonal that we've tried
	while {1} {
		incr p
		for {set k [expr { 0 - $p}]} {$k < $D} {incr k  1} {
			set fp($k) [snake $k]
		}
		for {set k [expr {$D + $p}]} {$k > $D} {incr k -1} {
			set fp($k) [snake $k]
		}

		if {[set fp($D) [snake $D]] == $N} {
			break
		}
	}

	# Return the number of non-zero edit operations and the edit script itself:
	return	[list	[expr {$D + (2*$p)}] \
					[string range $edit($D) 1 end]]
}

proc snake {k} {
	upvar 1 fp fp M M N N m m n n r r a a edit edit

	# NOTE: The calculation of y has been moved out of the loop of the original
	# expression of the algorithm.
	set {k-1} [expr {$k - 1}]	;# This is also used by [append-diff-to-edit]
	set {k+1} [expr {$k + 1}]	;# This is also used by [append-diff-to-edit]

	# In the best/typical case of two strings with few differences, a
	# significant part of a linear "fp" array goes completely unused (no reads
	# or writes).  Since we are using hash-tables for this anyway, we don't need
	# the array initialization step as long as we check to see if the element is
	# defined here before retrieving it.  If performance becomes a problem we
	# can add the initialization back in and use try a list representation
	# (which is an array in TCL...  (only for small lists?)).
	# NOTE: fp[k+/-1] are also used by append-diff-to-edit.
	if {[info exists fp(${k-1})]} { set {fp[k-1]} $fp(${k-1}) } else { set {fp[k-1]} -1 }
	if {[info exists fp(${k+1})]} { set {fp[k+1]} $fp(${k+1}) } else { set {fp[k+1]} -1 }

	# Calculate x and y in the hypothetical edit graph
	set y [max ${fp[k+1]} [expr {${fp[k-1]} + 1}]]	;# TCL 8.5+ can use [expr {max(...)}]...
	set x [expr {$y - $k}]

	append-diff-to-edit		;# append an add or remove operation to edit($k)

	while {		($x < $M)
			&&	($y < $N)
			&&	([string index $m $x]
			==	 [string index $n $y]) } {
		incr x
		incr y
		append edit($k) =	;# extend the edit($k) without add/remove
	}
	return $y
}

proc append-diff-to-edit {} {
	upvar 1 r r a a edit edit k k {k-1} {k-1} {k+1} {k+1} {fp[k-1]} s {fp[k+1]} t

	if {$s > $t} {
		if {[info exists edit(${k-1})]} {
			set edit($k) $edit(${k-1})	;# copy edit(k-1)
		}
		set o $a
	} else {
		if {[info exists edit(${k+1})]} {
			set edit($k) $edit(${k+1})	;# copy edit(k-1)
		}
		set o $r
	}
	append edit($k) $o
}

if {0} {
	# Basic unit tests:
	puts "diff {} {}			= [diff {} {}]"
	puts "diff {h} {h}			= [diff {h} {h}]"
	puts "diff {hello} {hello}	= [diff {hello} {hello}]"
	puts ""
	puts "diff {hello} {hell}	= [diff {hello} {hell}]"
	puts ""
	puts "diff {hello} {jello}	= [diff {hello} {jello}]"
	puts "diff {hello} {jell}	= [diff {hello} {jell}]"
	puts "diff {hello} {jel}	= [diff {hello} {jel}]"
	puts "diff {hello} {je}		= [diff {hello} {je}]"
	puts "diff {hello} {j}		= [diff {hello} {j}]"
	puts "diff {hello} {}		= [diff {hello} {}]"
	puts ""
	puts "diff {jello}	{hello}	= [diff {jello}	{hello}]"
	puts "diff {jell}	{hello}	= [diff {jell}	{hello}]"
	puts "diff {jel}	{hello}	= [diff {jel}	{hello}]"
	puts "diff {je}		{hello}	= [diff {je}	{hello}]"
	puts "diff {j}		{hello}	= [diff {j}		{hello}]"
	puts "diff {}		{hello}	= [diff {}		{hello}]"
}

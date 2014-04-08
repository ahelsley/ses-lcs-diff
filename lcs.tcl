proc "(" {args} {
	return [uplevel 1 [linsert [lrange $args 0 end-1] 0 expr]]
}

# Myers et. al O(NP) LCS Algorithm with edit-script storage, ca. 1989.
# NOTE: The original algorithm used 1-based strings (the first element of the
# string was referenced with 1).  This implementation has been changed to use
# 0-based strings as are natively found in TCL and many other languages.
proc diff {m n {a +} {r -}} {
	set M		[string length $m]
	set N		[string length $n]

	if {$N < $M} {
		# The algorithm will only work if N >= M.  To preserve this
		# requirement for correct operation, reverse the order of the input
		# strings and the meaning of the corresponding operations
		return [diff $n $m $r $a]
	}

	array set path	[list]
	set D		[( {$N - $M} )]
	set {D-1}	[( {$D -  1} )]
	set {D+1}	[( {$D +  1} )]

	array set fp [list]

	set p -1	;# p is the maximum distance from the diagonal that we've tried
	while {1} {
		incr p
		for {set k [( { 0 - $p} )]} {$k <= ${D-1}} {incr k  1} {
			set fp($k) [snake $k]
		}
		for {set k [( {$D + $p} )]} {$k >= ${D+1}} {incr k -1} {
			set fp($k) [snake $k]
		}

		if {[set fp($D) [snake $D]] == $N} {
			break
		}
	}

	# Return the number of non-zero edit operations and the edit script itself:
	return	[list	[( {$D + (2*$p)} )] \
					[lrange $path($D) 1 end]]
}

proc snake {k} {
	upvar 1 fp fp M M N N m m n n r r a a path path

	# This is moved out of the loop of the original expression of the algorithm.
	# Calculate x and y from k and fp[k+/-1].  Uninitialized fp(x) == -1.
	set {k-1} [incr k -1]
	set {k+1} [incr k  2]
	set k	  [incr k -1]

	# In the best case, a significant part of a linear "fp" array goes
	# completely unused (no reads or writes).  Since we are using hash-tables
	# for this anyway, we don't need the array initialization step if we check
	# to see if the element is defined here before retrieving it.  If
	# performance becomes a problem we can add the initialization back in and
	# use try a list representation (which is an array in TCL...
	# (only for small lists?)).
	if {[info exists fp(${k-1})]} { set {fp[k-1]} $fp(${k-1}) } else { set {fp[k-1]} -1 }
	if {[info exists fp(${k+1})]} { set {fp[k+1]} $fp(${k+1}) } else { set {fp[k+1]} -1 }

	set y [( max(${fp[k-1]}+1, ${fp[k+1]}) )]
	set x [( {$y - $k} )]

	append-diff-to-path		;# append an add or remove to path($k)

	while { ($x < $M)
			&& ($y < $N)
			&& ([string index $m $x] == [string index $n $y]) } {
		incr x
		incr y
		lappend path($k) 0	;# extend the path without add/remove
	}
	return $y
}

proc append-diff-to-path {} {
	upvar 1 r r a a path path k k {k-1} {k-1} {k+1} {k+1} {fp[k-1]} s {fp[k+1]} t

	if {$s > $t} {
		if {[info exists path(${k-1})]} {
			set path($k) $path(${k-1})	;# copy path(k-1)
		}
		set o $a
	} else {
		if {[info exists path(${k+1})]} {
			set path($k) $path(${k+1})	;# copy path(k-1)
		}
		set o $r
	}
	lappend path($k) $o
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

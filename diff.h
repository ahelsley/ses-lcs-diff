#ifndef DIFF_H
#	define DIFF_H
#	include <stdlib.h>
#	include <string.h>
#	include <assert.h>

#	define L1_CACHE_LINE_BYTES	16
#	define ssize_t				long

	ssize_t	max(ssize_t a, ssize_t  b) { return (a > b) ? a : b; }
	ssize_t	min(ssize_t a, ssize_t  b) { return (a < b) ? a : b; }

	// ADT for encapsulating an edit-script
	typedef struct edit_script {
		ssize_t	allocated, used;
		char	*script;
	} edit_script;
	typedef struct diff {
		size_t	n, a, r;
		char	*script;
	} diff;
	void es_resize(edit_script *script) {
		script->script = realloc(script->script, script->allocated);
		if (!script->script) abort();
	}
	void es_init(edit_script *script, ssize_t init_alloc) {
		script->used		= 0;
		script->script		= NULL;
		script->allocated	= init_alloc;
		es_resize(script);
	}
	void es_append(edit_script *script, char diff) {
		if (script->used >= script->allocated) {
			script->allocated = max(L1_CACHE_LINE_BYTES, 2*script->allocated);
			es_resize(script);
		}
		script->script[script->used++] = diff;
	}
	// 'to' must not have been used previously
	void es_copy(edit_script *to, edit_script *from) {
		to->allocated	= from->allocated;
		es_resize(to);
		memcpy(to->script, from->script, from->used);
		to->used		= from->used;
	}
	void es_free(edit_script *script) {
		if (script->script) free(script->script);
	}
#endif

// Compute the edit script to change string m into n.  Caller is responsible for
// freeing the memory for the returned script.
//
// Implements Myers et al. O(NP) LCS Algorithm ca. 1989 with edit-script storage.
// NOTE: The original algorithm used 1-based strings (the first element of the
// string was referenced with 1).  This implementation has been changed to use
// 0-based strings as are natively found in C and many other languages.
diff DIFF(TYPE *m, TYPE *n) {
	ssize_t M = LENGTH(m), N = LENGTH(n);
	char a = '+', r = '-';
	if (N < M) {
		// The algorithm will only work if N >= M.  To preserve this requirement
		// for correct operation, reverse the order of the input strings and the
		// meaning of the corresponding operations.
		// Swap m and n and the corresponding values
		{ TYPE	 *t	= m;	m = n; n = t; }	// swap sequences
		{ ssize_t t	= M;	M = N; N = t; }	// swap lengths
		{ char	  t	= a;	a = r; r = t; }	// swap operation semantics
	}

	const ssize_t D	= N - M;

	// Initialize the FP array
	// NOTE: In the best/typical case of two strings with few differences, a
	// significant part of the "fp" array goes completely unused (no reads or
	// writes).  //TODO// eliminate the fudge factor!
#	define FUDGE 8
	const ssize_t S = M+1 + N+1 +FUDGE;		// the size of the array of furtherst points
	ssize_t *fp	= malloc(S*sizeof(ssize_t));
	for(int i = 0; i < S; i++) { fp[i] = -1; }
#	define FP(x)	(fp[M+1 + x+1])

	// Allocate space for tracking edit-scripts
	edit_script *edit = calloc(S, sizeof(edit_script));	// a list of of up to k possible edit scripts ({+,-,=}*)
	if (!edit) abort();
#	define EDIT(x)	(edit[M+1 + x+1])


	// A nested function so that we don't have to constantly marshall and
	// un-marshall function parameters while allowing re-use of the same logic
	// in different parts of the parent function.
	ssize_t snake(ssize_t k) {
		// NOTE: The calculation of y has been moved out of the loop of the original
		// expression of the algorithm since it complicates the loop expressions
		// and can be done once here instead of 3 times there.
		ssize_t i = FP(k-1);
		ssize_t j = FP(k+1);

		// Calculate x and y in the hypothetical edit graph:
		ssize_t y = max(j, i + 1);
		ssize_t x = y - k;

		////////////////////////////////////////////////////////////////////////
		// Addition to the algorithm for saving the operation in an edit script
		{
			char op;
			if (i > j) {
				if (EDIT(k-1).used > 0) {
					es_copy(&EDIT(k), &EDIT(k-1));	// edit[k] := edit[k-1]
				}
				op = a;
			} else {
				if (EDIT(k+1).used > 0) {
					es_copy(&EDIT(k), &EDIT(k+1));	// edit[k] := edit[k+1]
				}
				op = r;
			}
			if (EDIT(k).allocated < 1) {
				es_init(&EDIT(k), L1_CACHE_LINE_BYTES);	// new path for k since none for k+/-1
			}
			es_append(&EDIT(k), op);				// append operation to path k
		}
		////////////////////////////////////////////////////////////////////////

		while (		(x < M)
				&&	(y < N)
				&&	(m[x] == n[y]) ) {
			x++;
			y++;
			es_append(&EDIT(k), '=');			// extend the edit[k] without add/remove
		}
		return y;
	}
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// Core of the algorithm
	ssize_t p = -1;
	do {
		p++;
		for (ssize_t k = 0 - p; k < D; k++) {
			FP(k) = snake(k);
		}
		for (ssize_t k = D + p; k > D; k--) {
			FP(k) = snake(k);
		}
	} while ((FP(D) = snake(D)) != N);
#	undef FP
	free(fp);	// Free list of furthest "points" (y coords)

	////////////////////////////////////////////////////////////////////////////
	// Copy the edit script into a pure C string:
	char *script = malloc(max(1, EDIT(D).used));
	if (!script) abort();
	if (EDIT(D).used < 2) {
		// Only 1 or fewer edit instructions => edit script is really empty.
		*script = '\0';
	} else {
		// First instruction in edit script is always '=' and is not meaningful.
		memcpy(script, 1+EDIT(D).script, EDIT(D).used-1);
		script[EDIT(D).used-1] = '\0';
	}
#	undef EDIT

	// Free partial/suboptimal scripts
	for(int i = 0; i < S; i++) {
		es_free(&edit[i]);
	}
	free(edit);	// Free list of partial/suboptimal scripts

	// Return edit distance and edit script
	diff result;
	if (a == '+') {
		result = (diff){n:D+2*p, a:D+p, r:  p, script:script};
	} else {
		result = (diff){n:D+2*p, a:  p, r:D+p, script:script};
	}
	return result;
	////////////////////////////////////////////////////////////////////////////
}

#undef DIFF
#undef TYPE
#undef LENGTH

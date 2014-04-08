#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////

#define DIFF				cdiff
#define TYPE				char
#define LENGTH(s)			strlen(s)
#include "diff.h"

#define DIFF				ldiff
#define TYPE				long
#define LENGTH(s)			(s = &s[1], (ssize_t)s[-1])
#include "diff.h"

////////////////////////////////////////////////////////////////////////////////

#define XFREE(x)			do{if(x){free(x);x=NULL;}}while(0)

#if 0
#	define ITERATIONS			10000
#	define PRINTF(format, ...)
#	define PUTS(string)
#else
#	define ITERATIONS			1
#	define PRINTF(format, ...)	printf(format, __VA_ARGS__)
#	define PUTS(string)			puts(string)
#endif

int main(int argc, char **argv) {
	// Basic unit tests:
	diff d = {n:0, a:0, r:0, script:NULL};

	for(int i = 0; i < ITERATIONS; i++) {
									PUTS(  "                       E - +");
	d = cdiff("hello",  "hello");	PRINTF("diff `hello' `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("h"       ,"h");		PRINTF("diff `h' `h'        = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("",       "");		PRINTF("diff `' `'          = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
									PUTS(  "");
	d = cdiff("hello",  "hell");	PRINTF("diff `hello' `hell' = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
									PUTS(  "");
	d = cdiff("hello",  "jello");	PRINTF("diff `hello' `jello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("hello",  "jell");	PRINTF("diff `hello' `jell' = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("hello",  "jel");		PRINTF("diff `hello' `jel'  = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("hello",  "je");		PRINTF("diff `hello' `je'   = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("hello",  "j");		PRINTF("diff `hello' `j'    = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("hello",  "");		PRINTF("diff `hello' `'     = (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
									PUTS(  "");
	d = cdiff("jello",  "hello");	PRINTF("diff `jello' `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("jell",   "hello");	PRINTF("diff `jell'  `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("jel",    "hello");	PRINTF("diff `jel'   `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("je",     "hello");	PRINTF("diff `je'    `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("j",      "hello");	PRINTF("diff `j'     `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	d = cdiff("",       "hello");	PRINTF("diff `'      `hello'= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
									PUTS(  "");
	}

									PUTS(  "                           E - +");
	{
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {5, 1, 2, 3, 3, 5};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {12335} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {4, 1, 2, 3, 3};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {1233}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {1233} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {3, 1, 2, 3};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {123}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {123} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {2, 1, 2};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {12}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {12} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {1, 1};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {1}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {1} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {0};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {5, 7, 2, 3, 3, 5};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {72335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {72335} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
		long a[] = {5, 1, 2, 3, 3, 5};
		long b[] = {4, 7, 2, 3, 3};
		d = ldiff(a, b);			PRINTF("ldiff {12335} {7233}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
		d = ldiff(b, a);			PRINTF("ldiff {7233} {12335}	= (%d=%d+%d): %s\n", d.n, d.r, d.a, d.script);	XFREE(d.script);
	} {
	}

	return 0;
}

typedef const char *	string;
typedef int				length;

void swap(string &X, string &Y, length &lX, length &lY, length &d);
length max(length a, length b);
length snake(length k, length y, string A, string B, length La, length Lb);

length edit_distance(string A, string B)
{
	// calculate string lengths and the difference in length
	length	La = strlen(A),	// the length of string A (M)
			Lb = strlen(B),	// the length of string B (N)
			Delta = Lb - La;	// diff. in length of A,B (a lower-bound of Ed-Dist)

	// ensure that B is the longer of the two strings (N >= M)
	if(Lb < La)
		swap(A, B, La, Lb, Delta);

	// setup an array from -(La+1)...(Lb+1) initialized with -1 for the elements
	length	space[4096];//- = new length[La + Lb + 3];
	memset(space, (char)-1, (La + Lb + 3)*sizeof(length));
	length	*fp = space + La + 1,
			P = -1;

	// calculate the edit distance
	do {
		P++;
		// generate furthest points for diagonals below Delta
		for(length diagN = -P; diagN < Delta; diagN++)
			fp[diagN] = snake( diagN,
							   max(fp[diagN-1]+1, fp[diagN+1]),
							   A, B, La, Lb );

		// generate furthest points for diagonals above Delta
		for(length diagN = Delta+P; diagN > Delta; diagN--)
			fp[diagN] = snake( diagN,
							   max(fp[diagN-1]+1, fp[diagN+1]),
							   A, B, La, Lb );

		// generate furthest point for diagonal Delta
		fp[Delta] = snake( Delta, max(fp[Delta-1] + 1, fp[Delta+1]),	A, B, La, Lb );
	} while(fp[Delta] != Lb);

//-	delete space;		// free the array
	return Delta + P + P;
}

inline void swap(string &X, string &Y, length &lX, length &lY, length &d)
{
	string T	= X;
	length lT	= lX;
	X	= Y;
	lX	= lY;
	Y	= T;
	lY	= lT;
	d	= -d;
}

inline length max(length a, length b) {if(a >= b) return a; else return b;}

// function to follow diagonals in the edit graph
//	returns furthest reachable y along diagonal k when starting from x,y
length snake(length k, length y, string A, string B, length La, length Lb)
{
	// Original algorithm has A[x+1] == B[y+1] which is consistent with
	//	its model of strings A = a_1,a_2, ...a_M and B = b_1,b_2, ...b_N.
	//	For C, the strings are: A = a_0,a_1, ...a_M-1 and B = b_1,b_2, ...b_N-1.
	// COMPARE:
	for(length x = y - k; x < La && y < Lb && A[x] == B[y]; x++, y++);
	return y;
}

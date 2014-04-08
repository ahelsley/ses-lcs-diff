function max(a,b) { if(a>=b) { return a; } else { return b; }

function diff(A,B) {
	var a; var add = '+';
	var b; var rem = '-';
	if (A.length < B.length) {
		a	= A.split("");
		b	= B.split("");
	} else {
		a	= B.split("");	add = '-';
		b	= A.split("");	rem = '+';
	}

	var fp		= {};
	var path	= {};

	var M = a.length;
	var N = b.length;
	var D = N - M;

	function snake(k) {
		var i = (fp[k-1] || -1) + 1;
		var j =  fp[k+1] || -1;

		var v;
		if (i > j) {
			if (path[k-1]) {
				path[k] = this.path[k-1].slice(0);
			}
			v = add;
		} else {
			if (path[k+1]) {
				path[k] = path[k+1].slice(0);
			}
			v = rem;
		}
		if (!path[k]) {
			path[k] = [];
		}
		path[k].push(v);

		var y = max(i, j);
		var x = y - k;
		while ((x < M && y < N)
				&& a[x] == b[y]) {
			x++;
			y++;
			path[k].push('=');
		}
		return y;
	}

	var p = -1;
	do {
		p++;
		for (var k =    -p ; k < D; k++) {
			fp[k] = snake(k);
		}
		for (var k = D + p ; k > D; k--) {
			fp[k] = snake(k);
		}
		fp[D] = snake(D);
	} while (fp[D] != N);

	// n = edit distance
	// a = # of added elements
	// r = # of removed elements
	// e = edit script
	return {n: D+2*p, a: p, r: D+p, e: path[D].slice(1)};
}

Diff.prototype.asHTML = function () {
	var a	= this.a.slice(1);
	var b	= this.b.slice(1);

	var html= "";
	var tags= [["<del>","</del>"], ["",""], ["<ins>","</ins>"]];

	// discard first element; it's trash created when transiting to (0,0).
	var which = this.lcs.slice(1);
	for(var i = 0; i < which.length;) {
		var j = i, t = "", w = which[i];
		if (w > 0) {
			for(; which[j]  > 0 && j < which.length; j++) {
				t += b.shift();
			}
		} else if (w < 0) {
			for(; which[j]  < 0 && j < which.length; j++) {
				t += a.shift();
			}
		} else {
			for(; which[j] == 0 && j < which.length; j++) {
				t += a.shift();
				b.shift();
			}
		}
		html += tags[1+w][0]+ t +tags[1+w][1];
		i = j;
	}
	return "<pre>" + html + "</pre>";
}

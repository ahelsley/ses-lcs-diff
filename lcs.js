function Diff(A, B) {
	this.path = {};
	if (A.length < B.length) {
		this.a	= A.split("");
		this.b	= B.split("");
	} else {
		this.a	= B.split("");
		this.b	= A.split("");
	}
	// push dummy.
	this.a.unshift(null);
	this.b.unshift(null);
	this.fp		= {};
	this.lcs	= this.onp();
}
function max() {
	var max = null;
	for (var i = 0; i < arguments.length ; i++ ) {
		if (max == null || max < arguments[i]) {
			max = arguments[i];
		}
	}
	return max;
}
Diff.prototype.onp = function () {
	var M = this.a.length-1;
	var N = this.b.length-1;
	var delta = N - M;
	var p = 0;
	do {
		for (var k =        -p ; k <= delta - 1 ; k++) {
			this.fp[k] = this.snake(k);
		}
		for (var k = delta + p ; k >= delta + 1 ; k--) {
			this.fp[k] = this.snake(k);
		}

		// k = delta:
		var k = delta;
		var y = this.snake(k);
		this.fp[k] = y;
		p++;
	} while (this.fp[delta] != N);
	return this.path[delta];
}
Diff.prototype.snake = function(k) {
	var i = (this.fp[k - 1] || -1) + 1;
	var j =  this.fp[k + 1] || -1;

	var v;
	if (i > j) {
		if (this.path[k - 1]) {
			this.path[k] = this.path[k-1].slice(0);
		}
		v = 1;
	} else {
		if (this.path[k + 1]) {
			this.path[k] = this.path[k+1].slice(0);
		}
		v = -1;
	}
	if (!this.path[k])
		this.path[k] = [];
	this.path[k].push(v);

	var y = max(i, j);
	var x = y - k;
	while ((x < (this.a.length-1) && y < (this.b.length-1))
			&& this.a[x+1] == this.b[y+1]) {
		x++;
		y++;
		this.path[k].push(0);
	}
	return y;
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

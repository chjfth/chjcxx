#ifndef ___MINMAX__H_20250822_
#define ___MINMAX__H_20250822_


template<typename T> T _MIN_(T x, T y) { return x < y ? x : y; }
template<typename T> T _MAX_(T x, T y) { return x > y ? x : y; }

template<typename T> T _MID_(T x, T m, T y) 
{ 
	// Pull m into the range of [x,y] .
	return _MAX_(x, _MIN_(m, y));
}


#endif

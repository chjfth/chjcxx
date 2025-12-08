#ifndef ___MINMAX__H_20251208_
#define ___MINMAX__H_20251208_


template<typename T1, typename T2> 
T1 _MIN_(T1 x, T2 y) 
{ 
	return x < y ? x : (T1)y; 
}

template<typename T1, typename T2> 
T1 _MAX_(T1 x, T2 y) 
{ 
	return x > y ? x : (T1)y; 
}

template<typename T1, typename T2, typename T3> 
T2 _MID_(T1 x, T2 m, T3 y) 
{ 
	// Clamp m into range of [x,y] .
	return _MAX_(_MIN_(m, y), x);
}


#endif

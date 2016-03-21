#ifndef ___MINMAX__H
#define ___MINMAX__H

#define _MIN_(a,b) ((a)<=(b) ? (a) : (b))
#define _MAX_(a,b) ((a)>=(b) ? (a) : (b))

#define _MID_(a,b,c) ( _MAX_(a,_MIN_(b,c)) )

#endif

#ifndef __CxxVerCheck_h_
#define __CxxVerCheck_h_
#define __CxxVerCheck_h_created_ 20250429
#define __CxxVerCheck_h_updated_ 20260527

/*
	  ____              ____  _      _                  
	 / ___| _     _    / ___|| |_ __| | __   _____ _ __ 
	| |   _| |_ _| |_  \___ \| __/ _` | \ \ / / _ \ '__|
	| |__|_   _|_   _|  ___) | || (_| |  \ V /  __/ |   
	 \____||_|   |_|   |____/ \__\__,_|   \_/ \___|_|   
*/

#if (__cplusplus>=201103L) || (_MSC_VER>=1900) // C++11 or VC2015+
# define CXX11_OR_NEWER
#endif // C++11

#if (__cplusplus>=201402L) || (_MSC_VER>=1900) // C++14 or VC2015+
# define CXX14_OR_NEWER
#endif // C++14



/*
	__     ___                 _    ____            
	\ \   / (_)___ _   _  __ _| |  / ___| _     _   
	 \ \ / /| / __| | | |/ _` | | | |   _| |_ _| |_ 
	  \ V / | \__ \ |_| | (_| | | | |__|_   _|_   _|
	   \_/  |_|___/\__,_|\__,_|_|  \____||_|   |_|  
*/
#ifdef _MSC_VER

#if _MSC_VER >= 1600
#define VC2010_OR_NEWER
#endif

#if _MSC_VER >= 1800
#define VC2013_OR_NEWER
#endif

#if _MSC_VER >= 1900
#define VC2015_OR_NEWER
#endif

#if _MSC_VER >= 1910
#define VC2017_OR_NEWER
#endif

#if _MSC_VER >= 1923
#define VC2019_OR_NEWER
#endif

#if _MSC_VER >= 1930
#define VC2022_OR_NEWER
#endif

////////

#if _MSVC_LANG >= 201402L
#define CXX14_OR_NEWER
#endif

#if _MSVC_LANG >= 201703L
#define CXX17_OR_NEWER
#endif

#if _MSVC_LANG >= 202002L
#define CXX20_OR_NEWER
#endif


#endif // _MSC_VER



/*
 ____            _    ____ _   _ _   _    ____  ____ ____ 
|  _ \ ___  __ _| |  / ___| \ | | | | |  / ___|/ ___/ ___|
| |_) / _ \/ _` | | | |  _|  \| | | | | | |  _  |  | |    
|  _ <  __/ (_| | | | |_| | |\  | |_| | | |_| | |__| |___ 
|_| \_\___|\__,_|_|  \____|_| \_|\___/   \____|\____\____|
*/

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#define USING_REAL_GCC
// Then use __GNUC__ to check for gcc major version, 7, 9, 10, 12 etc
// [2026-05-06] SimpleIni.h implementation need __GNUC__>=11 .
#endif





/*
	 _   _   _     _   _   _   _           
	| | | | | |_  (_) | | (_) | |_   _   _ 
	| | | | | __| | | | | | | | __| | | | |
	| |_| | | |_  | | | | | | | |_  | |_| |
	 \___/   \__| |_| |_| |_|  \__|  \__, |
									 |___/ 
*/


// Some C++ keyword name alias for older vers
#ifdef CXX11_OR_NEWER
	#define cxx11_override override
	#define cxx11_constexpr constexpr
#else
	#define cxx11_override
	#define cxx11_constexpr
#endif // CXX11_OR_NEWER


#ifdef CXX11_OR_NEWER
	#define CXX_DISABLE_COPY_ASSIGN(ClassName) \
		ClassName(const ClassName&) = delete; \
		ClassName& operator=(const ClassName&) = delete; 
#else
	#define CXX_DISABLE_COPY_ASSIGN(ClassName) \
	private: \
		ClassName(const ClassName&); \
		ClassName& operator=(const ClassName&); 
#endif // CXX11_OR_NEWER


#ifdef CXX17_OR_NEWER
#define cxx17_constexpr constexpr
#else
#define cxx17_constexpr
#endif // CXX17_OR_NEWER


#endif

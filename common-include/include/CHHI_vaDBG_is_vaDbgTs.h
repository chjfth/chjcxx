// If your CHHI.cpp wants to see debug-messages from various CHHI library modules
// (for XXX module, XXX_DEBUG defined as a premise), you need to:
// Copy this file's content to start portion of your CHHI.cpp, then (tweaking)
// replace `vaDbgTs` with your own debug-message output functions.
//
// Tweaking note: vaDBG, vaDGB1 etc must NOT be macro, they must be functions
// to support CHHI_vaDBG_hide.h/CHHI_vaDBG_show.h's code pattern.
//
// If your CHHI.cpp wants no debug messages, you don't have to use this file.


#include <utility>       // to have std::forward
#include <CxxVerCheck.h> // to see CXX11_OR_NEWER

#ifdef CXX11_OR_NEWER

template<typename... Args>
void vaDBG(Args&&... args) // forwards all arguments
{
	vaDbgTs(std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG1(Args&&... args) // forwards all arguments
{
	vaDbgTs(std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG2(Args&&... args) // forwards all arguments
{
	vaDbgTs(std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG3(Args&&... args) // forwards all arguments
{
	vaDbgTs(std::forward<Args>(args)...);
}

#else

void vaDBG(...)
{
	vaDbgTs(_T("Not C++11 compiled, vaDBG() is nullop."));
};

void vaDBG1(...)
{
	vaDbgTs(_T("Not C++11 compiled, vaDBG() is nullop."));
};

void vaDBG2(...)
{
	vaDbgTs(_T("Not C++11 compiled, vaDBG() is nullop."));
};

void vaDBG3(...)
{
	vaDbgTs(_T("Not C++11 compiled, vaDBG() is nullop."));
};

#endif // CXX11_OR_NEWER

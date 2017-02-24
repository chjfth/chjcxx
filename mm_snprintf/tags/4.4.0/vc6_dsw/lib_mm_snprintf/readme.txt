[2008-05-27]
In order to build the lib with this .dsp, you should manually copy mm_snprintf.cpp 
to mm_snprintfW.cpp. What's more, any time you do the change to mm_snprintf.cpp, 
you should synchronize it to mm_snprintfW.cpp .
The build result will be one lib supporting both ANSI & Unicode version of 
mm_snprintf functions.

VS project to improve later.

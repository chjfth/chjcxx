
ifdef gmu_DIR_GNUMAKEUNIPROC
  include $(gmu_DIR_GNUMAKEUNIPROC)/pattern1-concise-header.mki
  include $(gmu_DIR_ROOT)/nlscan/nlssvn-prjs.mki
else
  $(error gmu_DIR_GNUMAKEUNIPROC is not defined, You have to set up GnumakeUniproc environment to use this Makefile)
endif

# GMU-var: gmu_PRJ_NAME
# 	Define the project name(prj-name for short).
# project name for a project(precisely, after decoration
# into gmu_G_Final_PrjName) should be unique 
# across the whole build process of one GnumakeUniproc run. Prj-name
# conflict will be detected by GMU if two or more projects have the same
# gmu_G_Final_PrjName.
ifndef gmu_PRJ_NAME
  gmu_PRJ_NAME = gadgetlib-lib
endif

# GMU-var: gmp_bc_IMAGE_TYPE
#	Define the "image-type" to generated for this project.
# Common image-types are: LIB, DLL and EXE.
gmp_bc_IMAGE_TYPE = LIB

# Compiler specific settings below.
ifeq ($(call gmuf_IsWordInSet,$(gmp_COMPILER_ID),msvc wince),1)
  # msvc, wince compiler 
  gmp_bc_CRT_STATIC_LIB = 1
	# 1: Use static CRT lib (compiler option /MT or /MTd)
	# (null): use DLL CRT lib (compiler option /MD or /MDd)

  gmp_CFLAGS_NON_CODE_GEN += /W3
  
  gmu_up_list_SRC_FROM_ROOT += mswin/ps_func.cpp
  gmu_ud_list_PRJ_HEADER += 

  # gmp_u_list_PLUGIN_TO_LOAD += PI_winres_msvc       # compile Windows resource(.rc)
  # gmp_u_list_PLUGIN_TO_LOAD += PI_use_pch_msvc      # use precompile header
  # gmp_u_list_PLUGIN_TO_LOAD += PI_windll_from_lib   # make dll from lib
  # gmp_u_list_PLUGIN_TO_LOAD += PI_sync_debug_info

#  ifeq (wince,$(gmp_COMPILER_ID))
#    gmp_CFLAGS_MACRO_DEFS += -D NO_assert
#  endif

else ifeq ($(call gmuf_IsWordInSet,$(gmp_COMPILER_ID),linuxgcc),1)
  # GCC compiling linux program

  gmu_up_list_SRC_FROM_ROOT += linux/ps_func.cpp
  gmu_ud_list_PRJ_HEADER += 

  # gmp_u_list_PLUGIN_TO_LOAD += PI_gcc_strip_lo      # strip output executable
  # gmp_u_list_PLUGIN_TO_LOAD += PI_use_pch_gcc       # use precompile header
  
else ifeq ($(call gmuf_IsWordInSet,$(gmp_COMPILER_ID),armcpp),1)

  # armcpp
  gmu_up_list_SRC_FROM_ROOT += 
  gmu_ud_list_PRJ_HEADER += 

endif

# GMU-var: gmp_u_list_PLUGIN_TO_LOAD
#	Designate plugins to load in this make-var, multiple plugin names separated
# by spaces. Plugins are searched from $(gmp_ud_list_CUSTOM_MKI) and then
# from $(gmu_DIR_GNUMAKEUNIPROC)/pattern1cfg/plugins .
gmp_u_list_PLUGIN_TO_LOAD += PI_copy_psTCHARh
gmi_CTCH_dirNlssvn = $(gmu_ud_SRC_ROOT)/../../..
gmi_CTCH_dirCopyTo = $(gmu_ud_SRC_ROOT)/include

# Other plugins: PI_sync_devoutput PI_always_compile PI_file_precopy


gmp_u_list_DLL_EXPORT_SYM += DLLEXPORT_gadgetlib
	#(The C macros to decorate as __declspec(dllexport) etc. all platform)
	#Yes, even this is a LIB, you should have it in order to work with PI_dll_from_lib.

# GMU-var: gmu_uf_LINK_OUTPUT
#	Define the link ouput filename for this project.
# Decoration may be applied to this name which results in $(gmu_Gf_Final_LinkOutput).
# Using function gmpf_MakeLibName to construct output name makes the name more portable.
gmu_uf_LINK_OUTPUT = $(call gmpf_MakeLibName,gadgetlib)

gmp_DECO_UNICODE_FOR_LINK_OUTPUT = 1
	#Set this to 1 if you want a _U suffix on your output-image when gmp_bc_UNICODE=1
gmp_DECO_DEBUG_FOR_LINK_OUTPUT = 1
	#Set this to 1 if you want a _D suffix on your output-image when gmp_bc_DEBUG=1


# GMU-var: gmu_ud_SRC_ROOT
#	Define the root dir where all your source files(.c, .cpp, not .h) reside in.
# You can use a relative dir here, but note, it is relative to make-var $(CURDIR),
# not to this mki file. Normally, $(CURDIR) is your start-up makefile directory.
gmu_ud_SRC_ROOT = ..
dirThisRepo = ../..
dirNlssvn = $(dirThisRepo)/../..
dirCommonLib = $(dirNlssvn)/CommonLib
dirCommonInclude = $(dirCommonLib)/common-include

# GMU-var: gmu_up_list_SRC_FROM_ROOT
#	Define your source files to compile, source files(dir prefix allowed) separated by spaces or tabs.
# You MUST use relative paths here, which are relative to $(gmu_ud_SRC_ROOT), 
# however, any of the resulting absolute path should not go up beyond $(gmu_ud_SRC_ROOT),
# e.g.
# * You cannot use ``../myfunc.c'', which actually resides in $(gmu_ud_SRC_ROOT)'s parent dir.
# * You can use ``subdir1/../myfunc.c'' or ``subdir1/subdir2/../myfunc.c'', which are 
#	still in $(gmu_ud_SRC_ROOT) or $(gmu_ud_SRC_ROOT)'s subdir.
gmu_up_list_SRC_FROM_ROOT += \
	gadgetlib.c \

# GMU-var: gmu_ud_list_PRJ_HEADER
#	Define header file dirs, multiple dirs should be separated by spaces or tabs.
# You can use relative dirs here, but note, they are relative to make-var $(CURDIR),
# not to this mki file.
#	This var serves two purposes:
#	1. They'll be passed to the compiler through ``-I <dir1> -I <dir2> ...'' .
#	2. They'll be passed to `$(gmu_GCC_M_PREFIX)gcc -M' in favor of generating hdepend files.
gmu_ud_list_PRJ_HEADER += \
	$(gmu_ud_SRC_ROOT)/include \
	$(gmu_ud_SRC_ROOT) \
	$(dirCommonInclude)
	# Don't write ``-I'' in this var; -I will be prependded automatically.

gmp_CFLAGS_INCLUDES += # $(addprefix -I,<dir1> <dir2> ...)
	# (normally, you don't need this)
	# those -I xxx options (in case you don't want to list them in gmu_ud_list_PRJ_HEADER)
	# You must explicitly write ``-I'' before every dir in gmp_CFLAGS_INCLUDES

# GMU-vars: gmp_CFLAGS_<XXX>
#	Set specific compiler options for this project in the following vars:
# gmp_CFLAGS_CODE_GEN_ALL:
#	Any options that affect the generated binary code should be listed in it.
#	Consider gmp_CFLAGS_CODE_GEN_ALL as your accustomed CFLAGS.
# gmp_CFLAGS_NON_CODE_GEN:
#	Any options that do not affect the generated binary code should be listed in it.

gmp_CFLAGS_CODE_GEN_ALL +=
  # You may want to dispense options for gmp_CFLAGS_CODE_GEN_ALL into following 2 categories:
  gmp_CFLAGS_MACRO_DEFS += $(gmp_CFLAGS_MACRO_DEFS_preset) # append other macro defines, e.g. -Dxxx=yyy
  gmp_CFLAGS_CODE_GEN += $(gmp_CFLAGS_CODE_GEN_preset) # append other code generation related options

gmp_CFLAGS_NON_CODE_GEN += $(gmp_CFLAGS_NON_CODE_GEN_preset)

# If some compiler options(.e.g -Dxxx ) must be passed to `gcc -M' command, set them here:
# (Note: Those include-dir options(-I xxx) don't have to be set here.)
gmu_u_GCC_M_OPTIONS +=


# List extra objs to link in gmu_up_list_EXTRA_OBJ_TO_LINK:
gmu_up_list_EXTRA_OBJ_TO_LINK +=


# GMU-vars: gmu_uv_list_SUBPRJ_LINK_FOR_PARENT
#	If your LIB project wants its sub-projects to be linked by
# its parent EXE project, the LIB project can assign those sub-projects in 
# this variable, so that your parent EXE project don't have to know what
# (further sub-projects) your current LIB project are depending on.
# It's amazing, really.
# ( note: If you don't use gmu_uv_list_SUBPRJ_LINK_FOR_PARENT at first,
# and later enable gmu_uv_list_SUBPRJ_LINK_FOR_PARENT, then you must clean 
# the old project output-dir first in order for that change to work. )
gmu_uv_list_SUBPRJ_LINK_FOR_PARENT += # <subprj-ref> ...

<subprj-ref>_Makefile = # where the sub-project's makefile resides
# If the sub-project's makefiles are to be checked-out from some repository, assign it here.
# >>>> For SVN checkout
<subprj-ref>_svnUrl_forMakefiles = # SVN Url for the checkout
	# Hint: $(call gmuf_MakeSvnRepositoryPath,<SVN-path-prefix-var>,<remaining-repository-path>)
<subprj-ref>_svnLocalDir_forMakefiles = # Local dir corresponding to the whole SVN URL
<subprj-ref>_svnRev = # Revision to use(string after svn's -r switch)
# <<<< For SVN checkout
# >>>> For CVS checkout
<subprj-ref>_cvsRoot_forMakefiles = # CVSROOT for the checkout
	# Hint: $(call gmuf_MakeCvsRepositoryPath,<CVS-server-var>,<repository-path-on-CVS-server>)
<subprj-ref>_cvsModuleDir_forMakefiles = # CVS module name or module dir to checkout
<subprj-ref>_cvsLocalDir_forMakefiles = # Local dir corresponding to CVSROOT
<subprj-ref>_cvsTag_forMakefiles = # Tag or branch to checkout(string after cvs's -r switch)
# <<<< For CVS checkout


# ----------- below for plugin PI_sync_devoutput -----------

# If you use plugin PI_sync_devoutput, you may want to set:
gmi_SYDO_ud_SYNC_HEADER_FROM = $(gmu_ud_SRC_ROOT)/include
	# The dir that your headers reside in. ( relative to $(CURDIR) )

gmi_SYDO_up_list_SYNC_HEADER += gadgetlib
	# List filepaths of the header files to sync. ( relative to $(gmi_SYDO_ud_SYNC_HEADER_FROM) )
gmi_SYDO_up_list_SYNC_HEADER += $(if $(findstring PI_copy_psTCHARh,$(gmp_u_list_PLUGIN_TO_LOAD)),ps_TCHAR.h)

# gmi_SYDO_ud_SYNC_HEADER_TO = # (suggested to set as env-var or by parent project)
	# Where the headers will be sync-ed to.

# gmi_SYDO_ud_SYNC_LIB_TO = # (suggested to set as env-var or by parent project)
	# Where the static LIBs will be sync-ed to. Sync-source is $(gmp_Gp_list_LIB_OUTPUT) .



include $(gmu_DIR_GNUMAKEUNIPROC)/pattern1.mks


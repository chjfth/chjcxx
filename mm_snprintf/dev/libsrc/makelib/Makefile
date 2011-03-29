include $(gmu_DIR_GNUMAKEUNIPROC)/CommonFunctions.mki
	# To include definition of gmuf_IsWordInSet

# Different compiler uses different source set, described below. 
ifeq ($(call gmuf_IsWordInSet,$(gmp_COMPILER_ID),msvc mingw wince),1)
  # msvc, mingw or wince compiler
  gmp_bc_CRT_STATIC_LIB = 1

  gmu_up_list_SRC_FROM_ROOT += msvc/mm_psfunc.cpp
  gmu_ud_list_PRJ_HEADER += $(gmu_ud_SRC_ROOT)/msvc

  ifeq (wince,$(gmp_COMPILER_ID))
    gmp_CFLAGS_MACRO_DEFS += -D NO_assert
  endif

else ifeq ($(call gmuf_IsWordInSet,$(gmp_COMPILER_ID),linuxgcc),1)
  # linuxgcc
  gmu_up_list_SRC_FROM_ROOT += glibc/mm_psfunc.cpp glibc/wscfunc.cpp
  gmu_ud_list_PRJ_HEADER += $(gmu_ud_SRC_ROOT)/glibc
  	  
else ifeq ($(call gmuf_IsWordInSet,$(gmp_COMPILER_ID),armcpp),1)
  # armcpp
  MMSNPRINTF_NO_UNICODE = 1
  gmu_up_list_SRC_FROM_ROOT += glibc/mm_psfunc.cpp
	# Yes, use the same mm_psfunc.cpp as glibc, currently.
  gmu_ud_list_PRJ_HEADER += $(gmu_ud_SRC_ROOT)/armcpp

endif

include ./lib-all-compiler.mki

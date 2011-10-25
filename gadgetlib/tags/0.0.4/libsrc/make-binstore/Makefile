
ifdef gmu_DIR_ROOT
  include $(gmu_DIR_ROOT)/nlscan/nlssvn-scalacon.mki
else
  $(error gmu_DIR_ROOT is not defined, You have to set up GnumakeUniproc environment to use this Makefile)
endif

ifndef gmu_PRJ_NAME
  gmu_PRJ_NAME = gadgetlib-make-binstore
endif

gmp_CONTAINER_EXTRACT_COMPILER_ID = 1


dir_binstore = $(call gmuf_GetAbsPathByRela_soc,../../binstore)
$(call gmuf_AnounceExportEnvvar,gmp_u_list_PLUGIN_TO_LOAD_ENV_PRE=PI_sync_devoutput)
$(call gmuf_AnounceExportEnvvar,gmi_SYDO_ud_SYNC_LIB_TO=$(dir_binstore))
$(call gmuf_AnounceExportEnvvar,gmi_SYDO_ud_SYNC_DLL_TO=$(dir_binstore))
$(call gmuf_AnounceExportEnvvar,gmi_SYDO_ud_SYNC_EXE_TO=$(dir_binstore))


gmu_uv_list_ALL_SUBPRJ += lib dll

lib_Makefile = ../makelib/Makefile
#lib_MakeVarDefines =

dll_Makefile = ../makedll/Makefile
#lib_MakeVarDefines =


include $(gmu_DIR_GNUMAKEUNIPROC)/pattern-container1.mks


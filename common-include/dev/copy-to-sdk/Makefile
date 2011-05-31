ifdef gmu_DIR_GNUMAKEUNIPROC
  include $(gmu_DIR_GNUMAKEUNIPROC)/CommonFunctions.mki
else
  $(error gmu_DIR_GNUMAKEUNIPROC is not defined, You have to set up GnumakeUniproc environment to use this Makefile)
endif

gmu_PRJ_NAME = commmon-include-copy-to-sdk

ifndef gmb_syncto
  $(error gmb_syncto not defined, so I don't know where to copy the headers)
endif

# Replace <your-custom-target> with you own make-target whose rule should
# appear in u_freestyle.mki .
gmu_FREE_STYLE_MAKEFILE = 1 
gmp_USER_FREE_STYLE_TARGET += $(gmu_PRJ_NAME)

d_CopyToInclude = $(gmb_syncto)/include

$(gmu_PRJ_NAME):
	@echo "Copy common-include headers to $(d_CopyToInclude) ..."
	@(call gmuf_MkdirIfNotExist,$(d_CopyToInclude))
	cp ../*.h $(d_CopyToInclude)


include $(gmu_DIR_GNUMAKEUNIPROC)/pattern1-container.mks


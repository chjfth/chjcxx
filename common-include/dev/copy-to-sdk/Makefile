
gmu_PRJ_NAME = commmon-include-copy-to-sdk

ifndef gmb_syncto
  $(error gmb_syncto not defined, so I don't know where to copy the headers)
endif

# Replace <your-custom-target> with you own make-target whose rule should
# appear in u_freestyle.mki .
gmu_FREE_STYLE_MAKEFILE = 1 
gmp_USER_FREE_STYLE_TARGET += $(gmu_PRJ_NAME)


$(gmu_PRJ_NAME):
	@echo "Copy common-include headers to $(gmb_syncto)/include ..."
	cp ../*.h $(gmb_syncto)/include


include $(gmu_DIR_GNUMAKEUNIPROC)/pattern1-container.mks


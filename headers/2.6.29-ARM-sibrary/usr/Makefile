headers_install:
	cp -R include $(INSTALL_HDR_PATH)/

headers_check:
	echo Dummy

dist:
	rm -f ../kernel-headers-2.6.29.tgz
	tar -C .. -cvf ../kernel-headers-2.6.29.tgz usr

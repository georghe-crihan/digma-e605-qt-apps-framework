headers_install:
	cp -R include $(INSTALL_HDR_PATH)/

headers_check:
	echo Dummy

dist:
	rm -f ../kernel-headers-2.6.20.tgz
	tar -C .. -cvzf ../kernel-headers-2.6.20.tgz usr

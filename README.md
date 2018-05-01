# Digma E605 E-Ink reader third-party applications framework

## Subject
This is a low-cost E-Ink pearl reader, based on Boeye Sibrary C60 platform and SDK.

The application framework has been switched from GTK to QT 4.7 over the course of the years.
Currently this product is discontinued and unsupported by the vendor.

### Some technical specs and related info:
* Target chip RK-2818

  ARM926EJC or ARM926EJ-S rev 5 (v5l)

  CPU architecture  5TEJ (J stands for Java extensions)

* Crosstool-ng v. 1.12.4 (1.10.0 for the Android system)
* Linux boeye 2.6.25-dirty #767 Tue Mar 25 17:00:08 CST 2014 armv5tejl GNU/Linux

  Kernel version 2.6.25, <i id="strings">strings(1)</i> found therein:
```
  RK2818 SCU VERSION=20111204,V1.12
  Linux version 2.6.25-dirty (zpp@zpp) (gcc version 4.6.3 (Sourcery CodeBench Lite 2012.03-57) ) #767 Tue Mar 25 17:00:08 CST 2014
  rockchip 281x version release v1.09(Eink) used in Eink, without camera video and touch mabe
  rknand_base.c version: 4.30 20111009
  rknand_buffer.c version: 4.38 20121130
```
* Glibc V2.9, strings found therein: 
```
  GNU C Library (crosstool-NG 1.12.4) stable release version 2.9, by Roland McGrath et al.
```
* EPAT 2.0.1, produces libexpat.so.1.5.2
* DBUS 1.2.24, produces libdbus-1.so.3.4.0
* GCC v.4.4.3
* The device uses QT linuxfb

  qt-everywhere-opensource-src-4.7.4

## Related works:
* [eView](https://github.com/S-trace/eView) although it works only on GTK firmware.

## Prerequisites
0. git clone https://github.com/georghe-crihan/digma-e605-qt-apps-framework.git
1. Install CentOS 6.9

  The Parallels VM is named "CentOS 6". (See [Digressions](#digressions) section
  on why there's Parallels).

  I used netinstall and the "Development Workstation" type of install.

2. Enable the Wheel group in sudoers.conf through _visudo(8)_
3. Start XQuartz
4. SSH to host and start terminal
5. Optionally remove the GDM and WPA supplicant to conserve some memory

  Also I disable all but a single TTYs (see /etc/sysconfig/init).

6. install texinfo, EPEL and, optionally, DKMS:
  ```
  yum -y install texinfo epel-release dkms
  ```
7. Optionally install the Parallels tools
8. Switch to background:
  ```
  # Stop the Hypervisor and the VM:
  digma-e605-qt-apps-framework/tools/centos-up.sh 1
  # ... and start:
  digma-e605-qt-apps-framework/tools/centos-up.sh

  ```
9. Do a yum upgrade


## Crosstool-ng

NB: It builds only on CentOS 6 due to broken cloog-ppl (also, see the
[Digressions](#digressions) section)!

NB: crosstool-ng DOES NOT build on OSX. One of the reasons - case-insensitive FS.

### Bootstrap:
See the details here: https://crosstool-ng.github.io/docs/
```
git clone https://github.com/crosstool-ng/crosstool-ng.git
cd crosstool-ng
git checkout crosstool-ng-1.12.4
./configure
make
sudo make install
cd
```

### Configuring the toolchain:
```
  # Get the original Boeye/Sibrary headers from its SDK:
  git clone https://github.com/georghe-crihan/digma-e605-qt-apps-framework.git
  cd digma-e605-qt-apps-framework/headers/2.6.29-ARM-sibrary/usr
  make dist
  mv ../kernel-headers-2.6.29.tgz ~/
  cd ~/crosstools-ng
  # You have to download these manually, as otherwise the build just fails:
  wget http://ftp.osuosl.org/pub/clfs/conglomeration/cloog-ppl/cloog-ppl-0.15.10.tar.gz
  wget https://kent.dl.sourceforge.net/project/duma/duma/2.5.15/duma_2_5_15.tar.gz
  wget https://kent.dl.sourceforge.net/project/expat/expat/2.0.1/expat-2.0.1.tar.gz
  wget https://datapacket.dl.sourceforge.net/project/strace/strace/4.5.19/strace-4.5.19.tar.bz2
  mkdir -p .build/tarballs
  mv cloog-ppl-0.15.10.tar.gz duma_2_5_15.tar.gz expat-2.0.1.tar.gz strace-4.5.19.tar.bz2 .build/tarballs/
  cp samples/arm-unknown-linux-gnueabi/crosstool.config .config
  ct-ng menuconfig
  # and afterwards
  ct-ng build
```
> * Target Options: Target Architecture: arm
> * --------------- Architecture level: armv5te
> * --------------- Emit Assembly for CPU: arm926ej-s
> * --------------- Tune for CPU: arm926ej-s
> * --------------- Use specific FPU: vfp
> * --------------- Floating point: software
> * Toolchain options: Tuple's vendor string: 926ejs
> * Operating System: Target OS: Linux
> * ----------------- Get kernel headers from: kernel's headers_install
> * ----------------- Linux kernel version: custom tarball
> * ----------------- Path to custom tarball: /home/user/kernel-headers-2.6.29.tgz
> * C compiler: gcc version: 4.4.3
> * ----------- Additional supported languages: C++
> * Binary utilities: binutils version: 2.20.1a
> * C-library: glibc: glibc version: 2.9



## QT
### Prerequisites:

NB: These libraries, as well as the QT library itself WILL NOT be installed to
the target device, but only used on the build machine as build dependencies
since the original SDK headers and libraries are unavailable.

#### expat (a dependency of libdbus-1)

```
  wget https://datapacket.dl.sourceforge.net/project/expat/expat/2.0.1/expat-2.0.1.tar.gz
  tar -xvzf expat-2.0.1.tar.gz
  cd expat-2.0.1
  export PATH="${PATH}:/home/mac/x-tools/arm-926ejs-linux-gnueabi/bin/"
  ./configure --host=arm-926ejs-linux-gnueabi --prefix=/opt/rk2818
  make
  sudo -s
  export PATH="${PATH}:/home/mac/x-tools/arm-926ejs-linux-gnueabi/bin/"
  make install
  exit
  cd
```

#### libdbus-1 (a dependency of QT)

```
  wget https://dbus.freedesktop.org/releases/dbus/dbus-1.2.24.tar.gz
  tar -xvzf dbus-1.2.24.tar.gz
  cd dbus-1.2.24
  RK2818=/opt/rk2818
  INCPATH=${RK2818}/include
  LIBPATH=${RK2818}/lib
  export CFLAGS=-I${INCPATH}
  export LDFLAGS=-L${LIBPATH}
  export CXXFLAGS=${CFLAGS}
  ./configure --host=arm-926ejs-linux-gnueabi --prefix=/opt/rk2818
  make
  sudo -s
  export PATH="${PATH}:/home/mac/x-tools/arm-926ejs-linux-gnueabi/bin/"
  make install
  exit
  cd
```

### Build:
Example build for Raspberry-Pi: https://beter93.wordpress.com/2013/03/22/how-to-compile-qt-lib-with-crosstool-ng-for-raspberry/
```
  wget http://master.qt.io/archive/qt/4.7/qt-everywhere-opensource-src-4.7.4.tar.gz 
  tar -xvzf qt-everywhere-opensource-src-4.7.4.tar.gz
  cd qt-everywhere-opensource-src-4.7.4
  # Use my QT configuration:
  cp -R ~/digma-e605-qt-apps-framework/rk2818-g++ mkspecs/qws/

  # - or - you might roll your own, by starting from the default:

  cp -R mkspecs/qws/linux-arm-gnueabi-g++ mkspecs/qws/rk2818-g++/
  # and then edit the qmake.conf, replace the arm-none-linux-gnueabi-* prefix
  # with the arm-926ejs-linux-gnueabi-* and make any other changes as necessary.
  vi mkspecs/qws/rk2818-g++/qmake.conf

  export PKG_CONFIG_PATH=${LIBPATH}/pkgconfig  

  ./configure -lrt -opensource -confirm-license -prefix /opt/rk2818 \
    -no-qt3support -embedded arm -little-endian \
    -xplatform qws/rk2818-g++ -fast -no-xinput -no-xrandr \
    -no-openvg -no-opengl -no-gtkstyle -no-nis -no-cups -xmlpatterns \
    -exceptions -no-stl -no-accessibility -no-audio-backend -no-multimedia \
    -no-xfixes -no-mitshm -qt-gfx-linuxfb -dbus -force-pkg-config
  # Apply the patches:
  patch -p1 < ~/digma-e605-qt-apps-framework/qt-patches/patch-io.pri
  make
```

### Example:

http://doc.qt.io/qt-5/qtwidgets-tools-echoplugin-example.html

Better yet use demos/embedded/digiflip

### Deploying:

As mentioned in the [Research](#research) section, just copy the binary you
built to device's SD-CARD. This could be done through a USB cable, whereas
the SD-CARD would be mounted as a normal disk device.

Once copied (unmount properly and unplug the cable first), just launch the
executable from within the device's File Manager - simply click on it.

The [firstapp](firstapp) directory contains an installation
[script](firstapp/install.sh) to integrate the app into the device
environment, so as it would be available via a normal launch icon in the
"More..." folder in the stock QT File Manager. Click on the 
[install.sh](firstapp/install.sh) file, when finished copying.

## Research

### Stock DIGMA firmware analysis: 
Get the imgRePackerRK tool, see [References](#references) section.

Unpack the firmware first:
```
imgRePackerRK /cid update.img
# or, in my case, whereas Wine-HQ is used (see the "Digressions" section as to why):
digma-e605-qt-apps-framework/tools/unpack.sh
```

Open the OSX DiskUtility application and open that image therein.

NB: I have a trial version of the EXTFS tools by Paragon installed (including
the kernel support), so it mounts nicely, but you could, for example, use
OSXFuse.

### Tell the compiler versions:
```
objdump -s --section .comment /Volumes/Untitled/target-file
```

### How the kernel is put together:
Quoting russian [how is the kernel image built for the platform](http://roverbooksteel.narod.ru/debian/5point/kernel.htm) (my translation):
> Putting together a built kernel:
> ```
> ./mkkrnlimg arch/arm/boot/Image kernel.img.tmp
> cat kernel.img.tmp System.map > kernel.img
> ```
> There are two types of R2818 devices:
> 1. Running at a 600mhz clock speed and above. These devices often uas a SDK2-like kernel format (*boot.img = zImage + initramfs-cpio*)
>
> 2. Capped at ~300mhz (*Archos 70b ereader*, *Bq Voltaire* and a large number of chinese devices). A proprietary, scarcely documented format is used here.
> 
> The *mkkrnlimg* tool signs a non-compressed kernel (*arch/arm/boot/Image*) in a special way:
> ```
> ./mkkrnlimg arch/arm/boot/Image kernel.img.tmp
> ```
> The kernel is concatenated with the address table file *System.map*
> ```
> cat kernel.img.tmp System.map > kernel.img
> ```
> One can find the following, after extracting the firmware with the *rkunpack* tool (*AFPTool* wouldn't work):
> ```
> sadmich@esktop:~/rk2818$ ls -gG ./Image
> total 131108
> -rw-r--r-- 1   1441796 2012-02-19 22:48 boot.img
> -rw-r--r-- 1   5348346 2012-02-19 22:48 kernel.img
> -rw-r--r-- 1   4575932 2012-02-19 22:48 kernel.img-raw
> -rw-r--r-- 1    772402 2012-02-19 22:48 kernel.img-symbol
> -rw-r--r-- 1     49152 2012-02-19 22:48 misc.img
> -rw-r--r-- 1   2408452 2012-02-19 22:48 recovery.img
> -rw-r--r-- 1 119644160 2012-02-19 22:48 system.img
> ```
> *kernel.img-raw* - non-compressed kernel (i.e. a *arch/arm/boot/Image* file after the build)
>
> *kernel.img-symbol* - address table (*System.map* file after the build)
>
> The result of *mkkrnlimg* tool:
> ```
> kernel.img
> Offset  00 01 02 03   04 05 06 07   08 09 0A 0B   0C 0D 0E 0F   10 11 12 13   0123456789ABCDEF0123				
> 0000000 4B 52 4E 4C   BC 02 45 00   03 F0 21 E3   10 9F 10 EE   55 00 00 EB   KRNL..E...!.....U...
> 0000020 05 A0 B0 E1   51 00 00 0A   6A 00 00 EB   05 80 B0 E1   4E 00 00 0A   ....Q...j.......N...
> ...
> ```
> dword 0x4C4E524B - magic value - KRNL
>
> dword 0x0045D2BC - kernel size without the address table
>
> I.e. the *mkkrnlimg* tool is functionally identical to the *rkcrc* tool from the  *rkutils* package, when run with a "*-k*" command line parameter:
> ```
> ./rkcrc -k kernel.img-raw kernel.img-signed
> cat kernel.img-signed kernel.img-symbol > kernel.img
> ```
> The check reported these kinds of kernels were fully functional.
>

### General info on STOCK firmware for the G6 platform:
Quoting russian [Sibrary clone hacking page on a prominent gadgets forum](https://4pda.ru/forum/index.php?showtopic=423200&st=20) (my translation):
> A bit of software: if an executable file or a shell-script is copied into the E-Reader's memory or onto its SD-CARD and then opened via its "File Manager" - it thus will be run. It permits to launch arbitrary code on the E-Reader.

NB: apparently, the SD-CARD's FAT filesystem is mounted with *x* mode bits 
automatically turned on, so any file has the execute permissions set.

### Official Rockchip kernel

Rockchip officially supports RK2818 under Linux: http://linux-rockchip.org.

The kernel GIT repository:
```
git clone https://github.com/linux-rockchip/linux-rockchip.git 
cd linux-rockchip
# Checkout what appears to be a 2.6.20
git checkout 21d149db093c0d37e67620b281607844529fd0e8
```
NB: After the above commit, RK2818 support has been discontinued.

NB: Apparently, this is NOT the version used by Boeye/Sibrary (see the 
[strings](#user-content-strings), contained therein). 

#### Generating the headers:
```
  make menuconfig
  # Select the RK2818 architecture and VMMU in the menu
  make ARCH=arm INSTALL_HDR_PATH=/tmp/1 \
    CROSS_COMPILE=/home/mac/x-tools/arm-926ejs-linux-gnueabi/bin/arm-926ejs-linux-gnueabi- \
    headers_install
```

## Digressions

### Why there is a linux VM?

Personally I use OSX, as I strongly believe Linux on a Desktop is a waste of
time and effort, unless you are a strict Stallman follower/orthodox.
This is arguable, but ultimately a matter of personal taste.

In addition, I can promptly have any version of OS in a VM I want.

The emulator used could be anything from Parallels to QEMU or BOCHS.
Macports can make that really easy.
It could even be Docker engine (which presently still uses QEMU internally).

In my case it's s trial version of the Parallels Desktop, which is easily
scriptable and has an all superior OSX integration.

I also use Wine-HQ for some tools whereas no open source is available.

### Why CentOS 6?

Q: And not CentOS 7, for example, or OSX or you name it?

A: Because it's currently easier to build the Crosstool-NG toolchain.

## References

##### Similar works:
* [eView, russian forum](http://www.the-ebook.org/forum/viewtopic.php?p=1040672#1040672)

##### Rockchip platform:
* [Another Android Rockchip device modification page](http://freaktab.com/forum/tv-player-support/rk3188-devices/minix-x7-etc/647213-tutorial-modifying-the-boot-image)

##### QT:
* [QT Wiki page from Rockchip](http://opensource.rock-chips.com/wiki_Qt)
* [Another installing QT for Raspberry Pi](https://wiki.qt.io/Building_Qt_for_Embedded_Linux)
* [Solving QT build issues](http://bluelimn.tistory.com/entry/Qt-Cross-compile)

Here, the authors apparently were able to patch the boot image, unlike in the
quotations, I gave above.

##### CentOS image used:
* Name: CentOS-6.9-x86_64-netinstall.iso
* Size: 241172480
* MD5: cdab6b8142cb03e5f02102879d11ef44
* SHA1: 32f9f74fd27ec1ff7cc4f39a80d0dae34d9ec18b

##### imgRePackerRK:
* URL: https://forum.xda-developers.com/showthread.php?t=2257331
* Name: imgRePackerRK_106.zip
* Size: 180485
* MD5: 852bcc8f56694d3658db0d7d5e117093
* SHA1: f17f013c59e29d7376bdb1e856590dbd4a315797

##### Stock DIGMA firmware:
* URL: http://digma.ru
* Name: e605_boot620_20141220.zip
* Size: 136682454 bytes
* MD5: b739257045ec5f32363db0ff3ef936e1
* SHA1: 6f2c9da77ccf88443c4b6f28bbe6655b15f320ca

###### and therein:
* Name: system.img
* Size: 335544320
* MD5: 50c88cd2314c3f78aadeb6c6f1f5f7ed
* SHA1: 736976b6398a0398903a866ce42ad30b707bba88

##### The original Boeye/Sibrary toolchain, found elsewhere:
* Name: sibrary_toolchain.tar.bz2 
* Size: 116721851 bytes
* MD5: 7dfd9a84d98242e6780e95886a7ce7a2
* SHA1: 43833e54f9e81d9afcd9e06f76f085b5b99fb922

##### QT Anywhere 4.7.4:
* URL: http://master.qt.io/archive/qt/4.7/qt-everywhere-opensource-src-4.7.4.tar.gz
* Size: 220388303
* MD5: 9831cf1dfa8d0689a06c2c54c5c65aaf
* SHA1: af9016aa924a577f7b06ffd28c9773b56d74c939

## TODO:
- [ ] figure out how the device's keys are handled
- [ ] figure out how the device's virtual keyboard is invoked
- [ ] try to build with the stock 2.6.25 kernel - maybe that helps resolving the above issues.
- [ ] maybe include SSL, ALSA, gstreamer, etc...

## QT configure options cheatsheet:

```
  -embedded x86
  -qt-gfx-vnc
  -no-largefile
  -exceptions 
  -no-sse2
  -qt-zlib
  -no-gif
  -no-libtiff
  -qt-libpng
  -no-libmng
  -qt-libjpeg
  -openssl
  -depths 16
  -qt-kbd-linuxinput
  -nomake demos
  -nomake examples
  -qt-mouse-linuxinput
  -qt-mouse-tslib
```

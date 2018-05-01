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

  Kernel version 2.6.25, strings found therein:
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
* GCC v.4.4.3
* The device uses QT linuxfb

  qt-everywhere-opensource-src-4.7.4

## Related works:
* [eView](http://www.the-ebook.org/forum/viewtopic.php?p=1040672#1040672) although works only on GTK firmware

## Prerequisites
1. Install CentOS-6.9
I used netinstall and the "Development Workstation" type of install.
2. Enable the Wheel group in sudoers.conf through _visudo(8)_.
3. Start XQuartz
4. SSH to host and start terminal
5. I remove the GDM and WPA supplicant to conserve some memory; also I disable all but a single TTYs (see /etc/sysconfig/init)
6. install texinfo, EPEL and, optionally, DKMS:
  ```
  yum -y install texinfo epel-release dkms
  ```
7. Optionally install the Parallels tools
8. Switch to background by stopping and restarting VM with the script:
  ```
  $
  ```
9. Do a yum upgrade


## Crosstool-ng

NB: It builds only on Centos-6 due to broken cloog-ppl!

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
  wget
```

#### libdbus-1 (a dependency of QT)

```
  wget
```

### Build:
Example build for Raspberry-Pi: https://beter93.wordpress.com/2013/03/22/how-to-compile-qt-lib-with-crosstool-ng-for-raspberry/
```
  cd
  wget http://master.qt.io/archive/qt/4.7/qt-everywhere-opensource-src-4.7.4.tar.gz 
  tar -xvzf qt-everywhere-opensource-src-4.7.4.tar.gz
  cd qt-everywhere-opensource-src-4.7.4
  cp -R mkspecs/qws/linux-arm-gnueabi-g++ mkspecs/qws/rk2818-g++/
  # Edit the qmake.conf, replace the arm-none-linux-gnueabi-* prefix with the
  # arm-926ejs-linux-gnueabi-* and make any other changes as necessary.
  vi mkspecs/qws/rk2818-g++/qmake.conf
  export PATH="${PATH}:/home/mac/x-tools/arm-926ejs-linux-gnueabi/bin/"

  ./configure -lrt -opensource -confirm-license -prefix /opt/qt-arm \
    -no-qt3support -embedded arm -little-endian \
    -xplatform qws/rk2818-g++ -fast -no-xinput -no-xrandr \
    -no-openvg -no-opengl -no-gtkstyle -no-nis -no-cups -xmlpatterns \
    -exceptions -no-stl -no-accessibility -no-audio-backend -no-multimedia \
    -no-xfixes -no-mitshm -qt-gfx-linuxfb -dbus

  make
```

See no also, SSL, gstreamer, etc...

```
./configure -xplatform qws/linux-i486-g++ -embedded x86 \ 
        -prefix /usr/i486-pc-linux-gnu/usr/local/qt-embedded \
        -qt-gfx-linuxfb -qt-gfx-vnc \
        -no-largefile -exceptions -no-accessibility -no-qt3support -no-sse2 \
        -qt-zlib -no-gif -no-libtiff \
        -qt-libpng -no-libmng -qt-libjpeg -openssl -no-nis -no-cups -depths 16 \
        -qt-kbd-linuxinput -nomake demos -nomake examples \
        -qt-mouse-linuxinput -qt-mouse-tslib \
        -confirm-license
```

### Example:
http://doc.qt.io/qt-5/qtwidgets-tools-echoplugin-example.html

Better yet use demos/embedded/digiflip


## Research

### Unpack the firmware first:
```
imgRePackerRK /cid update.img
```

### Tell the compiler versions:
```
objdump -s --section .comment target-file-path
```
### How the kernel is put together:
[How is the kernel image built for the platform](http://roverbooksteel.narod.ru/debian/5point/kernel.htm)
> ```
> ./mkkrnlimg arch/arm/boot/Image kernel.img.tmp
> cat kernel.img.tmp System.map > kernel.img
> ```
> . Работающие на частоте 600mhz и выше. Таких аппараты часто работают с SDK2-подобным форматом ядра (boot.img = zImage + initramfs-cpio)
>
> 2. Зажатые на ~300mhz (Archos 70b ereader, Bq Voltaire и масса китайских девайсов). Здесь используется свой собственный, скудно документированный формат.
> 
> Утилита mkkrnlimg подписывает несжатое ядро (arch/arm/boot/Image) специальным образом:
> ```
> ./mkkrnlimg arch/arm/boot/Image kernel.img.tmp
> ```
> Ядро склеивается с таблицей адресов System.map
> ```
> cat kernel.img.tmp System.map > kernel.img
> ```
> При распаковке прошивки утилитой rkunpack (на AFPTool не сработает) видно следующее:
> kernel.img-raw - несжатое ядро (т.е. файл arch/arm/boot/Image поcле компиляции)
> kernel.img-symbol - таблица адресов (System.map поcле компиляции)

> Результат работы утилиты mkkrnlimg:
>￼
>
> dword 0x4C4E524B - маркер-идентификатор KRNL
> dword 0x0045D2BC - размер ядра без таблицы адресов
>
> Т.е. mkkrnlimg по функционалу идентична утилите rkcrc из комплекта rkutils, запускаемой с ключом "-k":
> ```
> ./rkcrc -k kernel.img-raw kernel.img-signed
> cat kernel.img-signed kernel.img-symbol > kernel.img
> ```
> При проверке такие ядра полноценно функционировали.
>

### General info on STOCK firmware for G6:
> Немного софта: если положить в память книги или на карточку исполнимый файл либо shell-скрипт и затем открыть его в "проводнике" - он будет выполнен. Это позволяет запускать на книге произвольный код. 

[Russian Sibrary clone hacking page on a prominent gadgets forum](https://4pda.ru/forum/index.php?showtopic=423200&st=20)

### Official Rockchip kernel

Rockchip officially supports RK2818 under Linux: http://linux-rockchip.org.

The kernel GIT repository:
```
git clone https://github.com/linux-rockchip/linux-rockchip.git 
git checkout 21d149db093c0d37e67620b281607844529fd0e8
```
NB: After the above commit, RK2818 support has been discontinued.
NB: Apparently, this is NOT the version used by Boeye/Sibrary (see the strings, contained therein). 

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

## References
### QT:
* [QT Wiki page from Rockchip](http://opensource.rock-chips.com/wiki_Qt)
* [Another installing QT for Raspberry Pi](https://wiki.qt.io/Building_Qt_for_Embedded_Linux)

#### CentOS image used:
* Name: CentOS-6.9-x86_64-netinstall.iso
* Size: 241172480
* MD5: cdab6b8142cb03e5f02102879d11ef44
* SHA1: 32f9f74fd27ec1ff7cc4f39a80d0dae34d9ec18b

#### Official DIGMA firmware:
* URL: http://digma.ru
* Name: e605_boot620_20141220.zip
* Size: 136682454 bytes
* MD5: b739257045ec5f32363db0ff3ef936e1
* SHA1: 6f2c9da77ccf88443c4b6f28bbe6655b15f320ca

And therein:
* Name: system.img
* Size: 335544320
* MD5: 50c88cd2314c3f78aadeb6c6f1f5f7ed
* SHA1: 736976b6398a0398903a866ce42ad30b707bba88

#### The original Boeye/Sibrary toolchain, found elsewhere:
* Name: sibrary_toolchain.tar.bz2 
* Size: 116721851 bytes
* MD5: 7dfd9a84d98242e6780e95886a7ce7a2
* SHA1: 43833e54f9e81d9afcd9e06f76f085b5b99fb922

#### QT Anywhere 4.7.4:
* URL: http://master.qt.io/archive/qt/4.7/qt-everywhere-opensource-src-4.7.4.tar.gz
* Size: 220388303
* MD5: 9831cf1dfa8d0689a06c2c54c5c65aaf
* SHA1: af9016aa924a577f7b06ffd28c9773b56d74c939

## TODO:
* build dbus and support
* figure out how the device's keys are handled
* figure out how the device's virtual keyboard is invoked
* try to build with the stock kernel - maybe that helps resolving the above issues.

### This document:
* mention OSX Disk Utility for image mount
* OSX Wine for rkunpack.exe as no source code is available
* Mention Parallels script.
* Mention kernel header version differences (and official SDK archive name, for that matter).


# Digma E605 E-Ink reader third-party applications framework

## Subject
This is a low-cost E-Ink pearl reader, based on Boeye Sibrary C60 platform and SDK.

The application framework has been switched from GTK to QT 4.7 over the course of the years.

Currently this product is discontinued and unsupported by the vendor.

## Some technical specs:
* Linux boeye 2.6.25-dirty #767 Tue Mar 25 17:00:08 CST 2014 armv5tejl GNU/Linux
* Builds only on Centos-6 due to broken PPL 
* Target chip RK-2818
* ARM926EJC or ARM926EJ-S rev 5 (v5l)
* CPU architecture  5TEJ
* Kernel version 2.6.25 
```
  RK2818 SCU VERSION=20111204,V1.12
  Linux version 2.6.25-dirty (zpp@zpp) (gcc version 4.6.3 (Sourcery CodeBench Lite 2012.03-57) ) #767 Tue Mar 25 17:00:08 CST 2014
  rockchip 281x version release v1.09(Eink) used in Eink, without camera video and touch mabe
  rknand_base.c version: 4.30 20111009
  rknand_buffer.c version: 4.38 20121130
```
* Glibc V2.9 
```
  GNU C Library (crosstool-NG 1.12.4) stable release version 2.9, by Roland McGrath et al.
```
* GCC v.4.4.3
* The device uses QT linuxfb
* Crosstool-ng v. 1.12.4 (1.10.0 for the Android system)
* qt-everywhere-opensource-src-4.7.4


NB: crosstool-ng DOES NOT build on OSX. One of the reasons - case-insensitive FS


## Related works:
* [http://www.the-ebook.org/forum/viewtopic.php?p=1040672#1040672][eView] although works only on GTK firmware

## QT Refernces:
* [http://opensource.rock-chips.com/wiki_Qt][QT Wiki page from Rockchip]
* [https://wiki.qt.io/Building_Qt_for_Embedded_Linux][Installing QT for Raspberry Pi]

## How the kernel is put together:
[http://roverbooksteel.narod.ru/debian/5point/kernel.htm][How is the kernel image built for the platform]
> ```
> ./mkkrnlimg arch/arm/boot/Image kernel.img.tmp
> cat kernel.img.tmp System.map > kernel.img
> ```
> . Работающие на частоте 600mhz и выше. Таких аппараты часто работают с SDK2-подобным форматом ядра (boot.img = zImage + initramfs-cpio)
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

## General info on STOCK firmware for G6:
> Немного софта: если положить в память книги или на карточку исполнимый файл либо shell-скрипт и затем открыть его в "проводнике" - он будет выполнен. Это позволяет запускать на книге произвольный код. 

[https://4pda.ru/forum/index.php?showtopic=423200&st=20][Russian Sibrary clone hacking page on a prominent gadgets forum]

## Crosstools-ng bootstrap:
[https://crosstool-ng.github.io/docs/install/]
```
git clone https://github.com/crosstool-ng/crosstool-ng.git
git checkout ...
```

## QT bootstrap:
https://beter93.wordpress.com/2013/03/22/how-to-compile-qt-lib-with-crosstool-ng-for-raspberry/

## QT Example:
http://doc.qt.io/qt-5/qtwidgets-tools-echoplugin-example.html
Better yet use demos/embedded/digiflip

## Tell the compiler versions:
```
objdump -s --section .comment
```

## Unpack the firmware first:
```
imgRePackerRK /cid update.img
```

## Build QT:
```
export PATH="${PATH}:/home/mac/x-tools/arm-unknown-linux-gnueabi/bin/«

./configure -lrt -opensource -confirm-license -prefix /opt/qt-arm -no-qt3support -embedded arm -little-endian -xplatform qws/linux-arm-gnueabi-g++ -fast -no-xinput -no-xrandr -no-openvg -no-opengl -no-gtkstyle -no-nis -no-cups -xmlpatterns -exceptions -no-stl -no-accessibility -no-audio-backend -no-multimedia -no-xfixes -no-mitshm -qt-gfx-linuxfb
```

See no also, SSL, gstreamer, etc...

```
./configure -xplatform qws/linux-i486-g++ -embedded x86 
        -prefix /usr/i486-pc-linux-gnu/usr/local/qt-embedded
        -qt-gfx-linuxfb -qt-gfx-vnc
        -no-largefile -exceptions -no-accessibility -no-qt3support -no-sse2 -qt-zlib -no-gif -no-libtiff
        -qt-libpng -no-libmng -qt-libjpeg -openssl -no-nis -no-cups -depths 16
        -qt-kbd-linuxinput -nomake demos -nomake examples
        -qt-mouse-linuxinput -qt-mouse-tslib
        -confirm-license
```

## Plan:
* OS-disk utility for image mount
* OS wine for rkunpack
* Mention exact CentOS version
* Mention exact DIGMA update version
* Mention Parallels incl. script or QEMU or Docker
* Mention kernel header version differences (and official SDK archive name, for that matter).
* sdk headers to GIT, dist target to Makefile
* link to official rockchip-linux
* mention of rk2818 stuff removed

## References:

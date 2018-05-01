--- src/corelib/io/io.pri.orig	2011-09-12 02:49:29.000000000 -0400
+++ src/corelib/io/io.pri	2018-05-01 12:05:06.219365636 -0400
@@ -74,7 +74,7 @@
             SOURCES += io/qsettings_mac.cpp io/qfilesystemwatcher_fsevents.cpp
         }
 
-        linux-*:!symbian {
+        linux-*|arm-926ejs-linux-gnueabi:!symbian {
             SOURCES += \
                     io/qfilesystemwatcher_inotify.cpp \
                     io/qfilesystemwatcher_dnotify.cpp

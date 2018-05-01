--- qt-everywhere-opensource-src-4.7.4/src/corelib/io/io.pri.orig	2018-05-01 12:35:29.345365860 -0400
+++ qt-everywhere-opensource-src-4.7.4/src/corelib/io/io.pri	2018-05-01 12:34:29.956365883 -0400
@@ -74,7 +74,7 @@
             SOURCES += io/qsettings_mac.cpp io/qfilesystemwatcher_fsevents.cpp
         }
 
-        linux-*:!symbian {
+        linux-*|arm-926ejs-linux-gnueabi-*|rk2818-*:!symbian {
             SOURCES += \
                     io/qfilesystemwatcher_inotify.cpp \
                     io/qfilesystemwatcher_dnotify.cpp

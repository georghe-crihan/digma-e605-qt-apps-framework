// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
/**
 * Copyright (c) 2011 Alexander Drozdoff <hatred@inbox.ru>
 * Update for Qt support by S-trace <s-trace@list.ru>, 2013
 * Ditributed under GPLv2 Terms
 * http://hatred.homelinux.net/~hatred/digma
 */
//#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h> /* atoi() */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h> /* dlopen() */
#include <pthread.h>
#include <linux/fb.h>
//#include "gtk_file_manager.h" /* Инклюдить первой среди своих, ибо typedef panel! */
#include "digma_hw.h"
#include <errno.h>
//#include "kobo_hw.h"
//#include "mylib.h"
//#include "mygtk.h"

static int refresh_type = REFRESH_NEW;
int framebuffer_descriptor = 0;

void write_string_to_file(const char *file, const char *value);
void write_int_to_file(const char *file, int value);

int hardware_has_backlight=FALSE, hardware_has_LED=FALSE, hardware_has_APM=FALSE, hardware_has_sysfs_sleep=FALSE, hardware_has_eink_mode_control=FALSE;
const char *LED_path, *backlight_path, *sysfs_sleep_path, *eink_mode_control_path;
int LED_state[LED_STATES]; /* Массив содержащий значения которые надо писать в sysfs чтобы управлять LED */
int previous_backlight_level; /* Уровень подсветки перед запуском eView */
int suspended=FALSE; /* Текущее состояние книги */
int was_in_picture_viewer=FALSE;
int hw_platform=HW_PLATFORM_UNKNOWN; /* Hardware platform detected by eView */
pthread_t suspend_helper_tid;

static int enable_refresh = TRUE; /* Принудительно запретить обновлять экран в особых случаях */
static int LED_notify = FALSE; /* Оповещение светодиодом об обновлении панелей и загрузке */

int (*apm_suspend)(int fd); /* Функция из libapm.so, загружается через dlopen */

/* Helper FB update function */
int epaper_update_helper(int fb, unsigned long int ioctl_call, void *mode)
{
  if (framebuffer_descriptor >= 0)
  {
    errno=0;
    ioctl(fb, ioctl_call, mode);
//    sleep_timer=sleep_timeout; // Reset sleep timer on every display refresh = we are not have any constantly refreshing display parts now!
    return errno;
  }
  return TRUE;
}

void epaperUpdate(__attribute__((unused)) unsigned long int ioctl_call, __attribute__((unused)) void *mode)
{
  #ifndef __amd64
  int ioctl_result;
  #endif //__amd64
  TRACE("Called void epaperUpdate()\n");
  if (enable_refresh == FALSE)
  {
    TRACE("Display refresh was locked, IGNORED!\n");
    (void) ioctl_call;
    (void) mode;
    return;
  }
  #ifndef __amd64
  /* Иначе запись в видеопамять не успевает завершиться и получаем верхний левый угол новой картинки и нижний правый - прежней. */
//  if (hw_platform != HW_PLATFORM_SIBRARY_GTK) {
//    const struct timespec delay = {0, QT_REFRESH_DELAY};
//    nanosleep(&delay, NULL);
//  }
  ioctl_result=epaper_update_helper(framebuffer_descriptor, ioctl_call, mode);
  #ifdef debug
  if (ioctl_result != 0)
  {
    TRACE("Display refresh ioctl call FAILED %d (%s)\n", ioctl_result, strerror(ioctl_result));
    // GTK прошивка, обновление от Qt: 1 (Операция не позволяется)
    // Qt прошивка, обновление от GTK: 22 (Недопустимый аргумент)
  }
  #else
  (void) ioctl_result;
  #endif
  #endif
  return;
}


int detect_refresh_type (void)
{
  int mode=3;
#if 0
  struct mxcfb_update_data data = {
    .update_region =
    { .top = 0,
      .left = 0,
      .width = 1,
      .height = 1
    },
    .update_mode = UPDATE_MODE_FULL,
    .update_marker = 0,
    .waveform_mode = WAVEFORM_MODE_AUTO,
    .temp = TEMP_USE_AMBIENT,
    .flags = 0
  };
#endif
  if (epaper_update_helper(framebuffer_descriptor, EPAPER_UPDATE_DISPLAY_QT, &mode) == 0)
  {
    refresh_type=REFRESH_NEW;
    TRACE("Display refresh was successed, new\n");
  }
  else if (epaper_update_helper(framebuffer_descriptor, EPAPER_UPDATE_FULL, &mode) == 0)
  {
    refresh_type=REFRESH_LEGACY;
    TRACE("Display refresh was successed, legacy\n");
  }
//  else if (epaper_update_helper(framebuffer_descriptor, MXCFB_SEND_UPDATE_ORG, &data) == 0)
//  {
//    refresh_type=REFRESH_KOBO;
//    TRACE("Display refresh was successed, kobo\n");
//  }
  else
  {
    TRACE("Display refresh was not detected!\n");
  }
  return (refresh_type);
}

/**
 * Update E-Ink screen fully
 */
void *epaperUpdateFull(void *arg)
{
  int mode = 3;
#if 0
  static int marker = 0;
  struct fb_var_screeninfo vinfo;
  struct mxcfb_update_data data = {
    .update_region =
        { .top = 0,
          .left = 0,
          .width = 0,
          .height = 0
        },
	.update_mode = UPDATE_MODE_FULL,
	.update_marker = ++marker,
	.waveform_mode = WAVEFORM_MODE_AUTO,
	.temp = TEMP_USE_AMBIENT,
	.flags = 0
  };
#endif
  (void)arg;
  TRACE("epaperUpdateFull()\n");  
  if (hardware_has_eink_mode_control)
  {
    write_string_to_file (eink_mode_control_path, "m");
    write_int_to_file(eink_mode_control_path, 0);
  }

#if 0
  if (ioctl(framebuffer_descriptor, FBIOGET_VSCREENINFO, &vinfo) == -1) {
    TRACE("Error reading variable framebuffer information\n");
  }

  data.update_region.width  = (int) vinfo.xres;
  data.update_region.height = (int) vinfo.yres;
#endif

  if (refresh_type == REFRESH_NEW)
    epaperUpdate(EPAPER_UPDATE_DISPLAY_QT, &mode);
//  else if (refresh_type == REFRESH_LEGACY)
//    epaperUpdate(EPAPER_UPDATE_FULL, &mode);
//  else if (refresh_type == REFRESH_KOBO) {
//    epaperUpdate(MXCFB_SEND_UPDATE_ORG, &data);
//    epaperUpdate(MXCFB_WAIT_FOR_UPDATE_COMPLETE, &data.update_marker);
//  }
  else
  {
    TRACE("Unable to refresh display - refresh type is unknown!\n");
  }

  if (hardware_has_eink_mode_control)
  {
    write_string_to_file (eink_mode_control_path, "q");
  }
  return NULL;
}

/**
 * Update only local (???)
 */
void *epaperUpdateLocal(void *arg)
{
  int mode = 2;
#if 0
  static int marker = 0;
  struct fb_var_screeninfo vinfo;
  struct mxcfb_update_data data = {
    .update_region =
        { .top = 0,
          .left = 0,
          .width = 0,
          .height = 0
        },
        .update_mode = UPDATE_MODE_PARTIAL,
	.update_marker = ++marker,
	.waveform_mode = WAVEFORM_MODE_AUTO,
	.temp = TEMP_USE_AMBIENT,
	.flags = 0
  };
#endif
  (void)arg;
  TRACE("epaperUpdateLocal()\n");  
  if (hardware_has_eink_mode_control)
  {
    write_string_to_file (eink_mode_control_path, "m");
    write_int_to_file(eink_mode_control_path, 2);
  }

#if 0
  if (ioctl(framebuffer_descriptor, FBIOGET_VSCREENINFO, &vinfo) == -1) {
    TRACE("Error reading variable framebuffer information\n");
  }

  data.update_region.width  = (int) vinfo.xres;
  data.update_region.height = (int) vinfo.yres;
#endif

  if (refresh_type == REFRESH_NEW)
    epaperUpdate(EPAPER_UPDATE_DISPLAY_QT, &mode);
//  else if (refresh_type == REFRESH_LEGACY)
//    epaperUpdate(EPAPER_UPDATE_LOCAL, &mode);
//  else if (refresh_type == REFRESH_KOBO) {
//    epaperUpdate(MXCFB_SEND_UPDATE_ORG, &data);
//    epaperUpdate(MXCFB_WAIT_FOR_UPDATE_COMPLETE, &data.update_marker);
//  }
  else
  {
    TRACE("Unable to refresh display - refresh type is unknown!\n");
  }

  if (hardware_has_eink_mode_control)
  {
    write_string_to_file (eink_mode_control_path, "q");
  }
  return NULL;
}

/**
 * Untested E-Ink function
 */
void *epaperUpdatePart(void *arg)
{
  int mode = 1;
#if 0
  static int marker = 0;
  struct fb_var_screeninfo vinfo;
  struct mxcfb_update_data data = {
    .update_region =
        { .top = 0,
          .left = 0,
          .width = 0,
          .height = 0
        },
        .update_mode = UPDATE_MODE_PARTIAL,
	.update_marker = ++marker,
	.waveform_mode = WAVEFORM_MODE_AUTO,
	.temp = TEMP_USE_AMBIENT,
	.flags = 0
  };
#endif
  (void)arg;
  TRACE("epaperUpdatePart()\n");  

  if (hardware_has_eink_mode_control)
  {
    write_string_to_file (eink_mode_control_path, "m");
    write_int_to_file(eink_mode_control_path, 5);
  }

#if 0
  if (ioctl(framebuffer_descriptor, FBIOGET_VSCREENINFO, &vinfo) == -1) {
    TRACE("Error reading variable framebuffer information\n");
  }

  data.update_region.width  = (int) vinfo.xres;
  data.update_region.height = (int) vinfo.yres;
#endif

  if (refresh_type == REFRESH_NEW)
    epaperUpdate(EPAPER_UPDATE_DISPLAY_QT, &mode);
//  else if (refresh_type == REFRESH_LEGACY)
//    epaperUpdate(EPAPER_UPDATE_LOCAL, &mode);
//  else if (refresh_type == REFRESH_KOBO) {
//    epaperUpdate(MXCFB_SEND_UPDATE_ORG, &data);
//    epaperUpdate(MXCFB_WAIT_FOR_UPDATE_COMPLETE, &data.update_marker);
//  }
  else
  {
    TRACE("Unable to refresh display - refresh type is unknown!\n");
  }

  if (hardware_has_eink_mode_control)
  {
    write_string_to_file (eink_mode_control_path, "q");
  }
  return NULL;
}

int check_for_file (const char *fpath)  /* Проверка наличия файла в файловой системе */
{
  if (access(fpath, F_OK))  /* F_OK просто проверяет существование файла */
    return FALSE;
  else
    return TRUE;
}

int read_int_from_file(const char *name) /*Чтение числа из файла name */
{
  FILE *file_descriptor=fopen(name,"rt");
  if (!file_descriptor)
  {
    TRACE("UNABLE TO OPEN %s FILE FOR READ!\n", name);
    return 0;
  }
  else
  {
    char temp[256];
    if (fgets(temp, 256, file_descriptor) == 0)
    {
      (void)fclose(file_descriptor);
      TRACE("Reading from %s failed!\n", name);
      return 0;
    }
    (void)fclose(file_descriptor);
    TRACE("Read '%s' from %s\n", temp, name);
    return (atoi (temp));
  }
}

// cppcheck-suppress "unusedFunction"
void detect_hardware(void) /* Обнаружение оборудования и его возможностей */
{
  TRACE("Detecting hardware\n");

  if (access("/sys/power/state-extended", F_OK) == 0)
  {
      TRACE("Found extended power control at file /sys/power/state-extended - looks like we are running on KOBO\n");
      hw_platform = HW_PLATFORM_KOBO;
  }

  if (hardware_has_backlight == FALSE) /* Digma R60G/GMini C6LHD (Qt) */
  {
    hardware_has_backlight=check_for_file ("/sys/class/backlight/boeye_backlight/brightness");
    if (hardware_has_backlight)
    {
      backlight_path="/sys/class/backlight/boeye_backlight/brightness";
      TRACE("Found backlight control at file %s\n", backlight_path);
      previous_backlight_level=read_int_from_file(backlight_path);
    } else {
      hardware_has_backlight=check_for_file ("/dev/ntx_io");
      if (hardware_has_backlight) {
        TRACE("Found ntx_io backlight control\n");
      }
    }
  }

  if (hardware_has_LED == FALSE) /* Digma R60G/GMini C6LHD (Qt) */
  {
    hardware_has_LED=check_for_file ("/sys/class/leds/charger-led/brightness");
    if (hardware_has_LED)
    {
      LED_path="/sys/class/leds/charger-led/brightness";
      TRACE("Found LED control at file %s\n", LED_path);
      LED_state[LED_ON]=2; /* Неверно! */
      LED_state[LED_OFF]=0;
      LED_state[LED_BLINK_SLOW]=1;
      LED_state[LED_BLINK_FAST]=4;
    }
  }

  if (hardware_has_LED == FALSE) /* Ritmix RBK700HD GTK/Qt */
  {
    hardware_has_LED=check_for_file ("/sys/class/leds/axp192-led-classdev/brightness");
    if (hardware_has_LED)
    {
      LED_path="/sys/class/leds/axp192-led-classdev/brightness";
      LED_state[LED_ON]=3;
      LED_state[LED_OFF]=4;
      LED_state[LED_BLINK_SLOW]=5;
      LED_state[LED_BLINK_FAST]=6;
      TRACE("Found LED control at file %s\n", LED_path);
    }
  }

  if (hardware_has_LED == FALSE) /* Digma E600 GTK */
  {
    hardware_has_LED=check_for_file ("/sys/devices/platform/boeye-leds/leds/da9030_led/brightness");
    if (hardware_has_LED)
    {
      LED_path="/sys/devices/platform/boeye-leds/leds/da9030_led/brightness";
      LED_state[LED_ON]=3; /* Проверить! */
      LED_state[LED_OFF]=4;
      LED_state[LED_BLINK_SLOW]=5;
      LED_state[LED_BLINK_FAST]=6;
      TRACE("Found LED control at file %s\n", LED_path);
    }
  }

  if (hardware_has_LED == FALSE) /* Kobo Aura H2O */
  {
    hardware_has_LED=check_for_file ("/sys/class/leds/pmic_ledsg/brightness");
    if (hardware_has_LED)
    {
      LED_path="/sys/class/leds/pmic_ledsg/brightness";
      LED_state[LED_ON]=255;
      LED_state[LED_OFF]=0;
      LED_state[LED_BLINK_SLOW]=255; // Not supported
      LED_state[LED_BLINK_FAST]=255; // Not supported
      TRACE("Found LED control at file %s\n", LED_path);
    }
  }

  if (hardware_has_APM == FALSE)
  {
    hardware_has_APM=check_for_file ("/dev/apm_bios");
    if (hardware_has_APM)
    {
      void *libapm_handle;
      char *error;
      (void)dlerror();    /* Clear any existing error */
      libapm_handle=dlopen("libapm.so", RTLD_NOW);
      error = dlerror();
      if (error != NULL)
      {
        TRACE("dlopen failed because %s\n", error);
        //         free(error); // Не надо - карается сегфолтом в GTK гораздо позже!
        hardware_has_APM=FALSE; /* Не можем управлять через APM, хотя существование файла в /dev/ даёт робкую надежду */
      }
      else
      {
        apm_suspend=(int (*)(int))dlsym(libapm_handle,"apm_suspend");
        error = dlerror();
        if (error != NULL)
        {
          TRACE("dlsym failed because %s\n", error);
          //         free(error); // Не надо - карается сегфолтом в GTK гораздо позже!
          hardware_has_APM=FALSE;
        }
        else
        {
          TRACE("Found APM power control via libapm\n");
        }
        //       free(libapm_handle); // Не надо - карается МОЛЧАЛИВЫМ сегфолтом
      }
    }
  }

  if (hardware_has_sysfs_sleep == FALSE)
  {
    hardware_has_sysfs_sleep=check_for_file ("/sys/power/state");
    if (hardware_has_sysfs_sleep)
    {
      sysfs_sleep_path="/sys/power/state";
      TRACE("Found sysfs sleep trigger at file %s\n", sysfs_sleep_path);
    }
  }

  if (hardware_has_eink_mode_control == FALSE)
  {
    hardware_has_eink_mode_control=check_for_file ("/sys/eink/einkmode");
    if (hardware_has_eink_mode_control)
    {
      eink_mode_control_path="/sys/eink/einkmode";
      TRACE("Found eInk mode control at file %s\n", eink_mode_control_path);
    }
  }

  #ifdef debug
  if (! hardware_has_LED)
    TRACE("LED control not found\n");
  if (! hardware_has_backlight)
    TRACE("Backlight control not found\n");
  if (! hardware_has_APM)
    TRACE("APM not found\n");
  if (! hardware_has_sysfs_sleep)
    TRACE("Sysfs sleep trigger not found\n");
  if (! hardware_has_eink_mode_control)
    TRACE("eInk mode control file not found\n");
  TRACE("Hardware detect finished\n");
  #endif

}

void write_int_to_file(const char *file, int value)
{
  FILE *file_descriptor;
  TRACE("writing %d to %s\n", value, file);
  file_descriptor=fopen(file,"wt");
  if (!file_descriptor)
  {
    TRACE("UNABLE TO OPEN %s FILE FOR WRITING!\n", file);
    return;
  }
  else
  {
    fprintf(file_descriptor, "%d", value);
    (void)fclose(file_descriptor);
  }
}

void write_string_to_file(const char *file, const char *value)
{
  FILE *file_descriptor;
  TRACE("writing %s to %s\n", value, file);
  file_descriptor=fopen(file,"wt");
  if (!file_descriptor)
  {
    TRACE("UNABLE TO OPEN %s FILE FOR WRITING!\n", file);
    return ;
  }
  else
  {
    fprintf(file_descriptor, "%s", value);
    (void)fclose(file_descriptor);
  }
}

void set_brightness(int value)
{
  if (hw_platform == HW_PLATFORM_KOBO) {
    int ntx_io_fd=open("/dev/ntx_io", O_RDWR);
    if (ntx_io_fd >= 0)
    {
      if (value > 100)
        value=100; // ntx_io accepts 0...100 as IOCTL arg, not 0...255
      ioctl(ntx_io_fd, 0xf1, value);
      close(ntx_io_fd);
    }
  }
  else if (hardware_has_backlight)
    write_int_to_file(backlight_path, value);
}

void set_led_state (int state)
{
  if ((LED_notify == TRUE) && (hardware_has_LED == TRUE))
    write_int_to_file(LED_path, LED_state[state]);
}

void kobo_enter_sleep(int deep_sleep) {
  if (hw_platform == HW_PLATFORM_KOBO) {
    const struct timespec delay = {2, 0};
    write_int_to_file ("/sys/power/state-extended", deep_sleep == FALSE ? 0 : 1);
    nanosleep(&delay, NULL); // 2 seconds
    sync();
    write_string_to_file ("/sys/power/state","mem");
    write_int_to_file ("/sys/power/state-extended", 0);
  }
}

#if 0
void *suspend_hardware_helper(struct_panel *panel)
{
  int count=0;
  (void)fflush (stdout);
  sync();
  TRACE("Suspending hardware\n");
  do
  {
    time_t startTime, endTime;
    double duration;
    const struct timespec delay = {0, 100000000};
    #ifdef debug
    if (LED_notify)
      set_led_state(LED_ON);
    #endif
    set_brightness(0);
    startTime = time(NULL);
    if (hardware_has_APM)
    {
      int apm_bios;
      TRACE("Using APM\n");
      apm_bios=open("/dev/apm_bios", O_RDWR);
      (void)(*apm_suspend) (apm_bios);
      (void)close(apm_bios);
    }
    else if (hw_platform == HW_PLATFORM_KOBO) {
      kobo_enter_sleep(TRUE); // TRUE = deep_sleep
    }
    else if (hardware_has_sysfs_sleep)
    {
      TRACE("Using sysfs trigger\n");
      write_string_to_file (sysfs_sleep_path,"mem");
    }
    else
    {
      TRACE("Unable to suspend hardware - no ways to suspend!\n");
      break;
    }
    endTime = time(NULL);
    duration = difftime(endTime, startTime);
    if ((int) duration <= 5)
    {
      TRACE("Hardware suspend failed\n");
    }
    else
    {
      #ifdef debug
      TRACE("Hardware suspend successed after %d attempts (sleeped %4.0f seconds)\n", count, duration);
      if (LED_notify)
        set_led_state(LED_OFF);
      #endif
      break;
    }
    #ifdef debug
    if (LED_notify)
      set_led_state(LED_ON);
    #endif
    if (count++>128)
    {
      TRACE("Too many failed attempts, break\n");
      break;
    }
    nanosleep(&delay, NULL); //0,1 seconds
  }
  while (TRUE);
  TRACE("Waking up device!\n");
  wake_up_device(panel);
  return NULL;
}

void suspend_hardware(struct_panel *panel)
{
  if(pthread_create(&suspend_helper_tid, NULL, (void * (*)(void *))suspend_hardware_helper, (void *)panel) != 0)
  {
    TRACE("Unable to start hardware sleep helper!\n");
  }
}
#endif

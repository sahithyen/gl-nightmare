#if !defined(NIGHTMARE_USE_GLES1) && !defined(NIGHTMARE_USE_GLES2)
#error "GL version not selected via a symbol"
#endif

#include <stdbool.h>

#include "signal-handler.h"
#include "scenes.h"
#include "common.h"
#include "egl.h"
#include "random.h"

#ifdef NIGHTMARE_USE_GLES1
#include <GLES/gl.h>
#elif defined NIGHTMARE_USE_GLES2
#include <GLES2/gl2.h>
#endif

int main()
{
   int status_code = 1;

   print("       _       _     _\n");
   print("      (_)     | |   | |\n");
   print(" _ __  _  __ _| |__ | |_ _ __ ___   __ _ _ __ ___\n");
   print("| '_ \\| |/ _` | '_ \\| __| '_ ` _ \\ / _` | '__/ _\\\n");
   print("| | | | | (_| | | | | |_| | | | | | (_| | | |  __/\n");
   print("|_| |_|_|\\__, |_| |_|\\__|_| |_| |_|\\__,_|_|  \\___|\n");
   print("          __/ |\n");
   print("         |___/\n");
   print("\n");

   // Initialize modules
   initialize_signal_handler();
   initialize_random();
   if (!initialize_egl())
   {
      print_error("Failed to initialize EGL\n");
      goto failure;
   }

   // Print environment info
   const GLubyte *vendor = glGetString(GL_VENDOR);
   const GLubyte *renderer = glGetString(GL_RENDERER);
   const GLubyte *version = glGetString(GL_VERSION);
   print("Environment information\n");
   print("-----------------------\n");
   print("EGL version : %i.%i\n", egl_major, egl_minor);
   print("GL vendor   : %s\n", vendor);
   print("GL renderer : %s\n", renderer);
   print("GL version  : %s\n", version);
   print("\n");

   // Run scenes
   if (!run_scenes())
   {
      print_error("Failed to run scenes\n");
      goto failure;
   }

   status_code = 0;
failure:
   cleanup_egl();
   return status_code;
}

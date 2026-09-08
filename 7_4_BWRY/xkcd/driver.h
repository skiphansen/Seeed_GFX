#define USE_XIAO_EPAPER_DISPLAY_BOARD_EE04

#ifdef E2741QS0B3
   #define CUSTOM_INIT        "E2741QS0B3_Init.h"
   #define CUSTOM_DEFINES     "E2741QS0B3_Defines.h"
//   #define CUSTOM_DRIVER      "E2741QS0B3_Driver.h"
   #define CUSTOM_ROTATION    "E2741QS0B3_Rotation.h"
   #define CUSTOM_USER_SETUP  "E2741QS0B3_Setup.h"
#else
   #if 1
      #define CUSTOM_INIT        "79667_Init.h"
      #define CUSTOM_DEFINES     "79667_Defines.h"
      #define CUSTOM_ROTATION    "79667_Rotation.h"
      #define CUSTOM_USER_SETUP  "Setup_2inch9_BWRY.h"
   #else
      #define BOARD_SCREEN_COMBO 512 // 2.9 inch BWRY ePaper Screen (JD79667)
   #endif
#endif



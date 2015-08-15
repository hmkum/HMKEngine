#pragma once

/* The unknown key */
#define HMK_KEY_UNKNOWN            -1

/* Printable keys */
#define HMK_KEY_SPACE              32
#define HMK_KEY_APOSTROPHE         39  /* ' */
#define HMK_KEY_COMMA              44  /* , */
#define HMK_KEY_MINUS              45  /* - */
#define HMK_KEY_PERIOD             46  /* . */
#define HMK_KEY_SLASH              47  /* / */
#define HMK_KEY_0                  48
#define HMK_KEY_1                  49
#define HMK_KEY_2                  50
#define HMK_KEY_3                  51
#define HMK_KEY_4                  52
#define HMK_KEY_5                  53
#define HMK_KEY_6                  54
#define HMK_KEY_7                  55
#define HMK_KEY_8                  56
#define HMK_KEY_9                  57
#define HMK_KEY_SEMICOLON          59  /* ; */
#define HMK_KEY_EQUAL              61  /* = */
#define HMK_KEY_A                  65
#define HMK_KEY_B                  66
#define HMK_KEY_C                  67
#define HMK_KEY_D                  68
#define HMK_KEY_E                  69
#define HMK_KEY_F                  70
#define HMK_KEY_G                  71
#define HMK_KEY_H                  72
#define HMK_KEY_I                  73
#define HMK_KEY_J                  74
#define HMK_KEY_K                  75
#define HMK_KEY_L                  76
#define HMK_KEY_M                  77
#define HMK_KEY_N                  78
#define HMK_KEY_O                  79
#define HMK_KEY_P                  80
#define HMK_KEY_Q                  81
#define HMK_KEY_R                  82
#define HMK_KEY_S                  83
#define HMK_KEY_T                  84
#define HMK_KEY_U                  85
#define HMK_KEY_V                  86
#define HMK_KEY_W                  87
#define HMK_KEY_X                  88
#define HMK_KEY_Y                  89
#define HMK_KEY_Z                  90
#define HMK_KEY_LEFT_BRACKET       91  /* [ */
#define HMK_KEY_BACKSLASH          92  /* \ */
#define HMK_KEY_RIGHT_BRACKET      93  /* ] */
#define HMK_KEY_GRAVE_ACCENT       96  /* ` */
#define HMK_KEY_WORLD_1            161 /* non-US #1 */
#define HMK_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define HMK_KEY_ESCAPE             256
#define HMK_KEY_ENTER              257
#define HMK_KEY_TAB                258
#define HMK_KEY_BACKSPACE          259
#define HMK_KEY_INSERT             260
#define HMK_KEY_DELETE             261
#define HMK_KEY_RIGHT              262
#define HMK_KEY_LEFT               263
#define HMK_KEY_DOWN               264
#define HMK_KEY_UP                 265
#define HMK_KEY_PAGE_UP            266
#define HMK_KEY_PAGE_DOWN          267
#define HMK_KEY_HOME               268
#define HMK_KEY_END                269
#define HMK_KEY_CAPS_LOCK          280
#define HMK_KEY_SCROLL_LOCK        281
#define HMK_KEY_NUM_LOCK           282
#define HMK_KEY_PRINT_SCREEN       283
#define HMK_KEY_PAUSE              284
#define HMK_KEY_F1                 290
#define HMK_KEY_F2                 291
#define HMK_KEY_F3                 292
#define HMK_KEY_F4                 293
#define HMK_KEY_F5                 294
#define HMK_KEY_F6                 295
#define HMK_KEY_F7                 296
#define HMK_KEY_F8                 297
#define HMK_KEY_F9                 298
#define HMK_KEY_F10                299
#define HMK_KEY_F11                300
#define HMK_KEY_F12                301
#define HMK_KEY_F13                302
#define HMK_KEY_F14                303
#define HMK_KEY_F15                304
#define HMK_KEY_F16                305
#define HMK_KEY_F17                306
#define HMK_KEY_F18                307
#define HMK_KEY_F19                308
#define HMK_KEY_F20                309
#define HMK_KEY_F21                310
#define HMK_KEY_F22                311
#define HMK_KEY_F23                312
#define HMK_KEY_F24                313
#define HMK_KEY_F25                314
#define HMK_KEY_KP_0               320
#define HMK_KEY_KP_1               321
#define HMK_KEY_KP_2               322
#define HMK_KEY_KP_3               323
#define HMK_KEY_KP_4               324
#define HMK_KEY_KP_5               325
#define HMK_KEY_KP_6               326
#define HMK_KEY_KP_7               327
#define HMK_KEY_KP_8               328
#define HMK_KEY_KP_9               329
#define HMK_KEY_KP_DECIMAL         330
#define HMK_KEY_KP_DIVIDE          331
#define HMK_KEY_KP_MULTIPLY        332
#define HMK_KEY_KP_SUBTRACT        333
#define HMK_KEY_KP_ADD             334
#define HMK_KEY_KP_ENTER           335
#define HMK_KEY_KP_EQUAL           336
#define HMK_KEY_LEFT_SHIFT         340
#define HMK_KEY_LEFT_CONTROL       341
#define HMK_KEY_LEFT_ALT           342
#define HMK_KEY_LEFT_SUPER         343
#define HMK_KEY_RIGHT_SHIFT        344
#define HMK_KEY_RIGHT_CONTROL      345
#define HMK_KEY_RIGHT_ALT          346
#define HMK_KEY_RIGHT_SUPER        347
#define HMK_KEY_MENU               348
#define HMK_KEY_LAST               HMK_KEY_MENU

#define HMK_RELEASE                0
#define HMK_PRESS                  1
#define HMK_REPEAT                 2
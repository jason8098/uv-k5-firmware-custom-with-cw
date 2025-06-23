#include <stdbool.h>
#include <string.h>
#include "app/morse.h"
#include "dcs.h"
#include "driver/st7565.h"
#include "external/printf/printf.h"
#include "misc.h"
#include "ui/helper.h"
#include "ui/morse.h"

#include "ui/ui.h"
#include "driver/bk4819.h"
void UI_DisplayMORSE(void)
{
UI_DisplayClear();
UI_PrintString("FOX HUNT TX v0.1", 0, 0, 0, 8);
 ST7565_BlitFullScreen();
}
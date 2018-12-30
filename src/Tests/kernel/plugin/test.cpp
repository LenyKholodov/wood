#define  DEBUG
#include <kernel\thread.h>
#include <kernel\plugin.h>

void     main ()
{
  Plugin::LoadPlugins (M_GetCurrentThread (),"");
}
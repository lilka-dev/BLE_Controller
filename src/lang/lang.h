#ifndef LANG_H
#define LANG_H

// Language selection via build flags:
// -DLANG_UK for Ukrainian
// -DLANG_EN for English (default)

#if defined(LANG_UK)
#include "lang_uk.h"
#else
#include "lang_en.h"
#endif

#endif // LANG_H

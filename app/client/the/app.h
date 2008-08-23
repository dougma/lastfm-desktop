#include "../App.h"

namespace The
{
    inline App& app() { return *(App*)qApp; }
}

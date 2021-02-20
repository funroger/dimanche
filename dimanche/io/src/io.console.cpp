
#include <dimanche/io/console.h>

#if defined(_WINDOWS)
#include <windows.h>
#else // !defined(_WINDOWS)
#endif // defined(_WINDOWS)

namespace dimanche {
namespace io {
namespace con {
namespace font {

#if defined(_WINDOWS)

const COLOR green(void) {
    return COLOR {FOREGROUND_GREEN | FOREGROUND_INTENSITY};}
const COLOR red(void) {
    return COLOR {FOREGROUND_RED | FOREGROUND_INTENSITY};}
const COLOR reset(void) {
    return COLOR {FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED};}
const COLOR yellow(void) {
    return COLOR {FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY};}
const COLOR white(void) {
    return COLOR {FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY};}

#else // !defined(_WINDOWS)

#endif // defined(_WINDOWS)

} // namespace font
} // namespace con
} // namespace io
} // namespace dimanche

std::ostream & operator << (std::ostream &out,
    const dimanche::io::con::COLOR && color)
{
#if defined(_WINDOWS)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color.color);
#else // !defined(_WINDOWS)
    UNUSED(color);
#endif // defined(_WINDOWS)

    return out;

} // std::ostream & operator << (std::ostream &out,

std::wostream & operator << (std::wostream &out,
    const dimanche::io::con::COLOR && color)
{
#if defined(_WINDOWS)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color.color);
#else // !defined(_WINDOWS)
    UNUSED(color);
#endif // defined(_WINDOWS)

    return out;

} // std::wostream & operator << (std::wostream &out,

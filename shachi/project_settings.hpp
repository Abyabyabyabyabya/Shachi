#ifndef INCLUDED_SHACHI_PROJECT_SETTINGS_HPP_
#define INCLUDED_SHACHI_PROJECT_SETTINGS_HPP_

struct ProjectSettings
{
    template <class T> static constexpr T kWindowWidth  = static_cast<T>(   1280   );
    template <class T> static constexpr T kWindowHeight = static_cast<T>(    720   );
    template <class T> static constexpr T kHorizontalResolution = static_cast<T>(   1280   );
    template <class T> static constexpr T kVerticalResolution   = static_cast<T>(    720   );
};

#endif // !INCLUDED_SHACHI_PROJECT_SETTINGS_HPP_
// EOF

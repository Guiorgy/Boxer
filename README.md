# Boxer 🥊

## Introduction

Boxer is a simple C++ library that allows for easy cross-platform (Windows & Linux) creation of message boxes / alerts / what have you.

## Example

macOS:

![macOS](https://user-images.githubusercontent.com/1409522/213894782-72c37b24-bdb3-4b29-a847-cbff7748b1fe.png)

Windows:

![Windows](https://user-images.githubusercontent.com/1409522/213894790-55cf2be8-bcc0-4867-95e0-7741993f07eb.png)

Linux:

![Linux](https://user-images.githubusercontent.com/1409522/213894798-1bb1c279-5190-4108-b49c-08a28c7dfc29.png)

## Dependencies

On Linux, Boxer requires either the gtk+-3.0 (`apt install libgtk-3-dev` and `pkg-config --cflags --libs gtk+-3.0`) or gtk4 (`apt isntall libgtk-4-dev` and `pkg-config --cflags --libs gtk4`) package.

By default, Boxer uses GTK3. `GTK4` or `GTK4_DEPRECATED` needs to be defined when compiling Boxer to enable GTK4:

```c++
#define GTK4
// or #define GTK4_DEPRECATED
#include "boxer.hpp"
```

`GTK4_DEPRECATED` uses the same `GtkMessageDialog` API as GTK3, however, it was deprecated in `4.10` and gives a compile-time warning. `GTK4` instead uses `GtkAlertDialog`, which was added in `4.10`.

## Including Boxer

Wherever you want to use Boxer, just include the header:

```c++
#include "boxer.hpp"
```

## Using Boxer

To create a message box using Boxer, call the 'show' method in the 'boxer' namespace and provide a message and title:

```c++
boxer::show("Simple message boxes are very easy to create.", "Simple Example");
```

A style / set of buttons may also be specified, and the user's selection can be determined from the function's return value:

```c++
boxer::Selection sel = boxer::show("Make a choice:", "Decision", boxer::Style::Warning, boxer::Buttons::YesNo);
```

Calls to 'show' are blocking - execution of your program will not continue until the user dismisses the message box.

### Encoding

Boxer accepts strings encoded in UTF-8:

```c++
boxer::show(u8"Boxer accepts UTF-8 strings. 💯", u8"Unicode 👍");
```

On Windows, `UNICODE` needs to be defined when compiling Boxer to enable UTF-8 support:

```c++
#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include "boxer.hpp"
```

#pragma once

#if defined(BOXER_DLL) && defined(BOXER_BUILD_DLL)
   /*!
    * BOXER_DLL must be defined by applications that are linking against the DLL version of the Boxer library.
    * BOXER_BUILD_DLL is defined when compiling the DLL version of the library.
    */
   #error "You may not have both BOXER_DLL and BOXER_BUILD_DLL defined"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#ifndef WINDOWS
#define WINDOWS
#define UNDEF_WINDOWS
#endif // WINDOWS
#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#if defined(__linux__) && defined(WINDOWS)
   #error "You may not have both __linux__ and [WIN32|_WIN32|__WIN32|__CYGWIN__] defined"
#endif

#include <map>
#include <string>

#if defined(__linux__)
#include <gtk/gtk.h>
#elif defined(WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // defined(__linux__)

/*!
 * BOXERAPI is used to declare public API classes / functions for export from the DLL / shared library / dynamic library
 */
#if defined(WINDOWS) && defined(BOXER_BUILD_DLL)
   // We are building Boxer as a Win32 DLL
   #define BOXERAPI __declspec(dllexport)
#elif defined(WINDOWS) && defined(BOXER_DLL)
   // We are calling Boxer as a Win32 DLL
   #define BOXERAPI __declspec(dllimport)
#elif defined(__GNUC__) && defined(BOXER_BUILD_DLL)
   // We are building Boxer as a shared / dynamic library
   #define BOXERAPI __attribute__((visibility("default")))
#else
   // We are building or calling Boxer as a static library
   #define BOXERAPI
#endif

namespace boxer
{

/*!
 * Options for styles to apply to a message box
 */
enum class Style
{
   Info,
   Warning,
   Error,
   Question
};

/*!
 * Options for buttons to provide on a message box
 */
enum class Buttons
{
   OK,
   OKCancel,
   YesNo,
   Quit
};

/*!
 * Possible responses from a message box. 'None' signifies that no option was chosen, and 'Error' signifies that an
 * error was encountered while creating the message box.
 */
enum class Selection
{
   OK,
   Cancel,
   Yes,
   No,
   Quit,
   None,
   Error
};

namespace
{
#if defined(__linux__)
   GtkMessageType getMessageType(Style style)
   {
      switch (style)
      {
      case Style::Info:
         return GTK_MESSAGE_INFO;
      case Style::Warning:
         return GTK_MESSAGE_WARNING;
      case Style::Error:
         return GTK_MESSAGE_ERROR;
      case Style::Question:
         return GTK_MESSAGE_QUESTION;
      default:
         return GTK_MESSAGE_INFO;
      }
   }

   GtkButtonsType getButtonsType(Buttons buttons)
   {
      switch (buttons)
      {
      case Buttons::OK:
         return GTK_BUTTONS_OK;
      case Buttons::OKCancel:
         return GTK_BUTTONS_OK_CANCEL;
      case Buttons::YesNo:
         return GTK_BUTTONS_YES_NO;
     case Buttons::Quit:
         return GTK_BUTTONS_CLOSE;
      default:
         return GTK_BUTTONS_OK;
      }
   }

   Selection getSelection(gint response)
   {
      switch (response)
      {
      case GTK_RESPONSE_OK:
         return Selection::OK;
      case GTK_RESPONSE_CANCEL:
         return Selection::Cancel;
      case GTK_RESPONSE_YES:
         return Selection::Yes;
      case GTK_RESPONSE_NO:
         return Selection::No;
      case GTK_RESPONSE_CLOSE:
         return Selection::Quit;
      default:
         return Selection::None;
      }
   }
#elif defined(WINDOWS)
 #if defined(UNICODE)
   bool utf8ToUtf16(const char* utf8String, std::wstring& utf16String)
   {
      int count = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, nullptr, 0);
      if (count <= 0)
      {
         return false;
      }

      utf16String = std::wstring(static_cast<size_t>(count), L'\0');
      return MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, &utf16String[0], count) > 0;
   }
 #endif // defined(UNICODE)

   UINT getIcon(Style style)
   {
      switch (style)
      {
      case Style::Info:
         return MB_ICONINFORMATION;
      case Style::Warning:
         return MB_ICONWARNING;
      case Style::Error:
         return MB_ICONERROR;
      case Style::Question:
         return MB_ICONQUESTION;
      default:
         return MB_ICONINFORMATION;
      }
   }

   UINT getButtons(Buttons buttons)
   {
      switch (buttons)
      {
      case Buttons::OK:
      case Buttons::Quit: // There is no 'Quit' button on Windows :(
         return MB_OK;
      case Buttons::OKCancel:
         return MB_OKCANCEL;
      case Buttons::YesNo:
         return MB_YESNO;
      default:
         return MB_OK;
      }
   }

   Selection getSelection(int response, Buttons buttons)
   {
      switch (response)
      {
      case IDOK:
         return buttons == Buttons::Quit ? Selection::Quit : Selection::OK;
      case IDCANCEL:
         return Selection::Cancel;
      case IDYES:
         return Selection::Yes;
      case IDNO:
         return Selection::No;
      default:
         return Selection::None;
      }
   }
#endif // defined(__linux__/WINDOWS)
} // namespace

/*!
 * The default style to apply to a message box
 */
constexpr Style kDefaultStyle = Style::Info;

/*!
 * The default buttons to provide on a message box
 */
constexpr Buttons kDefaultButtons = Buttons::OK;

/*!
 * Blocking call to create a modal message box with the given message, title, style, and buttons
 */
BOXERAPI Selection show(const char* message, const char* title, Style style, Buttons buttons)
{
#if defined(__linux__)
   if (!gtk_init_check(0, nullptr))
   {
      return Selection::Error;
   }

   // Create a parent window to stop gtk_dialog_run from complaining
   GtkWidget* parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);

   GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                              GTK_DIALOG_MODAL,
                                              getMessageType(style),
                                              getButtonsType(buttons),
                                              "%s",
                                              message);
   gtk_window_set_title(GTK_WINDOW(dialog), title);

   gtk_window_set_gravity(GTK_WINDOW(parent), GDK_GRAVITY_CENTER);
   gtk_window_set_gravity(GTK_WINDOW(dialog), GDK_GRAVITY_CENTER);
   gtk_window_set_position(GTK_WINDOW(parent), GTK_WIN_POS_CENTER);
   gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

   Selection selection = getSelection(gtk_dialog_run(GTK_DIALOG(dialog)));

   gtk_widget_destroy(GTK_WIDGET(dialog));
   gtk_widget_destroy(GTK_WIDGET(parent));
   while (g_main_context_iteration(nullptr, false));

   return selection;
#elif defined(WINDOWS)
   UINT flags = MB_TASKMODAL;

   flags |= getIcon(style);
   flags |= getButtons(buttons);

 #if defined(UNICODE)
   std::wstring wideMessage;
   std::wstring wideTitle;
   if (!utf8ToUtf16(message, wideMessage) || !utf8ToUtf16(title, wideTitle))
   {
      return Selection::Error;
   }

   const WCHAR* messageArg = wideMessage.c_str();
   const WCHAR* titleArg = wideTitle.c_str();
 #else // defined(UNICODE)
   const char* messageArg = message;
   const char* titleArg = title;
 #endif // defined(UNICODE)

   return getSelection(MessageBox(nullptr, messageArg, titleArg, flags), buttons);
#endif // defined(__linux__/WINDOWS)
}

/*!
 * Convenience function to call show() with the default buttons
 */
inline Selection show(const char* message, const char* title, Style style)
{
   return show(message, title, style, kDefaultButtons);
}

/*!
 * Convenience function to call show() with the default style
 */
inline Selection show(const char* message, const char* title, Buttons buttons)
{
   return show(message, title, kDefaultStyle, buttons);
}

/*!
 * Convenience function to call show() with the default style and buttons
 */
inline Selection show(const char* message, const char* title)
{
   return show(message, title, kDefaultStyle, kDefaultButtons);
}

} // namespace boxer

namespace std {
    namespace boxer_detail {
        const map<boxer::Style, const string> styleToString = {
            { boxer::Style::Info, "Info" },
            { boxer::Style::Warning, "Warning" },
            { boxer::Style::Error, "Error" },
            { boxer::Style::Question, "Question" }
        };

        const map<boxer::Buttons, const string> buttonsToString = {
            { boxer::Buttons::OK, "OK" },
            { boxer::Buttons::OKCancel, "OKCancel" },
            { boxer::Buttons::YesNo, "YesNo" },
            { boxer::Buttons::Quit, "Quit" }
        };

        const map<boxer::Selection, const string> selectionToString = {
            { boxer::Selection::OK, "OK" },
            { boxer::Selection::Cancel, "Cancel" },
            { boxer::Selection::Yes, "Yes" },
            { boxer::Selection::No, "No" },
            { boxer::Selection::Quit, "Quit" },
            { boxer::Selection::None, "None" },
            { boxer::Selection::Error, "Error" }
        };
    } // namespace

    const string& to_string(const boxer::Style style) {
        return boxer_detail::styleToString.at(style);
    }

    const string& to_string(const boxer::Buttons buttons) {
        return boxer_detail::buttonsToString.at(buttons);
    }

    const string& to_string(const boxer::Selection selection) {
        return boxer_detail::selectionToString.at(selection);
    }
} // namespace std

#ifdef UNDEF_WINDOWS
#undef UNDEF_WINDOWS
#undef WINDOWS
#endif

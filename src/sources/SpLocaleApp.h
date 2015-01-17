/*
 - SpLocaleApp.h
 -
 - Class derived from BApplication that manages localized applications.
 - As a bonus, it also manages the presence of a replicant in the toolbar.
 -
 - Copyright © 2000-2001 Bernard Krummenacher (Silicon-Peace)
 -
 - 2000.12.09 Bernard Krummenacher		Created this file.
 -
*/

#ifndef SP_LOCALE_APP_H
#define SP_LOCALE_APP_H

#include <be/app/Roster.h>
#include <be/app/Application.h>
#include <be/storage/Path.h>

// Msg sent to the application when the user changed
// the program language or the user's preferred one.
#define MSG_LANGUAGE_CHANGED 'sMLC'

class SpAppPrivate;	// This class contains private data. Please don't ask for details.

class SpLocaleApp:
	public BApplication
{
	friend class SpLocale;
	friend class SpAppPrivate;
	SpAppPrivate* P;	// Pointer to the private data. 
public:

	// Constructor. Specifies the program's native language.
	SpLocaleApp
	(
		const char *signature,							// The usual BeOS application signature.
		const char* NativeLanguage = "eng",	// Native language ISO639 code.
		const char* NativeCountry = "---"		// Native country ISO3166 code. "---" means no specific country.
	);

	// Constructor. Specifies the program's native language and menu options.
	// DO NOT USE ANY MORE!!! WILL BE REMOVED IN VERSION 2.0 !!!!!!
	// Instead, put the flags as AddToFileMenu() parameters.
	// If only the "Language" menu item is required, use AddLanguageMenuItem().
	SpLocaleApp
	(
		const char *signature,							// The usual BeOS application signature.
		bool DeskbarInMenu,									// True to add a "Live in deskbar" menu entry.
		bool CheckWebInMenu,								// True to add a "Check language update..." menu entry.
		const char* CheckWebUrl = NULL,			// URL for language files repository. If NULL, defaults to SpLocale official repository.
		const char* NativeLanguage = "eng",	// Native language ISO639 code.
		const char* NativeCountry = "---"		// Native country ISO3166 code. "---" means no specific country.
	);

	// Gets a translated string based on the native language string.
	// When no translation is available, returns native string.
	static const char* Translate
	(
		const char* NativeString						// Native language string.
	);

	// Get the most appropriate fonts for the specified language.
	// If LanguageName == NULL, get the fonts for the current language.
	static void GetLocalePlainFont(BFont* Font, const char* LanguageName = NULL);
	static void GetLocaleBoldFont(BFont* Font, const char* LanguageName = NULL);
	static void GetLocaleFixedFont(BFont* Font, const char* LanguageName = NULL);
	static void GetLocaleMenuFont(BFont* Font, const char* LanguageName = NULL);
	
	// Apply the menu font to the whole menu bar tree.
	static void FixMenuFont(BMenu* Menu);

	// Application's file name. Just a free bonus.
	const char* AppName(void);
	
	// Specify an application that must change language at the same time as this one, for instance Tracker for Deskbar.
	void AddCooperativeApp(const char* AppSignature);
	
	// Add the "Language" menu item in any menu. 
	void AddLanguageMenuItem(BMenu* Menu);
	
	// Add a few items in the file menu (Live in deskbar, Language, Check language update..., Quit).
	// All the items are added, but when flags are added in the object constructor.
	void AddToFileMenu(BMenu* FileMenu);

	// Add selected items in the file menu (Live in deskbar, Language, Check language update..., Quit).
	// The "language" item is always added.
	void AddToFileMenu
	(
		BMenu* FileMenu,
		bool DeskbarInMenu,									// True to add a "Live in deskbar" menu entry.
		bool CheckWebInMenu = false,				// True to add a "Check language update..." menu entry.
		bool QuitInMenu = false				// True to add a "Quit" menu entry.
	);

	// Set and get the URL pointing to the language file repository (Format is: "http://there@dot.com/.../bla-bla.html").
	// By default, it point to the SpLocale repository.
	void SetCheckWebUrl(const char* Url);
	const char* CheckWebUrl(void);
	
	// Add a few entries in the help menu (About...). 
	void AddToHelpMenu(BMenu* HelpMenu);
	
	// Handles some private messages.
	// Must be called by the application's MessageReceived() function.
	virtual void MessageReceived(BMessage* message);
};

// Classless version of SpLocaleApp::Translate().
inline const char* SpTranslate
(
	const char* NativeString							// Native language string.
)
{
	return SpLocaleApp::Translate(NativeString);
}

// Classless versions of SpLocaleApp::GetLocale...Font().
inline void SpGetLocalePlainFont
(
	BFont* Font,
	const char* LanguageName = NULL
)
{
	SpLocaleApp::GetLocalePlainFont(Font, LanguageName);
}

inline void SpGetLocaleBoldFont
(
	BFont* Font,
	const char* LanguageName = NULL
)
{
	SpLocaleApp::GetLocaleBoldFont(Font, LanguageName);
}

inline void SpGetLocaleFixedFont
(
	BFont* Font,
	const char* LanguageName = NULL
)
{
	SpLocaleApp::GetLocaleFixedFont(Font, LanguageName);
}

inline void SpGetLocaleMenuFont
(
	BFont* Font,
	const char* LanguageName = NULL
)
{
	SpLocaleApp::GetLocaleMenuFont(Font, LanguageName);
}



// Classless versions of SpLocaleApp::FixMenuFont().
// Apply the menu font to the whole menu bar tree.
inline void SpFixMenuFont(BMenu* Menu)
{
	SpLocaleApp::FixMenuFont(Menu);
}

#endif

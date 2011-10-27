/*	Better Oblivion Sorting Software

	A "one-click" program for users that quickly optimises and avoids 
	detrimental conflicts in their TES IV: Oblivion, Nehrim - At Fate's Edge, 
	TES V: Skyrim, Fallout 3 and Fallout: New Vegas mod load orders.

    Copyright (C) 2011  WrinklyNinja & the BOSS development team. 
	Copyright license:
    http://creativecommons.org/licenses/by-nc-nd/3.0/

	$Revision: 2188 $, $Date: 2011-01-20 10:05:16 +0000 (Thu, 20 Jan 2011) $
*/

#ifndef __ELEMENTIDS__HPP__
#define __ELEMENTIDS__HPP__

//We want to ensure that the GUI-specific code in BOSS-Common is included.
#ifndef BOSSGUI
#define BOSSGUI
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#       include "wx/wx.h"
#endif

enum {
	//Main window.
    OPTION_EditUserRules = wxID_HIGHEST + 1, // declares an id which will be used to call our button
	OPTION_OpenBOSSlog,
	OPTION_Run,
	OPTION_CheckForUpdates,
    MENU_Quit,
	MENU_OpenMainReadMe,
	MENU_OpenUserRulesReadMe,
	MENU_OpenMasterlistReadMe,
	MENU_OpenAPIReadMe,
	MENU_ShowAbout,
	MENU_ShowSettings,
	DROPDOWN_LogFormat,
	DROPDOWN_Game,
	DROPDOWN_Revert,
	CHECKBOX_ShowBOSSlog,
	CHECKBOX_Update,
	CHECKBOX_EnableVersions,
	CHECKBOX_EnableCRCs,
	CHECKBOX_TrialRun,
	RADIOBUTTON_SortOption,
	RADIOBUTTON_UpdateOption,
	RADIOBUTTON_UndoOption,
	//About window.
	OPTION_ExitAbout,
	//Settings window.
	OPTION_OKExitSettings,
	OPTION_CancelExitSettings,
	DROPDOWN_ProxyType,
	//User Rules Editor.
	LIST_RuleList,
	BUTTON_NewRule,
	BUTTON_EditRule,
	BUTTON_DeleteRule,
	LIST_Modlist,
	LIST_Masterlist,
	BUTTON_OKExitEditor,
	BUTTON_CancelExitEditor,
	TEXT_ModMessages,
	SEARCH_Masterlist,
	SEARCH_Modlist,
	TEXT_RuleMod,
	CHECKBOX_SortMods,
	RADIO_SortMod,
	RADIO_InsertMod,
	CHOICE_BeforeAfter,
	CHOICE_TopBottom,
	TEXT_SortMod,
	TEXT_InsertMod,
	CHECKBOX_RemoveMessages,
	CHECKBOX_AddMessages,
	TEXT_NewMessages,
	BUTTON_MoveRuleUp,
	BUTTON_MoveRuleDown
};
#endif
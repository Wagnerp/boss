/*	General User Interface for BOSS (Better Oblivion Sorting Software)
	
	Providing a graphical frontend to BOSS's functions.

    Copyright (C) 2011 WrinklyNinja & the BOSS development team.
    http://creativecommons.org/licenses/by-nc-nd/3.0/


	$Revision: 2188 $, $Date: 2011-01-20 10:05:16 +0000 (Thu, 20 Jan 2011) $
*/

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include <wx/hyperlink.h>
#include "boost/exception/get_error_info.hpp"
#include <boost/algorithm/string.hpp>

#include <wx/richtext/richtextctrl.h>
#include <wx/aboutdlg.h>

#include "gui.h"
#include "parser.h"
#include <iostream>

namespace fs = boost::filesystem;

BEGIN_EVENT_TABLE ( MainFrame, wxFrame )
	EVT_CLOSE (MainFrame::OnClose)
	EVT_MENU ( MENU_Quit, MainFrame::OnQuit )
	EVT_MENU ( OPTION_OpenUserlist, MainFrame::OnOpenFile )
	EVT_MENU ( OPTION_OpenBOSSlog, MainFrame::OnOpenFile )
	EVT_MENU ( OPTION_Run, MainFrame::OnRunBOSS )
	EVT_MENU ( MENU_OpenMReadMe, MainFrame::OnOpenFile )
	EVT_MENU ( MENU_OpenURReadMe, MainFrame::OnOpenFile )
	EVT_MENU ( OPTION_CheckForUpdates, MainFrame::OnUpdateCheck )
	EVT_MENU ( MENU_ShowAbout, MainFrame::OnAbout )
	EVT_BUTTON ( OPTION_Run, MainFrame::OnRunBOSS )
	EVT_BUTTON ( OPTION_OpenUserlist, MainFrame::OnOpenFile )
	EVT_BUTTON ( OPTION_OpenBOSSlog, MainFrame::OnOpenFile )
	EVT_BUTTON ( OPTION_CheckForUpdates, MainFrame::OnUpdateCheck )
	EVT_COMBOBOX ( DROPDOWN_LogFormat, MainFrame::OnFormatChange )
	EVT_COMBOBOX ( DROPDOWN_Verbosity, MainFrame::OnVerbosityChange )
	EVT_COMBOBOX ( DROPDOWN_Game, MainFrame::OnGameChange )
	EVT_COMBOBOX ( DROPDOWN_Revert, MainFrame::OnRevertChange )
	EVT_CHECKBOX ( CHECKBOX_ShowBOSSlog, MainFrame::OnLogDisplayChange )
	EVT_CHECKBOX ( CHECKBOX_EnableDebug, MainFrame::OnDebugChange )
	EVT_CHECKBOX ( CHECKBOX_Update, MainFrame::OnUpdateChange )
	EVT_CHECKBOX ( CHECKBOX_SortEnableVersions, MainFrame::OnVersionDisplayChange )
	EVT_CHECKBOX ( CHECKBOX_SortEnableCRCs, MainFrame::OnCRCDisplayChange )
	EVT_CHECKBOX ( CHECKBOX_RevertEnableVersions, MainFrame::OnVersionDisplayChange )
	EVT_CHECKBOX ( CHECKBOX_RevertEnableCRCs, MainFrame::OnCRCDisplayChange )
	EVT_CHECKBOX ( CHECKBOX_EnableLogging, MainFrame::OnLoggingChange )
	EVT_CHECKBOX ( CHECKBOX_TrialRun, MainFrame::OnTrialRunChange )
	EVT_RADIOBUTTON ( RADIOBUTTON_SortOption, MainFrame::OnRunTypeChange )
	EVT_RADIOBUTTON ( RADIOBUTTON_UpdateOption, MainFrame::OnRunTypeChange )
	EVT_RADIOBUTTON ( RADIOBUTTON_UndoOption, MainFrame::OnRunTypeChange )
END_EVENT_TABLE()

IMPLEMENT_APP(BossGUI)

//Draws the main window when program starts.
bool BossGUI::OnInit() {
	//Set up variable defaults.
	if (fs::exists("BOSS.ini"))
		boss::parseIni("BOSS.ini");

	MainFrame *frame = new MainFrame(
		wxT("Better Oblivion Sorting Software GUI - " + boss::GetGame()), 100, 100, 510, 370);

	frame->SetIcon(wxIconLocation("BOSS GUI.exe"));
	frame->Show(TRUE);
	SetTopWindow(frame);
	return true;
}

MainFrame::MainFrame(const wxChar *title, int x, int y, int width, int height) : wxFrame(NULL, -1, title, wxPoint(x, y), wxSize(width, height)) {

	//Some variable setup.
	wxString choices[] = {
        wxT("HTML"),
        wxT("Plain Text")
    };
	wxString choices2[] = {
        wxT("Standard (0)"),
        wxT("Level 1"),
		wxT("Level 2"),
		wxT("Level 3")
    };
	wxString choices3[] = {
		wxT("Autodetect"),
		wxT("Oblivion"),
		wxT("Fallout 3"),
		wxT("Nehrim"),
		wxT("Fallout: New Vegas")
	};
	wxString choices4[] = {
		wxT("No Undo"),
		wxT("Last Run"),
		wxT("2nd Last Run")
	};

	//Set up menu bar first.
    MenuBar = new wxMenuBar();
    // File Menu
    FileMenu = new wxMenu();
	FileMenu->Append(OPTION_OpenBOSSlog, _T("&Open BOSSlog"), _T("Opens your BOSSlog."));
    FileMenu->Append(OPTION_OpenUserlist, _T("&Edit Userlist"), _T("Opens your userlist in your default text editor."));
    FileMenu->Append(OPTION_Run, _T("&Run BOSS"), _T("Runs BOSS with the options you have chosen."));
    FileMenu->AppendSeparator();
    FileMenu->Append(MENU_Quit, _T("&Quit"), _T("Quit BOSS GUI."));
    MenuBar->Append(FileMenu, _T("&File"));
    // About menu
    HelpMenu = new wxMenu();
	HelpMenu->Append(MENU_OpenMReadMe, _T("Open &Main ReadMe"), _T("Opens the main BOSS ReadMe in your default web browser."));
	HelpMenu->Append(MENU_OpenURReadMe, _T("Open &User Rules ReadMe"), _T("Opens the User Rules ReadMe in your default web browser."));
	HelpMenu->AppendSeparator();
	HelpMenu->Append(OPTION_CheckForUpdates, _T("&Check For Updates..."), _T("Checks for updates to BOSS and your masterlist."));
	HelpMenu->Append(MENU_ShowAbout, _T("&About BOSS GUI..."), _T("Shows information about BOSS GUI."));
    MenuBar->Append(HelpMenu, _T("&Help"));
    SetMenuBar(MenuBar);

	//Set up stuff in the frame.
	SetBackgroundColour(wxColour(255,255,255));

	//Contents in one big resizing box.
	wxBoxSizer *bigBox = new wxBoxSizer(wxHORIZONTAL);

	//Create first column box and add the output options to it.
	wxBoxSizer *columnBox = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer *outputOptionsBox = new wxStaticBoxSizer(wxVERTICAL, this, "Output Options");
	wxBoxSizer *formatBox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *verbosityBox = new wxBoxSizer(wxHORIZONTAL);
	//Add stuff to output options sizer.
	outputOptionsBox->Add(ShowLogBox = new wxCheckBox(this,CHECKBOX_ShowBOSSlog, "Show BOSSlog On Completion"), 0, wxALL, 5);
	outputOptionsBox->Add(DebugBox = new wxCheckBox(this,CHECKBOX_EnableDebug, "Enable Debug Output"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	outputOptionsBox->Add(LoggingBox = new wxCheckBox(this,CHECKBOX_EnableLogging, "Enable Logging Of Command Line Output"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	formatBox->Add(new wxStaticText(this, wxID_ANY, "BOSSlog Format: "), 1, wxLEFT | wxBOTTOM, 5);
	formatBox->Add(FormatBox = new wxComboBox(this, DROPDOWN_LogFormat, choices[0], wxPoint(110,60), wxDefaultSize, 2, choices, wxCB_READONLY), 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);
	verbosityBox->Add(new wxStaticText(this, wxID_ANY, "Command Line Verbosity: "), 1, wxLEFT | wxBOTTOM, 5);
	verbosityBox->Add(VerbosityBox = new wxComboBox(this, DROPDOWN_Verbosity, choices2[0], wxDefaultPosition, wxDefaultSize, 4, choices2, wxCB_READONLY), 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);
	//Add the verbosityBox to its parent now to preserve layout.
	outputOptionsBox->Add(formatBox, 0, wxEXPAND, 0);
	outputOptionsBox->Add(verbosityBox, 0, wxEXPAND, 0);
	columnBox->Add(outputOptionsBox, 0, wxBOTTOM, 30);

	//Now add the main buttons to the first column.
	wxBoxSizer *buttonBox = new wxBoxSizer(wxVERTICAL);
	buttonBox->Add(OpenUserlistButton = new wxButton(this,OPTION_OpenUserlist, "Edit Userlist", wxDefaultPosition, wxSize(120,30)), 0, wxBOTTOM, 5);
	buttonBox->Add(RunBOSSButton = new wxButton(this,OPTION_Run, "Run BOSS", wxDefaultPosition, wxSize(120,30)));
	buttonBox->Add(OpenBOSSlogButton = new wxButton(this,OPTION_OpenBOSSlog, "View BOSSlog", wxDefaultPosition, wxSize(120,30)), 0, wxTOP, 5);
	columnBox->Add(buttonBox, 0, wxALIGN_CENTER, 20);

	//Add the first column to the big box.
	bigBox->Add(columnBox, 0, wxALL, 20);

	//The second column has a border.
	wxStaticBoxSizer *runOptionsBox = new wxStaticBoxSizer(wxVERTICAL, this, "Run Options");
	wxBoxSizer *sortBox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *updateBox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *undoBox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *gameBox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *revertBox = new wxBoxSizer(wxHORIZONTAL);

	//Run Options
	runOptionsBox->Add(SortOption = new wxRadioButton(this, RADIOBUTTON_SortOption, "Sort Mods"), 0, wxALL, 5);
	
	//Sort option stuff.
	sortBox->Add(UpdateBox = new wxCheckBox(this,CHECKBOX_Update, "Update Masterlist"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	sortBox->Add(SortVersionBox = new wxCheckBox(this,CHECKBOX_SortEnableVersions, "Enable Mod Version Display"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	sortBox->Add(SortCRCBox = new wxCheckBox(this,CHECKBOX_SortEnableCRCs, "Enable File CRC Display"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	sortBox->Add(TrialRunBox = new wxCheckBox(this,CHECKBOX_TrialRun, "Perform Trial Run"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	runOptionsBox->Add(sortBox, 0, wxLEFT | wxBOTTOM, 20);
	
	//Update only stuff.
	runOptionsBox->Add(UpdateOption = new wxRadioButton(this, RADIOBUTTON_UpdateOption, "Update Masterlist Only"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	gameBox->Add(GameText = new wxStaticText(this, wxID_ANY, "Game: "), 1, wxLEFT | wxBOTTOM, 15);
	gameBox->Add(GameBox = new wxComboBox(this, DROPDOWN_Game, choices3[0], wxDefaultPosition, wxDefaultSize, 5, choices3, wxCB_READONLY), 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);
	updateBox->Add(gameBox, 0, wxEXPAND, 0);
	runOptionsBox->Add(updateBox, 0, wxEXPAND | wxLEFT | wxBOTTOM, 20);
	
	//Undo option stuff.
	runOptionsBox->Add(UndoOption = new wxRadioButton(this, RADIOBUTTON_UndoOption, "Undo Changes"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	revertBox->Add(RevertText = new wxStaticText(this, wxID_ANY, "Undo Level: "), 1, wxLEFT | wxBOTTOM, 5);
	revertBox->Add(RevertBox = new wxComboBox(this, DROPDOWN_Revert, choices4[0], wxDefaultPosition, wxDefaultSize, 3, choices4, wxCB_READONLY), 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);
	undoBox->Add(revertBox, 0, wxEXPAND, 0);
	undoBox->Add(UndoVersionBox = new wxCheckBox(this,CHECKBOX_RevertEnableVersions, "Enable Mod Version Display"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	undoBox->Add(UndoCRCBox = new wxCheckBox(this,CHECKBOX_RevertEnableCRCs, "Enable File CRC Display"), 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
	runOptionsBox->Add(undoBox, 0, wxEXPAND | wxLEFT | wxBOTTOM, 20);

	bigBox->Add(runOptionsBox, 0, wxTOP | wxRIGHT | wxBOTTOM, 20);


	//Tooltips
	LoggingBox->SetToolTip("The output is logged to the BOSSCommandLineLog.txt file");
	FormatBox->SetToolTip("This decides both the format of BOSSlog generated when you click the \"Run BOSS\" button and the BOSSlog format opened when you click the \"View BOSSlog\" button.");
	OpenBOSSlogButton->SetToolTip("The format of BOSSlog this opens is decided by the setting of the \"BOSSlog Format\" Output Option above.");
	DebugBox->SetToolTip("Adds source code references to command line output.");
	VerbosityBox->SetToolTip("The higher the verbosity level, the more information is outputted to the command line.");
	TrialRunBox->SetToolTip("Runs BOSS, but doesn't actually change your load order.");

	//Set option values based on initialised variable values.
	RunBOSSButton->SetDefault();
	if (boss::update)
		UpdateBox->SetValue(true);
	else
		UpdateBox->SetValue(false);
	if (boss::silent)
		ShowLogBox->SetValue(false);
	else
		ShowLogBox->SetValue(true);
	if (boss::debug)
		DebugBox->SetValue(true);
	else
		DebugBox->SetValue(false);
	if (boss::trial_run)
		TrialRunBox->SetValue(true);
	else
		TrialRunBox->SetValue(false);
	if (boss::logCL)
		LoggingBox->SetValue(true);
	else
		LoggingBox->SetValue(false);
	if (boss::sort_show_CRCs)
		SortCRCBox->SetValue(true);
	else
		SortCRCBox->SetValue(false);
	if (boss::sort_skip_version_parse)
		SortVersionBox->SetValue(false);
	else
		SortVersionBox->SetValue(true);
	if (boss::revert_show_CRCs)
		UndoCRCBox->SetValue(true);
	else
		UndoCRCBox->SetValue(false);
	if (boss::revert_skip_version_parse)
		UndoVersionBox->SetValue(false);
	else
		UndoVersionBox->SetValue(true);
	if (boss::game == 0)
		GameBox->SetValue(choices3[0]);
	else if (boss::game == 1)
		GameBox->SetValue(choices3[1]);
	else if (boss::game == 2)
		GameBox->SetValue(choices3[2]);
	else if (boss::game == 3)
		GameBox->SetValue(choices3[3]);
	else if (boss::game == 4)
		GameBox->SetValue(choices3[4]);
	if (boss::revert == 0)
		RevertBox->SetValue(choices4[0]);
	else if (boss::revert == 1)
		RevertBox->SetValue(choices4[1]);
	else if (boss::revert == 2)
		RevertBox->SetValue(choices4[2]);
	if (boss::verbosity == 0)
		VerbosityBox->SetValue(choices2[0]);
	else if (boss::verbosity == 1)
		VerbosityBox->SetValue(choices2[1]);
	else if (boss::verbosity == 2)
		VerbosityBox->SetValue(choices2[2]);
	else if (boss::verbosity == 3)
		VerbosityBox->SetValue(choices2[3]);
	if (boss::log_format == "html")
		FormatBox->SetValue(choices[0]);
	else if (boss::log_format == "text")
		FormatBox->SetValue(choices[1]);

	if (boss::run_type == 1) {
		SortOption->SetValue(true);
		UpdateBox->Enable(true);
		SortVersionBox->Enable(true);
		SortCRCBox->Enable(true);
		TrialRunBox->Enable(true);
		GameText->Enable(false);
		GameBox->Enable(false);
		RevertText->Enable(false);
		RevertBox->Enable(false);
		UndoVersionBox->Enable(false);
		UndoCRCBox->Enable(false);
	} else if (boss::run_type == 2) {
		UpdateOption->SetValue(true);
		UpdateBox->Enable(false);
		SortVersionBox->Enable(false);
		SortCRCBox->Enable(false);
		TrialRunBox->Enable(false);
		GameText->Enable(true);
		GameBox->Enable(true);
		RevertText->Enable(false);
		RevertBox->Enable(false);
		UndoVersionBox->Enable(false);
		UndoCRCBox->Enable(false);
	} else {
		UndoOption->SetValue(true);
		UpdateBox->Enable(false);
		SortVersionBox->Enable(false);
		SortCRCBox->Enable(false);
		TrialRunBox->Enable(false);
		GameText->Enable(false);
		GameBox->Enable(false);
		RevertText->Enable(true);
		RevertBox->Enable(true);
		UndoVersionBox->Enable(true);
		UndoCRCBox->Enable(true);
	}

	//Now set up the status bar.
	CreateStatusBar(1);
    SetStatusText("Ready");

	//Now set the layout and sizes.
	SetSizerAndFit(bigBox);

	//Now check for program updates.
	CheckForUpdate();
}

//Called when program exits.
void MainFrame::OnQuit( wxCommandEvent& event ) {
	Close(TRUE); // Tells the OS to quit running this process
}

void MainFrame::OnClose(wxCloseEvent& event) {
       //Save settings to BOSS.ini before quitting.
	//Read ini file into string buffer, then search for setting strings and replace their values.
	std::string buffer;
	if (fs::exists("BOSS.ini")) {
		boss::fileToBuffer("BOSS.ini",buffer);
		size_t pos1,pos2;
		pos1 = buffer.find("[GUI.LastOptions]");
		for (int i=0;i<14;i++) {
			pos1 = buffer.find("=",pos1+1);
			if (i==0)
				buffer.replace(pos1+2,1,boss::IntToString(boss::run_type));  //Replace RunType setting.
			else if (i==1)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::silent));  //Replace SilentRun setting.
			else if (i==2)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::debug));  //Replace Debug setting.
			else if (i==3)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::logCL));  //Replace LogCLOutput setting.
			else if (i==4)
				buffer.replace(pos1+2,4,boss::log_format);  //Replace BOSSlogFormat setting.
			else if (i==5)
				buffer.replace(pos1+2,1,boss::IntToString(boss::verbosity));  //Replace CLVerbosity setting.
			else if (i==6)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::update));  //Replace UpdateMasterlist setting.
			else if (i==7)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::sort_skip_version_parse));  //Replace SortNoVersionParse setting.
			else if (i==8)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::sort_show_CRCs));  //Replace SortDisplayCRCs setting.
			else if (i==9)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::trial_run));  //Replace DoTrialRun setting.
			else if (i==10) {
				//Need to find out how long the current setting string is.
				//Look for the end of the line.
				pos2 = buffer.find("\r\n",pos1);  //Windows EOL.
				if (pos2 == std::string::npos)
					pos2 = buffer.find("\n",pos1);  //Unix EOL.
				//Length of setting string is pos2-pos1-2.
				if (boss::game == 0)
					buffer.replace(pos1+2,pos2-pos1-2,"auto");  //Replace Game setting.
				else if (boss::game == 1)
					buffer.replace(pos1+2,pos2-pos1-2,"Oblivion");  //Replace Game setting.
				else if (boss::game == 2)
					buffer.replace(pos1+2,pos2-pos1-2,"Fallout3");  //Replace Game setting.
				else if (boss::game == 3)
					buffer.replace(pos1+2,pos2-pos1-2,"Nehrim");  //Replace Game setting.
				else
					buffer.replace(pos1+2,pos2-pos1-2,"FalloutNV");  //Replace Game setting.
			} else if (i==11)
				buffer.replace(pos1+2,1,boss::IntToString(boss::revert));  //Replace RevertLevel setting.
			else if (i==12)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::revert_skip_version_parse));  //Replace RevertNoVersionParse setting.
			else if (i==13)
				buffer.replace(pos1+2,1,boss::BoolToString(boss::revert_show_CRCs));  //Replace RevertDisplayCRCs setting.
		}
		std::ofstream out("BOSS.ini");
		if (!out.fail()) {
			out.unsetf(std::ios::skipws);
			out << buffer;
			out.close();
		}
	}

    Destroy();  // you may also do:  event.Skip();
                // since the default event handler does call Destroy(), too
}

//Called when program exits.
void MainFrame::OnRunBOSS( wxCommandEvent& event ) {
	if (fs::exists("BOSS.exe"))
		boss::RunBOSS();
	else
		wxMessageBox(wxString::Format(
				wxT("Error: BOSS.exe not found. Reinstall BOSS correctly, so that both BOSS.exe and BOSS GUI.exe are in the BOSS folder in your game's Data folder.")
			),
			wxT("Error"),
			wxOK | wxICON_INFORMATION,
			this);
}

//Call when a file is opened. Either readmes, BOSSlogs or userlist.
void MainFrame::OnOpenFile( wxCommandEvent& event ) {
	std::string file;
	if (event.GetId() == OPTION_OpenUserlist)
		file = "userlist.txt";
	else if (event.GetId() == OPTION_OpenBOSSlog)
		file = "bosslog";
	else if (event.GetId() == MENU_OpenMReadMe)
		file = "BOSS ReadMe";
	else
		file = "BOSS User Rules ReadMe";
	//Need to somehow choose file based on what fired the event.
	if (file == "userlist.txt") {
		if (!fs::exists(file)) {  //Create the userlist.
			std::ofstream ofile(file.c_str());
			ofile.close();
		}
		//Now open it.
		//EditFrame *UserlistEditor = new EditFrame(NULL, wxT("Edit Userlist"), 100, 100, 450, 340);
		//UserlistEditor->Show(TRUE);
		boss::OpenInSysDefault(fs::path("userlist.txt"));
	} else if (file == "bosslog") {
		if (boss::log_format == "html") {  //Open HTML BOSSlog.
			if (fs::exists("BOSSlog.html"))
				boss::OpenInSysDefault(fs::path("BOSSlog.html"));
			else {
				wxMessageBox(wxString::Format(
					wxT("Error: No BOSSlog.html found. Make sure you have run_type BOSS with the HTML output format selected, or run_type BOSS from BOSS.exe, at least once before attempting to open the BOSSlog in the HTML format.")
				),
				wxT("Error"),
				wxOK | wxICON_INFORMATION,
				this);
			}
		} else {  //Open text BOSSlog.
			if (fs::exists("BOSSlog.txt"))
				boss::OpenInSysDefault(fs::path("BOSSlog.txt"));
			else {
				wxMessageBox(wxString::Format(
					wxT("Error: No BOSSlog.txt found. Make sure you have run_type BOSS at least once with the text output format selected before attempting to open the BOSSlog in the plain text format.")
				),
				wxT("Error"),
				wxOK | wxICON_INFORMATION,
				this);
			}
		}
	} else {  //Readme files. They could be anywhere - this could be complicated.
		//Simplify by looking for either the files themselves or shortcuts to them in the BOSS folder.
		//If neither, show a pop-up message saying they can't be found.
		if (fs::exists(file + ".html")) {
			file = "\"" + file + ".html\"";
			boss::OpenInSysDefault(fs::path(file));
		} else if (fs::exists(file + ".lnk")) {
			file = "\"" + file + ".lnk\"";
			boss::OpenInSysDefault(fs::path(file));
		} else {
			//No ReadMe exists, show a pop-up message saying so.
			wxMessageBox(wxString::Format(
				wxT("Error: No %s found. Make sure you have the ReadMe or a shortcut to the ReadMe in your BOSS folder."),
				file
			),
			wxT("Error"),
			wxOK | wxICON_INFORMATION,
			this);
		}
	}
}

void MainFrame::OnAbout(wxCommandEvent& event) {

	wxFrame *frame = new wxFrame(this, -1, "About Better Oblivion Sorting Software GUI", wxPoint(100, 100), wxSize(350, 250),wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
	frame->SetBackgroundColour(wxColour(255,255,255));

	wxBoxSizer *box = new wxBoxSizer(wxVERTICAL);

	/*wxRichTextCtrl *text = new wxRichTextCtrl(frame, -1, wxEmptyString, wxDefaultPosition, wxSize(350, 250));

	wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);

	text->BeginBold();
	text->WriteText("Better Oblivion Sorting Software GUI");
	text->EndBold();
	text->LineBreak();
	text->WriteText(boss::gui_version+" ("+boss::gui_releaseDate+")");
	text->Newline();

	text->WriteText("Provides a graphical front end for running ");
	text->BeginStyle(urlStyle);
	text->BeginURL("http://code.google.com/p/better-oblivion-sorting-software/");
	text->WriteText("Better Oblivion Sorting Software");
	text->EndURL();
	text->EndStyle();
	text->Newline();

	text->WriteText("� WrinklyNinja and the BOSS development team, 2011.");
	text->LineBreak();
	text->WriteText("Some rights reserved. Copyright license: ");
	text->BeginStyle(urlStyle);
	text->BeginURL("http://creativecommons.org/licenses/by-nc-nd/3.0/");
	text->WriteText("CC Attribution-Noncommercial-No Derivative Works 3.0");
	text->EndURL();
	text->EndStyle();

	text->Layout();

	box->Add(text);*/

	box->Add(new wxStaticText(frame,-1,
		"Better Oblivion Sorting Software GUI\nv"+boss::gui_version+" ("+boss::gui_releaseDate+")\n\n"
		"Provides a graphical frontend for running "), 0, wxTOP | wxLEFT | wxRIGHT, 20);

	wxHyperlinkCtrl *link = new wxHyperlinkCtrl(frame, -1, "Better Oblivion Sorting Software","http://code.google.com/p/better-oblivion-sorting-software/");
	link->SetBackgroundColour(wxColour(255,255,255));
	box->Add(link, 0, wxBOTTOM | wxLEFT | wxRIGHT, 20);
	box->Add(new wxStaticText(frame, -1, "� WrinklyNinja and the BOSS development team, 2011.\nSome rights reserved. Copyright license:"), 0, wxLEFT | wxRIGHT, 20);
	link = new wxHyperlinkCtrl(frame, -1, "CC Attribution-Noncommercial-No Derivative Works 3.0","http://creativecommons.org/licenses/by-nc-nd/3.0/");
	link->SetBackgroundColour(wxColour(255,255,255));
	box->Add(link, 0, wxBOTTOM | wxLEFT | wxRIGHT, 20);

	//Now set the layout and sizes.
	frame->SetSizerAndFit(box);

	frame->Show(TRUE);
}

void MainFrame::OnFormatChange(wxCommandEvent& event) {
	if (event.GetInt() == 0)
		boss::log_format = "html";
	else
		boss::log_format = "text";
}

void MainFrame::OnVerbosityChange(wxCommandEvent& event) {
	boss::verbosity = event.GetInt();
}
void MainFrame::OnGameChange(wxCommandEvent& event) {
	boss::game = event.GetInt();
}

void MainFrame::OnRevertChange(wxCommandEvent& event) {
	boss::revert = event.GetInt();
}

void MainFrame::OnLogDisplayChange(wxCommandEvent& event) {
	boss::silent = (!event.IsChecked());
}

void MainFrame::OnDebugChange(wxCommandEvent& event) {
	boss::debug = event.IsChecked();
}

void MainFrame::OnUpdateChange(wxCommandEvent& event) {
	boss::update = event.IsChecked();
}

void MainFrame::OnVersionDisplayChange(wxCommandEvent& event) {
	if (event.GetId() == CHECKBOX_SortEnableVersions)
		boss::sort_skip_version_parse = (!event.IsChecked());
	else
		boss::revert_skip_version_parse = (!event.IsChecked());
}

void MainFrame::OnCRCDisplayChange(wxCommandEvent& event) {
	if (event.GetId() == CHECKBOX_SortEnableCRCs)
		boss::sort_show_CRCs = event.IsChecked();
	else
		boss::revert_show_CRCs = event.IsChecked();
}

void MainFrame::OnLoggingChange(wxCommandEvent& event) {
	boss::logCL = event.IsChecked();
}

void MainFrame::OnTrialRunChange(wxCommandEvent& event) {
	boss::trial_run = event.IsChecked();
}

void MainFrame::OnRunTypeChange(wxCommandEvent& event) {
	if (event.GetId() == RADIOBUTTON_SortOption) {
		boss::run_type = 1;
		UpdateBox->Enable(true);
		SortVersionBox->Enable(true);
		SortCRCBox->Enable(true);
		TrialRunBox->Enable(true);
		GameText->Enable(false);
		GameBox->Enable(false);
		RevertText->Enable(false);
		RevertBox->Enable(false);
		UndoVersionBox->Enable(false);
		UndoCRCBox->Enable(false);
	}else if (event.GetId() == RADIOBUTTON_UpdateOption) {
		boss::run_type = 2;
		UpdateBox->Enable(false);
		SortVersionBox->Enable(false);
		SortCRCBox->Enable(false);
		TrialRunBox->Enable(false);
		GameText->Enable(true);
		GameBox->Enable(true);
		RevertText->Enable(false);
		RevertBox->Enable(false);
		UndoVersionBox->Enable(false);
		UndoCRCBox->Enable(false);
	}else {
		boss::run_type = 3;
		UpdateBox->Enable(false);
		SortVersionBox->Enable(false);
		SortCRCBox->Enable(false);
		TrialRunBox->Enable(false);
		GameText->Enable(false);
		GameBox->Enable(false);
		RevertText->Enable(true);
		RevertBox->Enable(true);
		UndoVersionBox->Enable(true);
		UndoCRCBox->Enable(true);
	}
}

void MainFrame::OnUpdateCheck(wxCommandEvent& event) {
	std::string updateText;
	if (boss::CheckConnection()) {
		try {
			updateText = boss::IsUpdateAvailable();
			if (updateText.length() == 0) {
				wxMessageBox(wxT("You are already using the latest version of BOSS."), wxT("Check For Updates"), wxOK | wxICON_INFORMATION, this);
				return;
			} else
				updateText = "Update available! New version: " + updateText;
		} catch (boss::update_error & e) {
			std::string const * detail = boost::get_error_info<boss::err_detail>(e);
			updateText = "Update check failed. Details: " + *detail;
		}
	} else {
		wxMessageBox(wxT("Update check failed. No Internet connection detected."), wxT("Check For Updates"), wxOK | wxICON_ERROR, this);
		return;
	}

	wxFrame *frame = new wxFrame(this, -1, "Check For Updates", wxPoint(100, 100), wxSize(450, 340),wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
	
	frame->SetBackgroundColour(wxColour(255,255,255));

	wxBoxSizer *bigBox = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *updateBox = new wxBoxSizer(wxVERTICAL);
	updateBox->Add(new wxStaticText(frame,-1,updateText, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP));
	
	wxBoxSizer *textBox = new wxBoxSizer(wxVERTICAL);
	textBox->Add(new wxStaticText(frame,-1,"BOSS updates can be downloaded from the following sites:"));
	
	wxBoxSizer *linkBox = new wxBoxSizer(wxVERTICAL);
	wxHyperlinkCtrl *link = new wxHyperlinkCtrl(frame,-1,"TES Nexus","http://www.tesnexus.com/downloads/file.php?id=20516");
	link->SetBackgroundColour(wxColour(255,255,255));
	linkBox->Add(link);
	link = new wxHyperlinkCtrl(frame,-1,"Fallout 3 Nexus","http://www.fallout3nexus.com/downloads/file.php?id=10193");
	link->SetBackgroundColour(wxColour(255,255,255));
	linkBox->Add(link);
	link = new wxHyperlinkCtrl(frame,-1,"New Vegas Nexus","http://www.newvegasnexus.com/downloads/file.php?id=35999");
	link->SetBackgroundColour(wxColour(255,255,255));
	linkBox->Add(link);

	bigBox->Add(updateBox, 0, wxALL, 20);
	bigBox->Add(textBox, 0,  wxLEFT | wxRIGHT, 20);
	bigBox->Add(linkBox, 0, wxALL, 20);

	//Now set the layout and sizes.
	frame->SetSizerAndFit(bigBox);

	frame->Show(TRUE);
}

void MainFrame::CheckForUpdate() {
	std::string updateText;
	if (boss::CheckConnection()) {
		try {
			updateText = boss::IsUpdateAvailable();
			if (updateText.length() != 0)
				updateText = "Update available! New version: " + updateText;
			else
				return;
		} catch (boss::update_error&) {
			return;
		}
	} else
		return;

	wxFrame *frame = new wxFrame(this, -1, "Check For Updates", wxPoint(100, 100), wxSize(450, 340),wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
	
	frame->SetBackgroundColour(wxColour(255,255,255));

	wxBoxSizer *bigBox = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *updateBox = new wxBoxSizer(wxVERTICAL);
	updateBox->Add(new wxStaticText(frame,-1,updateText, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP));
	
	wxBoxSizer *textBox = new wxBoxSizer(wxVERTICAL);
	textBox->Add(new wxStaticText(frame,-1,"BOSS updates can be downloaded from the following sites:"));
	
	wxBoxSizer *linkBox = new wxBoxSizer(wxVERTICAL);
	wxHyperlinkCtrl *link = new wxHyperlinkCtrl(frame,-1,"TES Nexus","http://www.tesnexus.com/downloads/file.php?id=20516");
	link->SetBackgroundColour(wxColour(255,255,255));
	linkBox->Add(link);
	link = new wxHyperlinkCtrl(frame,-1,"Fallout 3 Nexus","http://www.fallout3nexus.com/downloads/file.php?id=10193");
	link->SetBackgroundColour(wxColour(255,255,255));
	linkBox->Add(link);
	link = new wxHyperlinkCtrl(frame,-1,"New Vegas Nexus","http://www.newvegasnexus.com/downloads/file.php?id=35999");
	link->SetBackgroundColour(wxColour(255,255,255));
	linkBox->Add(link);

	bigBox->Add(updateBox, 0, wxALL, 20);
	bigBox->Add(textBox, 0,  wxLEFT | wxRIGHT, 20);
	bigBox->Add(linkBox, 0, wxALL, 20);

	//Now set the layout and sizes.
	frame->SetSizerAndFit(bigBox);

	frame->Show(TRUE);
}
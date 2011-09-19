/*	Better Oblivion Sorting Software
	
	Quick and Dirty Load Order Utility
	(Making C++ look like the scripting language it isn't.)

    Copyright (C) 2009-2010  Random/Random007/jpearce & the BOSS development team
    http://creativecommons.org/licenses/by-nc-nd/3.0/

	$Revision: 3184 $, $Date: 2011-08-26 20:52:13 +0100 (Fri, 26 Aug 2011) $
*/

#ifndef __BOSS_UPDATER_H__
#define __BOSS_UPDATER_H__

#ifndef BOOST_SPIRIT_UNICODE
#define BOOST_SPIRIT_UNICODE 
#endif

#include <string>
#include <vector>
//#include <wx/progdlg.h>

namespace boss {
	using namespace std;

	enum installType {  //Possible types of install the user has.
		MANUAL,
		INSTALLER,
		MASTERLIST
	};

	struct fileInfo {  //Holds the information for files to be downloaded.
		string name;
		unsigned int crc;

		fileInfo();
		fileInfo(string str);
	};

	struct uiStruct {
		void *p;
		bool isGUI;
		size_t fileIndex;

		uiStruct();
		uiStruct(void *GUIpoint);
	};

	extern vector<fileInfo> updatedFiles;  //The updated files. These don't have the .new extension.

	////////////////////////
	// General Functions
	////////////////////////

	int progress_func(void *data, double dlTotal, double dlNow, double ulTotal, double ulNow);

	//Checks if an Internet connection is present.
	bool CheckConnection();

	//Cleans up after the user cancels a download.
	void CleanUp();


	////////////////////////
	// Masterlist Updating
	////////////////////////

	//Updates the local masterlist to the latest available online.
	void UpdateMasterlist(uiStruct ui, unsigned int& localRevision, string& localDate, unsigned int& remoteRevision, string& remoteDate);


	////////////////////////
	// BOSS Updating
	////////////////////////

	//Checks if a new release of BOSS is available or not.
	string IsBOSSUpdateAvailable();

	//Downloads and installs a BOSS update.
	vector<string> DownloadInstallBOSSUpdate(uiStruct ui, const int updateType, const string updateVersion);
}
#endif
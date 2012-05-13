/*	BOSS
	
	A "one-click" program for users that quickly optimises and avoids 
	detrimental conflicts in their TES IV: Oblivion, Nehrim - At Fate's Edge, 
	TES V: Skyrim, Fallout 3 and Fallout: New Vegas mod load orders.

    Copyright (C) 2009-2012    BOSS Development Team.

	This file is part of BOSS.

    BOSS is free software: you can redistribute 
	it and/or modify it under the terms of the GNU General Public License 
	as published by the Free Software Foundation, either version 3 of 
	the License, or (at your option) any later version.

    BOSS is distributed in the hope that it will 
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BOSS.  If not, see 
	<http://www.gnu.org/licenses/>.

	$Revision: 3184 $, $Date: 2011-08-26 20:52:13 +0100 (Fri, 26 Aug 2011) $
*/

#include "Common/Game.h"
#include "Common/Globals.h"
#include "Support/Helpers.h"
#include "Support/Logger.h"

#if _WIN32 || _WIN64
#	include "Windows.h"
#	include "Shlobj.h"
#endif

namespace boss {
	using namespace std;
 
	uint32_t AutodetectGame(vector<uint32_t> detectedGames) {  //Throws exception if error.
		if (gl_last_game != AUTODETECT) {
			for (size_t i=0, max = detectedGames.size(); i < max; i++) {
				if (gl_last_game == detectedGames[i])
					return gl_last_game;
			}
		}
		LOG_INFO("Autodetecting game.");
		
		if (Game(NEHRIM, "", true).IsInstalledLocally())  //Before Oblivion because Nehrim installs can have Oblivion.esm for porting mods.
			return NEHRIM;
		else if (Game(OBLIVION, "", true).IsInstalledLocally())
			return OBLIVION;
		else if (Game(SKYRIM, "", true).IsInstalledLocally())
			return SKYRIM;
		else if (Game(FALLOUTNV, "", true).IsInstalledLocally())  //Before Fallout 3 because some mods for New Vegas require Fallout3.esm.
			return FALLOUTNV;
		else if (Game(FALLOUT3, "", true).IsInstalledLocally())
			return FALLOUT3;
		else if (Game(MORROWIND, "", true).IsInstalledLocally())
			return MORROWIND;
		else {
			LOG_INFO("No game detected locally. Using Registry paths.");
			return AUTODETECT;
		}
	}

	BOSS_COMMON uint32_t DetectGame(vector<uint32_t>& detectedGames, vector<uint32_t>& undetectedGames) {
		//Detect all installed games.
		if (Game(OBLIVION, "", true).IsInstalled()) //Look for Oblivion.
			detectedGames.push_back(OBLIVION);
		else
			undetectedGames.push_back(OBLIVION);
		if (Game(NEHRIM, "", true).IsInstalled()) //Look for Nehrim.
			detectedGames.push_back(NEHRIM);
		else
			undetectedGames.push_back(NEHRIM);
		if (Game(SKYRIM, "", true).IsInstalled()) //Look for Skyrim.
			detectedGames.push_back(SKYRIM);
		else
			undetectedGames.push_back(SKYRIM);
		if (Game(FALLOUT3, "", true).IsInstalled()) //Look for Fallout 3.
			detectedGames.push_back(FALLOUT3);
		else
			undetectedGames.push_back(FALLOUT3);
		if (Game(FALLOUTNV, "", true).IsInstalled()) //Look for Fallout New Vegas.
			detectedGames.push_back(FALLOUTNV);
		else
			undetectedGames.push_back(FALLOUTNV);
		if (Game(MORROWIND, "", true).IsInstalled()) //Look for Morrowind.
			detectedGames.push_back(MORROWIND);
		else
			undetectedGames.push_back(MORROWIND);
			
		//Now set return a game.
		if (gl_game != AUTODETECT) {
			if (gl_update_only)
				return gl_game;
			else if (Game(gl_game, "", true).IsInstalled())
				return gl_game;
			else
				return AutodetectGame(detectedGames);  //Game not found. Autodetect.
		} else
			return AutodetectGame(detectedGames);
	}

	////////////////////////////
	// Game Class Functions
	////////////////////////////

	Game::Game() {
		id = AUTODETECT;
		name = "";

		executable = "";
		masterFile = "";
		scriptExtender = "";
		seExecutable = "";
			
		registryKey = "";
		registrySubKey = "";
			
		bossFolderName = "";
		appdataFolderName = "";
		pluginsFolderName = "";
		pluginsFileName = "";

		gamePath = fs::path();
		pluginsPath = fs::path();
		loadorderPath = fs::path();
	}
	
	Game::Game(uint32_t inGame, string dataFolder, bool noPathInit) {
		id = inGame;
		if (inGame == OBLIVION) {
			name = "TES IV: Oblivion";

			executable = "Oblivion.exe";
			masterFile = "Oblivion.esm";
			scriptExtender = "OBSE";
			seExecutable = "obse_1_2_416.dll";
			
			registryKey = "Software\\Bethesda Softworks\\Oblivion";
			registrySubKey = "Installed Path";
			
			bossFolderName = "Oblivion";
			appdataFolderName = "Oblivion";
			pluginsFolderName = "Data";
			pluginsFileName = "plugins.txt";
		} else if (inGame == NEHRIM) {
			name = "Nehrim - At Fate's Edge";
			
			executable = "Oblivion.exe";
			masterFile = "Nehrim.esm";
			scriptExtender = "OBSE";
			seExecutable = "obse_1_2_416.dll";
			
			registryKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nehrim - At Fate's Edge_is1";
			registrySubKey = "InstallLocation";
			
			bossFolderName = "Nehrim";
			appdataFolderName = "Oblivion";
			pluginsFolderName = "Data";
			pluginsFileName = "plugins.txt";
		} else if (inGame == SKYRIM) {
			name = "TES V: Skyrim";
			
			executable = "TESV.exe";
			masterFile = "Skyrim.esm";
			scriptExtender = "SKSE";
			seExecutable = "skse_loader.exe";
			
			registryKey = "Software\\Bethesda Softworks\\Skyrim";
			registrySubKey = "Installed Path";
			
			bossFolderName = "Skyrim";
			appdataFolderName = "Skyrim";
			pluginsFolderName = "Data";
			pluginsFileName = "plugins.txt";
		} else if (inGame == FALLOUT3) {
			name = "Fallout 3";
			
			executable = "Fallout3.exe";
			masterFile = "Fallout3.esm";
			scriptExtender = "FOSE";
			seExecutable = "fose_loader.exe";
			
			registryKey = "Software\\Bethesda Softworks\\Fallout3";
			registrySubKey = "Installed Path";
			
			bossFolderName = "Fallout 3";
			appdataFolderName = "Fallout3";
			pluginsFolderName = "Data";
			pluginsFileName = "plugins.txt";
		} else if (inGame == FALLOUTNV) {
			name = "Fallout: New Vegas";
			
			executable = "FalloutNV.exe";
			masterFile = "FalloutNV.esm";
			scriptExtender = "NVSE";
			seExecutable = "nvse_loader.exe";
			
			registryKey = "Software\\Bethesda Softworks\\FalloutNV";
			registrySubKey = "Installed Path";
			
			bossFolderName = "Fallout New Vegas";
			appdataFolderName = "FalloutNV";
			pluginsFolderName = "Data";
			pluginsFileName = "plugins.txt";
		} else if (inGame == MORROWIND) {
			name = "TES III: Morrowind";
			
			executable = "Morrwind.exe";
			masterFile = "Morrowind.esm";
			scriptExtender = "MWSE";
			seExecutable = "MWSE.dll";
			
			registryKey = "Software\\Bethesda Softworks\\Morrowind";
			registrySubKey = "Installed Path";
			
			bossFolderName = "Morrowind";
			appdataFolderName = "";
			pluginsFolderName = "Data Files";
			pluginsFileName = "Morrowind.ini";
		} else
			throw boss_error(BOSS_ERROR_NO_GAME_DETECTED);
		
		if (!noPathInit) {
			if (dataFolder.empty()) {
				//First look for local install, then look for Registry.
				if (fs::exists(boss_path / ".." / pluginsFolderName / masterFile) || gl_update_only)
					gamePath = boss_path / "..";
				else if (RegKeyExists("HKEY_LOCAL_MACHINE", registryKey, registrySubKey))
					gamePath = fs::path(RegKeyStringValue("HKEY_LOCAL_MACHINE", registryKey, registrySubKey));
				else if (gl_update_only)  //Update only games are treated as installed locally if not actually installed.
					gamePath = boss_path / "..";
				else
					throw boss_error(BOSS_ERROR_NO_GAME_DETECTED);
			}
			
			//Check if game master file exists. Requires data path to be set. This should call MasterFile().Exists(), but that depends on gl_current_game ATM.
			if (!fs::exists(DataFolder() / MasterFile().Name()))
				throw boss_error(BOSS_ERROR_FILE_NOT_FOUND, MasterFile().Name());
			
			//Requires data path to be set.
			if (inGame == OBLIVION) {
				//Looking up bUseMyGamesDirectory, which only has effect if =0 and exists in Oblivion folder.
				if (fs::exists(GameFolder() / "Oblivion.ini")) {
					Settings oblivionIni;
					oblivionIni.Load(GameFolder() / "Oblivion.ini");  //This also sets the variable up.

					if (oblivionIni.GetValue("bUseMyGamesDirectory") == "0") {
						pluginsPath = GameFolder() / pluginsFileName;
						loadorderPath = GameFolder() / "loadorder.txt";
					} else {
						pluginsPath = GetLocalAppDataPath() / appdataFolderName / pluginsFileName;
						loadorderPath = GetLocalAppDataPath() / appdataFolderName / "loadorder.txt";
					}
				}
			} else if (inGame == MORROWIND) {
				pluginsPath = GameFolder() / pluginsFileName;
				loadorderPath = GameFolder() / "loadorder.txt";
			} else {
				pluginsPath = GetLocalAppDataPath() / appdataFolderName / pluginsFileName;
				loadorderPath = GetLocalAppDataPath() / appdataFolderName / "loadorder.txt";
			}
		
			//Make sure that the BOSS game path exists.
			try {
				if (!fs::exists(boss_path / bossFolderName))
					fs::create_directory(boss_path / bossFolderName);
			} catch (fs::filesystem_error e) {
				throw boss_error(BOSS_ERROR_FS_CREATE_DIRECTORY_FAIL, fs::path(boss_path / bossFolderName).string(), e.what());
			}
		}
	}
	
	bool Game::IsInstalled() const {
		return (IsInstalledLocally() || RegKeyExists("HKEY_LOCAL_MACHINE", registryKey, registrySubKey));
	}
	
	bool Game::IsInstalledLocally() const {
		return fs::exists(boss_path / ".." / pluginsFolderName / masterFile);
	}
	
	uint32_t Game::Id() const {
		return id;
	}

	string Game::Name() const {
		return name;
	}

	string Game::ScriptExtender() const {
		return scriptExtender;
	}
	
	Item Game::MasterFile() const {
		return Item(masterFile);
	}

	Version Game::GetVersion() const {
		return Version(Executable());
	}

	fs::path Game::Executable() const {
		return GameFolder() / executable;
	}

	fs::path Game::GameFolder() const {
		return gamePath;
	}
	
	fs::path Game::DataFolder() const {
		return GameFolder() / pluginsFolderName;
	}
	
	fs::path Game::SEPluginsFolder() const {
		return DataFolder() / scriptExtender / "Plugins";
	}
	
	fs::path Game::SEExecutable() const {
		return GameFolder() / seExecutable;
	}
	
	fs::path Game::ActivePluginsFile() const {
		return pluginsPath;
	}
	
	fs::path Game::LoadOrderFile() const {
		return loadorderPath;
	}
	
	fs::path Game::Masterlist() const {
		return boss_path / bossFolderName / "masterlist.txt";
	}
	
	fs::path Game::Userlist() const {
		return boss_path / bossFolderName / "userlist.txt";
	}
	
	fs::path Game::Modlist() const {
		return boss_path / bossFolderName / "modlist.txt";
	}
	
	fs::path Game::OldModlist() const {
		return boss_path / bossFolderName / "modlist.old";
	}
	
	fs::path Game::Log(uint32_t format) const {
		if (format == HTML)
			return boss_path / bossFolderName / "BOSSlog.html";
		else
			return boss_path / bossFolderName / "BOSSlog.txt";
	}

	//Can be used to get the location of the LOCALAPPDATA folder (and its Windows XP equivalent).
	fs::path Game::GetLocalAppDataPath() {
#if _WIN32 || _WIN64
		HWND owner;
		TCHAR path[MAX_PATH];

		HRESULT res = SHGetFolderPath(owner, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);

		if (res == S_OK)
			return fs::path(path);
		else
			return fs::path("");
#else
		return fs::path("");
#endif
	}
}
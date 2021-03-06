/*	Better Oblivion Sorting Software
	
	Quick and Dirty Load Order Utility
	(Making C++ look like the scripting language it isn't.)

    Copyright (C) 2009-2010  Random/Random007/jpearce & the BOSS development team
    http://creativecommons.org/licenses/by-nc-nd/3.0/

	$Revision: 2102 $, $Date: 2010-12-31 22:34:08 +0000 (Fri, 31 Dec 2010) $
*/

#include "Support/Logger.h"
#include "Userlist.h"
#include <boost/algorithm/string.hpp>


namespace fs = boost::filesystem;


namespace boss {
	using namespace std;
	using boost::algorithm::to_lower_copy;
	using boost::algorithm::to_lower;
	using boost::algorithm::to_upper_copy;
	using boost::algorithm::trim_copy;
	using boost::wformat;

	namespace { // Using an anonymous so local private declarations are only usable from inside this file.
		

		// Error messages for rule validation
		static boost::wformat ESortLineInForRule(L"It includes a sort line in a rule with a FOR rule keyword.");
		static boost::wformat ERuleHasUndefinedObject(L"The line with keyword '%1%' has an undefined object.");
		static boost::wformat EPluginNotInstalled(L"'%1%' is not installed.");
		static boost::wformat EAddingModGroup(L"It tries to add a group.");
		static boost::wformat ESortingGroupEsms(L"It tries to sort the group \"ESMs\".");
		static boost::wformat ESortingMasterEsm(L"It tries to sort the master .ESM file.");
		static boost::wformat EReferencingModAndGroup(L"It references a mod and a group.");
		static boost::wformat ESortingGroupBeforeEsms(L"It tries to sort a group before the group \"ESMs\".");
		static boost::wformat ESortingModBeforeGameMaster(L"It tries to sort a mod before the master .ESM file.");
		static boost::wformat EInsertingToTopOfEsms(L"It tries to insert a mod into the top of the group \"ESMs\", before the master .ESM file.");
		static boost::wformat EInsertingGroupToGroupOrModToMod(L"It tries to insert a group or insert a mod into another mod.");
		static boost::wformat EAttachingMessageToGroup(L"It tries to attach a message to a group.");
		static boost::wformat EUnrecognisedKeyword(L"The line \"%1%: %2%\" does not contain a recognised keyword. If this line is the start of a rule, that rule will also be skipped.");
		static boost::wformat EAppearsBeforeFirstRule(L"The line \"%1%: %2%\" appears before the first recognised rule line. Line skipped.");
		static boost::wformat EUnrecognizedKeywordBeforeFirstRule(L"The line \"%1%: %2%\" does not contain a recognised keyword, and appears before the first recognised rule line. Line skipped.");

		static boost::wformat MessageParagraphFormat(
			L"<p style='margin-left:40px; text-indent:-40px;'\n"
			 "	The rule beginning \" %1%: %2%\" has been skipped as it has the following problem(s):\n"
			 "	<br/>\n"
			 "	%3%\n"
			 "	<br/>\n"
			 "</p>\n"
			);

		static boost::wformat MessageSpanFormat(
			L"	<span class='%1%'>\n"
			 "		%2%\n"
			 "	</span>\n"
			);

		// Used to throw as exception when signaling a rule parsing error, in order to make the code a bit more compact.
		struct failure
		{
			failure(bool skipped, wstring const& rule, wstring const& object, boost::wformat const& message) 
				: skipped(skipped), object(object), rule(rule), message(message)
			{}

			wstring object;
			wstring rule;
			wformat message;
			bool skipped;
		};
	}

	//Date comparison, used for sorting mods in modlist class.
	bool SortByDate(wstring mod1,wstring mod2) {
		time_t t1 = 0,t2 = 0;
		try {
			t1 = fs::last_write_time(data_path / mod1);
			t2 = fs::last_write_time(data_path / mod2);
		}catch (fs::filesystem_error e){
			LOG_WARN("%s; Report the mod in question with a download link to an official BOSS thread.", e.what());
		}
		double diff = difftime(t1,t2);

		if (diff > 0)
			return false;
		else
			return true;
	}

	//Checks if a given object is an esp, an esm or a ghosted mod.
	bool IsPlugin(wstring object) {
		to_lower(object);
		return (object.find(L".esp")!=string::npos || object.find(L".esm")!=string::npos);
	}

	bool Exists(wstring object) 
	{
		return fs::exists(data_path / object) || fs::exists(data_path / fs::path(object + L".ghost"));
	}

	//Prints messages generated by userlist parsing and rule processing to the output file stream given.
	void Rules::PrintMessages(wofstream& output) {
		output << messages;
	}

	//Add rules from userlist.txt into the rules object.
	//Then checks rule syntax and discards rules with incorrect structures.
	//Also checks if the mods referenced by rules are in your Data folder, and discards rule that reference missing mods.
	//Generates error messages for rules that are discarded.
	void Rules::AddRules() {
		wifstream userlist;
		wstring line,key,object;
		size_t pos;
		bool skip = false;
		messages += L"<p>";
		userlist.open(userlist_path.c_str());
		while (!userlist.eof()) {
			wchar_t cbuffer[MAXLENGTH];
			userlist.getline(cbuffer,MAXLENGTH);
			line=cbuffer;

			if (line.length() == 0) continue;			

			if (line.substr(0,2) == L"//") continue;

			pos = line.find(L":");
			if (pos == wstring::npos) continue;

			key = to_upper_copy(trim_copy(line.substr(0,pos)));
			object = trim_copy(line.substr(pos+1));

			try {
				if (key==L"add" || key==L"override" || key==L"for") {

					if (skip) {
						keys.erase(keys.begin() + rules.back(), keys.end());
						objects.erase(objects.begin() + rules.back(), objects.end());
						rules.pop_back();
					}

					keys.push_back(key);
					objects.push_back(object);
					rules.push_back((int)keys.size() - 1);
					skip = false;

					const wstring rule = keys[rules.back()];
					const wstring subject = objects[rules.back()];

					if (object.empty()) {
						throw failure(skip, rule, subject, ERuleHasUndefinedObject % key);
					}
				
					if (IsPlugin(object) && !Exists(object)) {
						throw failure(skip, rule, subject, EPluginNotInstalled % object);
					} 
				
					if (key==L"add" && !IsPlugin(object)) {
						throw failure(skip, rule, subject, EAddingModGroup);
					} 
					
					if (key==L"override") {
						if (Tidy(object)==L"esms") {
							throw failure(skip, rule, subject, ESortingGroupEsms);
						}
						
						if (Tidy(object)==L"oblivion.esm" || Tidy(object)==L"fallout3.esm" || Tidy(object)==L"nehrim.esm" || Tidy(object)==L"falloutnv.esm") {
							throw failure(skip, rule, subject, ESortingMasterEsm);
						}
					}
				

				} else if (!rules.empty()) {

					if ((key==L"before" || key==L"after")) {
						keys.push_back(key);
						objects.push_back(object);

						const wstring rule = keys[rules.back()];
						const wstring subject = objects[rules.back()];

						if (rule==L"for") {
							throw failure(skip, rule, subject, ESortLineInForRule);
						}

						if (object.empty()) {
							throw failure(skip, rule, subject, ERuleHasUndefinedObject % key);
						} 

						if (subject.length() > 0 && ((IsPlugin(object) && !IsPlugin(subject)) || (!IsPlugin(object) && IsPlugin(subject)))) {
							throw failure(skip, rule, subject, EReferencingModAndGroup);
						}

						if (key==L"before") {
							if (Tidy(object)==L"esms") {
								throw failure(skip, rule, subject, ESortingGroupBeforeEsms);
							} 
							
							if (Tidy(object)==L"oblivion.esm" || Tidy(object)==L"fallout3.esm" || Tidy(object)==L"nehrim.esm" || Tidy(object)==L"falloutnv.esm") {
								throw failure(skip, rule, subject, ESortingModBeforeGameMaster);
							}
						}

					} else if ((key==L"top" || key==L"bottom")) {
						keys.push_back(key);
						objects.push_back(object);

						const wstring rule = keys[rules.back()];
						const wstring subject = objects[rules.back()];

						if (rule==L"for") {
							throw failure(skip, rule, subject, ESortLineInForRule);
						}

						if (object.empty()) {
							throw failure(skip, rule, subject, ERuleHasUndefinedObject % key);
						} 
						
						if (Tidy(object)==L"esms" && key==L"top") {
							throw failure(skip, rule, subject, EInsertingToTopOfEsms);
						}
						
						if (subject.length() > 0 && !IsPlugin(subject) || IsPlugin(object)) {
							throw failure(skip, rule, subject, EInsertingGroupToGroupOrModToMod);
						}

					} else if ((key==L"append" || key==L"replace")) {					
						keys.push_back(key);
						objects.push_back(object);

						const wstring rule = keys[rules.back()];
						const wstring subject = objects[rules.back()];

						if (object.empty()) {
							throw failure(skip, rule, subject, ERuleHasUndefinedObject % key);
						}

						if (!IsPlugin(subject)) {
							throw failure(skip, rule, subject, EAttachingMessageToGroup);
						}

					} else {
						const wstring rule = keys[rules.back()];
						const wstring subject = objects[rules.back()];

						//Line does not contain a recognised keyword. Skip it and the rule containing it. If it is a rule line, then the previous rule will also be skipped.
						throw failure(skip, rule, subject, EUnrecognisedKeyword % key % object);
					}

				} else {
					//Line is not a rule line, and appears before the first rule line, so does not belong to a rule. Skip it.
					if (key==L"before" || key==L"after" || key==L"top" || key==L"bottom" || key==L"append" || key==L"replace") 
						AddError(EAppearsBeforeFirstRule % key % object);
					else
						AddError(EUnrecognizedKeywordBeforeFirstRule % key % object);
				}

			} catch(failure const& e) {
				skip = true;
				AddError(e.skipped, e.rule, e.object, e.message);
			}
		}

		userlist.close();
		messages += L"</p>";
		if (skip) {
			keys.erase(keys.begin()+rules.back(), keys.end());
			objects.erase(objects.begin()+rules.back(), objects.end());
			rules.pop_back();
		}
	}

	const wstring Rules::FormatMesssage(wstring const& class_, wstring const& rule, wstring const& object, wformat const& message)
	{
		wstring const span = FormatMesssage(class_ , message);
		return (MessageParagraphFormat % rule % object % span).str();
	}

	const wstring Rules::FormatMesssage(wstring const& class_, wformat const& message)
	{
		return (MessageSpanFormat % class_ % message.str()).str();
	}

	void Rules::AddMessage(bool skipped, wstring const& rule, wstring const& object, wformat const& message, wstring const& class_)
	{
		if (!skipped) {
			messages += FormatMesssage(class_, rule, object, message) ;
		} else {
			messages += FormatMesssage(class_, message) ;
		}
	}

	void Rules::AddError(bool skipped, wstring const& rule, wstring const& object, wformat const& message)
	{
		AddMessage(skipped, rule, object, message, L"error");
	}

	void Rules::AddError(wformat const& message)
	{
		messages += FormatMesssage(L"error", message);
	}

	void Rules::AddRule(parsing::Rule const& rule)
	{
		/*
		A rule has been parsed in full:

			Here goes: 
				- the validation code found above in AddRules()
				- Add the Rule definition to our local arrays for later use.			
		*/
	};

	// Called when an error is detected while parsing the input file.
	void Rules::SyntaxError(
			string::const_iterator const& begin, 
			string::const_iterator const& end, 
			string::const_iterator const& error_pos, 
			string const& what) 
	{
		std::string context(error_pos, std::min(error_pos + 50, end));
		boost::trim_left(context);
		boost::replace_all(context, "\n", "<EOL>");

		std::cerr << "Syntax error while trying to parse Userlist.txt: '" << what << "' near this input: '" << context << "'." << std::endl;
	};

	void Rules::ParsingFailed(
			string::const_iterator	const& begin, 
			string::const_iterator	const& end, 
			string::const_iterator	const& error_pos, 
			string::difference_type lineNo)
	{
		string context(error_pos, std::min(error_pos + 50, end));
		boost::trim_left(context);
		boost::replace_all(context, "\n", "<EOL>");

		std::cerr << "Userlist.txt parsing error at line#: " << lineNo << " while reading near this input: '" << context << "'." << std::endl;
	};


	//Adds mods in directory to modlist in date order (AKA load order).
	void Mods::AddMods() {
		LOG_DEBUG("Reading user mods...");
		if (fs::is_directory(data_path)) {
			for (fs::directory_iterator itr(data_path); itr!=fs::directory_iterator(); ++itr) {
				const wstring filename = itr->path().filename().wstring();
				wcout << filename.c_str() <<endl;
				const wstring ext = to_lower_copy(itr->path().extension().wstring());
				if (fs::is_regular_file(itr->status()) && (ext==L".esp" || ext==L".esm" || ext==L".ghost")) {
					LOG_TRACE("-- Found mod: '%s'", filename.c_str());			
					mods.push_back(filename);
				}
			}
		}
		modmessages.resize((int)mods.size());
		sort(mods.begin(),mods.end(),SortByDate);
		LOG_DEBUG("Reading user mods done: %" PRIuS " total mods found.", mods.size());
	}

	//Save mod list to modlist.txt. Backs up old modlist.txt as modlist.old first.
	int Mods::SaveModList() {
		wofstream modlist;
		try {
			LOG_DEBUG("Saving backup of current modlist...");
			if (fs::exists(curr_modlist_path)) fs::rename(curr_modlist_path, prev_modlist_path);
		} catch(boost::filesystem::filesystem_error e) {
			//Couldn't rename the file, print an error message.
			LOG_ERROR("Backup of modlist failed with error: %s", e.what());
			return 1;
		}
		
		modlist.open(curr_modlist_path.c_str());
		//Provide error message if it can't be written.
		if (modlist.fail()) {
			LOG_ERROR("Backup cannot be saved.");
			return 2;
		}
		for (int i=0;i<(int)mods.size();i++) {
			modlist << mods[i] << endl;
		}
		modlist.close();
		LOG_INFO("Backup saved successfully.");
		return 0;
	}

	//Look for a mod in the modlist, even if the mod in question is ghosted.
	int Mods::GetModIndex(wstring mod) {
		for (int i=0;i<(int)mods.size();i++) {
			if (Tidy(mods[i])==Tidy(mod) || Tidy(mods[i])==Tidy(mod+L".ghost")) return i;
		}
		return -1;
	}
}
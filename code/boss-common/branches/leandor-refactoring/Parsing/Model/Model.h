#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>

#include "Model/Keywords.h"

namespace boss { namespace parsing {


	struct RuleAction
	{
		ActionKeyword		key;
		std::string			argument;
	};

	struct RuleActions : public std::vector<RuleAction> 
	{
	};


	struct RuleHeader
	{
		OperationKeyword	key;
		std::string			subject;
	};

	struct Rule 
	{
		RuleHeader			header;
		RuleActions			actions;
	};

	struct Rules : public std::vector<Rule>
	{
	};

	//- Helper operators for printing the resulting structure -{{{
	std::ostream& operator<< (std::ostream& os, ActionKeyword const& action)
	{
		switch (action.Value)
		{
		case ActionKeyword::AFTER :
			return os << "AFTER";

		case ActionKeyword::APPEND :
			return os << "APPEND";

		case ActionKeyword::BEFORE :
			return os << "BEFORE";

		case ActionKeyword::BOTTOM :
			return os << "BOTTOM";

		case ActionKeyword::REPLACE :
			return os << "REPLACE";

		case ActionKeyword::TOP :
			return os << "TOP";
		}

		return os;
	};

	std::ostream& operator<< (std::ostream& os, OperationKeyword const& operation)
	{
		switch (operation.Value)
		{
		case OperationKeyword::ADD :
			return os << "ADD";

		case OperationKeyword::OVERRIDE :
			return os << "OVERRIDE";

		case OperationKeyword::FOR :
			return os << "FOR";
		}

		return os;
	};

	std::ostream& operator<< (std::ostream& os, RuleHeader const& header)
	{
		os << "OPERATION: " << header.key << " ON: \"" << header.subject << "\"" << std::endl;
		return os;
	};

	std::ostream& operator<< (std::ostream& os, RuleAction const& action)
	{
		os << "    - RULE: " << action.key << "('" << action.argument << "')" << std::endl;

		return os;
	};

	std::ostream& operator<< (std::ostream& os, RuleActions const& actions)
	{
		BOOST_FOREACH(RuleAction const& action, actions)
		{
			os << action;
		}

		return os;
	};

	std::ostream& operator<< (std::ostream& os, Rule const& rule)
	{
		os << rule.header << rule.actions;
		return os;
	};

	std::ostream& operator<< (std::ostream& os, Rules const& rules)
	{
		BOOST_FOREACH(Rule const& rule, rules)
		{
			os << rule;
		}

		return os;
	};
	//}}}
}}
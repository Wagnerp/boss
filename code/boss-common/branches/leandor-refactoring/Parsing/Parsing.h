#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace boss { namespace parsing {
	namespace spirit	= boost::spirit;
	namespace qi		= boost::spirit::qi;
	namespace fusion	= boost::fusion;
	namespace phoenix	= boost::phoenix;
	namespace iso8859_1 = boost::spirit::iso8859_1;

	using fusion::unused_type;
}};


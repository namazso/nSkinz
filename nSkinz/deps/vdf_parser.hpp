//MIT License
//
//Copyright(c) 2016 Matthias Moeller
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#ifndef __TYTI_STEAM_VDF_PARSER_H__
#define __TYTI_STEAM_VDF_PARSER_H__

#include <vector>
#include <unordered_map>
#include <utility>
#include <fstream>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/config.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp> 
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace tyti
{
    namespace vdf
    {
        template<typename CharT>
        struct basic_object;

        template<typename iStreamT, typename charT>
        basic_object<charT> read(iStreamT& inStream, bool *ok);

        namespace detail
        {
            ///////////////////////////////////////////////////////////////////////////
            //  Grammar data structure
            ///////////////////////////////////////////////////////////////////////////
            template<typename T>
            struct parser_ast;

            template<typename charT>
            struct variant
            {
                typedef boost::variant <
                    boost::recursive_wrapper< parser_ast<charT> > //cildren
                    , std::pair<std::basic_string<charT>, std::basic_string<charT> > //attribute
                    , std::basic_string<charT> //includes
                > parser_node; 
            };



            template<typename charT>
            struct parser_ast
            {
                std::basic_string<charT> name;                           // tag name
                std::vector< typename variant<charT>::parser_node > children;        // children
            };


            ///////////////////////////////////////////////////////////////////////////
            //  Helper functions selecting the right encoding (char/wchar_T)
            ///////////////////////////////////////////////////////////////////////////

            template<typename T>
            struct literal_macro_help
            {
                static const char* result(const char* c, const wchar_t* wc)
                {
                    return c;
                }
                static const char result(char c, wchar_t wc)
                {
                    return c;
                }
            };

            template<>
            struct literal_macro_help<wchar_t>
            {
                static const wchar_t* result(const char* c, const wchar_t* wc)
                {
                    return wc;
                }
                static const wchar_t result(char c, wchar_t wc)
                {
                    return wc;
                }
            };
#define TYTI_L(type, text) vdf::detail::literal_macro_help<type>::result(text, L##text)

            template<typename T>
            struct encoding_selector
            {
                typedef boost::spirit::standard::char_type char_type;
                typedef boost::spirit::standard::space_type space_type;
                static const char_type char_;
                static const space_type space;
            };

            template<>
            struct encoding_selector<wchar_t>
            {
                typedef boost::spirit::standard_wide::char_type char_type;
                typedef boost::spirit::standard_wide::space_type space_type;
                static const char_type char_;
                static const space_type space;
            };
            template<typename T>
            const typename encoding_selector<T>::char_type encoding_selector<T>::char_ = boost::spirit::standard::char_;
            template<typename T>
            const typename encoding_selector<T>::space_type encoding_selector<T>::space = boost::spirit::standard::space;
            const typename encoding_selector<wchar_t>::char_type encoding_selector<wchar_t>::char_ = boost::spirit::standard_wide::char_;
            const typename encoding_selector<wchar_t>::space_type encoding_selector<wchar_t>::space = boost::spirit::standard_wide::space;

            ///////////////////////////////////////////////////////////////////////////
            //  Grammar
            ///////////////////////////////////////////////////////////////////////////
            template<typename CharT, typename Iterator>
            struct vdf_skipper : public boost::spirit::qi::grammar<Iterator> {

            private:
                typedef encoding_selector<typename Iterator::value_type> es;
            public:

                vdf_skipper() : vdf_skipper::base_type(skip, "comment_skipper") {
                    //skip whitespace or C++ comment until next line
                    comment = (TYTI_L(CharT, "//") >> *(es::char_ - TYTI_L(CharT, '\n')));
                    skip = es::space | comment;
                }
                boost::spirit::qi::rule<Iterator> skip;
                boost::spirit::qi::rule<Iterator> comment;
            };

            template <typename CharT, typename Iterator>
            struct vdf_grammar
                : boost::spirit::qi::grammar<Iterator, parser_ast<CharT>(), boost::spirit::qi::locals< std::basic_string<CharT> >, vdf_skipper<CharT, Iterator> >
            {
                typedef CharT char_type;
                typedef std::basic_string<char_type> gr_string;
                typedef encoding_selector<char_type> es;
                typedef typename variant<char_type>::parser_node parser_node;
                vdf_grammar()
                    : vdf_grammar::base_type(vdf, "vdf")
                {
                    namespace spirit = boost::spirit;
                    namespace phoenix = boost::phoenix;
                    using spirit::qi::lit;
                    using spirit::qi::lexeme;
                    using spirit::qi::on_error;
                    using spirit::qi::fail;
                    using namespace spirit::qi::labels;

                    using phoenix::construct;
                    using phoenix::val;

                    quoted_string %= lexeme[TYTI_L(char_type, '"') >> 
                        (es::char_ - TYTI_L(char_type, '"') /*- TYTI_L(char_type, '#')
															I have no idea how all this works,
															but this crashes items_game.txt parsing*/) >> 
                        *(es::char_ - TYTI_L(char_type, '"')) >> TYTI_L(char_type, '"')];

                    include %= (lit(TYTI_L(char_type,"\"#base\"")) | lit(TYTI_L(char_type,"\"#include\""))) > quoted_string;
                    text %= quoted_string > quoted_string;
                    node %= vdf | text | include;

                    vdf %=
                        quoted_string >> TYTI_L(char_type, "{")
                        >> *node >> TYTI_L(char_type, "}");

                    vdf.name("vdf");
                    node.name("node");
                    text.name("text");
                    quoted_string.name("string");

                    //on_error<fail>
                    //    (
                    //        vdf
                    //        , std::cout
                    //        << val("Error! Expecting ")
                    //        << spirit::_4                               // what failed?
                    //        << val(" here: \"")
                    //        << construct<std::string>(spirit::_3, spirit::_2)   // iterators to error-pos, end
                    //        << val("\"")
                    //        << std::endl
                    //        );
                }

                typedef vdf_skipper<CharT, Iterator> skipper;
                //typedef typename es::space_type skipper;

                boost::spirit::qi::rule<Iterator, parser_ast<CharT>(), boost::spirit::qi::locals<gr_string>, skipper> vdf;
                boost::spirit::qi::rule<Iterator, parser_node(), skipper> node;
                boost::spirit::qi::rule<Iterator, std::pair<gr_string, gr_string>(), skipper> text;
                boost::spirit::qi::rule<Iterator, gr_string(), skipper> quoted_string;
                boost::spirit::qi::rule<Iterator, gr_string(), skipper> include;
            };

            ///////////////////////////////////////////////////////////////////////////
            //  Visitor
            ///////////////////////////////////////////////////////////////////////////

            // trasformation from parser_ast to output tree via objects
            template<typename charT>
            struct vdf_praser_ast_visitor : public boost::static_visitor<>
            {
                typedef basic_object<charT> vis_object;
                typedef std::pair<std::basic_string<charT>, std::basic_string<charT> > vis_key_value;
                vis_object& m_currentObj;
            public:
                vdf_praser_ast_visitor(vis_object& t) : m_currentObj(t) {}
                void operator()(std::pair<std::basic_string<charT>, std::basic_string<charT> > in) const
                {
                    m_currentObj.attribs.insert(std::move(in));
                }
                void operator()(std::basic_string<charT> in) const
                {
                    std::basic_ifstream<charT> file(in);
                    if (!file.is_open())
                    {
                        std::cerr << "Could not open file: " << std::string(in.begin(), in.end()) << std::endl;
                        return;
                    }
                    auto obj = tyti::vdf::read(file);
                    m_currentObj.childs.emplace(obj.name, std::move(obj));
                }
                void operator()(const parser_ast<charT>& x) const
                {
                    vis_object t;
                    t.name = x.name;
                    for (auto& i : x.children)
                        boost::apply_visitor(vdf_praser_ast_visitor<charT>(t), i);
                    m_currentObj.childs.emplace(t.name, std::move(t));
                }

            };

            ///////////////////////////////////////////////////////////////////////////
            //  Writer helper functions
            ///////////////////////////////////////////////////////////////////////////

            struct tabs
            {
                size_t t;
                tabs(size_t i) :t(i) {}
            };

            template<typename oStreamT>
            oStreamT& operator<<(oStreamT& s, tabs t)
            {
                for (; t.t > 0; --t.t)
                    s << "\t";
                return s;
            }
        } // end namespace detail

        ///////////////////////////////////////////////////////////////////////////
        //  Interface
        ///////////////////////////////////////////////////////////////////////////

        /// basic object node. Every object has a name and can contains attributes saved as key_value pairs or childrens
        template<typename CharT>
        struct basic_object
        {
            typedef CharT char_type;
            std::basic_string<char_type> name;
            std::unordered_map<std::basic_string<char_type>, std::basic_string<char_type> > attribs;
            std::unordered_map<std::basic_string<char_type>, basic_object<char_type> > childs;
        };

        typedef basic_object<char> object;
        typedef basic_object<wchar_t> wobject;

        /** \brief writes given object tree in vdf format to given stream.
        Uses tabs instead of whitespaces.
        */
        template<typename oStreamT, typename charT = typename oStreamT::char_type>
        void write(oStreamT& s, const basic_object<charT>& r, size_t t = 0)
        {
            using namespace detail;
            s << tabs(t) << TYTI_L(charT, '"') << r.name << TYTI_L(charT, "\"\n") << tabs(t) << TYTI_L(charT, "{\n");
            for (auto& i : r.attribs)
                s << tabs(t + 1) << TYTI_L(charT, '"') << i.first << TYTI_L(charT, "\"\t\t\"") << i.second << TYTI_L(charT, "\"\n");
            for (auto& i : r.childs)
                write(s, i, t + 1);
            s << tabs(t) << TYTI_L(charT, "}\n");
        }


        /** \brief Read VDF formatted sequences defined by the range [first, last).
        If the file is mailformatted, parser will try to read it until it can.
        @param first begin iterator
        @param end end iterator
        @param ok output bool. true, if parser successed, false, if parser failed
        */
        template<typename IterT, typename charT = typename IterT::value_type>
        basic_object<charT> read(IterT first, IterT last, bool* ok = 0)
        {
            using namespace detail;
            parser_ast<charT> ast;
            vdf_grammar<charT, IterT> parser;
            vdf_skipper<charT, IterT> skipper;
            bool r = boost::spirit::qi::phrase_parse(first, last, parser, skipper, ast);
            if (ok)
                *ok = r;

            basic_object<charT> root;
                for(auto& i : ast.children)
                boost::apply_visitor(vdf_praser_ast_visitor<charT>(root), i);
            root.name = ast.name;
            return root;
        }

        /** \brief Loads a stream (e.g. filestream) into the memory and parses the vdf formatted data.
        */
        template<typename iStreamT, typename charT = typename iStreamT::char_type>
        basic_object<charT> read(iStreamT& inStream, bool *ok = 0)
        {
            // cache the file
            std::basic_string<charT> str;
            inStream.seekg(0, std::ios::end);
            str.resize(inStream.tellg());
            if (str.empty())
                return basic_object<charT>();

            inStream.seekg(0, std::ios::beg);
            inStream.read(&str[0], str.size());
            inStream.close();

            // parse it
            return read(str.begin(), str.end(), ok);
        }

    } // end namespace vdf
} // end namespace tyti
#ifndef TYTI_NO_L_UNDEF
#undef TYTI_L
#endif

BOOST_FUSION_ADAPT_STRUCT(
    tyti::vdf::detail::parser_ast<char>,
    (std::string, name)
    (std::vector< tyti::vdf::detail::variant<char>::parser_node >, children)
)

BOOST_FUSION_ADAPT_STRUCT(
    tyti::vdf::detail::parser_ast<wchar_t>,
    (std::wstring, name)
    (std::vector< tyti::vdf::detail::variant<wchar_t>::parser_node >, children)
)


#endif //__TYTI_STEAM_VDF_PARSER_H__
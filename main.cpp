#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <algorithm>
#include <variant>

// 
// Outputs std::variant to stream
// 
template<class T>
struct streamer {
    const T& val;
};
template<class T> streamer(T) -> streamer<T>;

template<class T>
std::ostream& operator<<(std::ostream& os, streamer<T> s) {
    os << s.val;
    return os;
}

template<class... Ts>
std::ostream& operator<<(std::ostream& os, streamer<std::variant<Ts...>> sv) {
   std::visit([&os](const auto& v) { os << streamer{v}; }, sv.val);
   return os;
}

// 
// Represents atomic type
//
using Atom=std::variant<double, std::string>;

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

static std::vector<std::string> split(const std::string& s)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, ' '))
  {
    trim(token);
    if (token.length() > 0)
      tokens.push_back(token);
  }
  return tokens;
}

std::vector<std::string> tokenize(const std::string& chars)
{
  return split(std::regex_replace(std::regex_replace(chars, std::regex("\\("), " ( "), std::regex("\\)"), " ) "));
}

struct Expression {
  Expression () : isAtom{false} {}

  bool isAtom;
  std::vector<Expression> list;
  Atom                    atom;

  std::string toString() 
  {
    std::stringstream ss;

    if (isAtom)
    {
      ss << streamer{atom};
    }
    else 
    {
      ss << "[ ";
      for (auto e : list)
      {
        ss << e.toString();
        ss << ", ";
      }
      ss << " ]";
    }

    return ss.str();
  }
};

Expression readFromTokens(std::vector<std::string>::const_iterator& begin, std::vector<std::string>::const_iterator& end)
{
    // Read an expression from a sequence of tokens.
    if (begin == end)
        throw std::runtime_error("unexpected EOF");    

    auto token = *begin++;
    if (token == "(")
    {
        Expression expr;
        while (begin != end && *begin != ")") 
        {
          expr.list.push_back(readFromTokens(begin, end));
        }

        if (begin == end)
          throw std::runtime_error("missing )");

        ++begin; // pop off ')'

        return expr;
    }
    else if (token != ")")
    {
      Expression expr;
      expr.isAtom = true;

      bool isNumeric = true;
      try 
      {
        expr.atom = std::stod(token);
      }
      catch (std::invalid_argument& e) 
      {
        isNumeric = false;
      }

      if (!isNumeric)
        expr.atom = token;

      return expr;
    }

    throw std::runtime_error("unexpected )");
}

int main()
{
  try {
  std::string schemeCode = "(begin (define r 10) (* pi (* r r)))";

  std::vector<std::string> tokens = tokenize(schemeCode);

  for (auto& t : tokens)
    std::cout << t;

  std::cout << std::endl;

  auto begin = tokens.cbegin();
  auto end = tokens.cend();

  Expression expr = readFromTokens(begin, end);

  std::cout << expr.list.size() << std::endl;
  std::cout << expr.toString() << std::endl;


  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

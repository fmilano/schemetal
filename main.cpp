#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <algorithm>


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
  std::vector<Expression*> list;
  std::string              atom;

  std::string toString() 
  {
    std::stringstream ss;

    if (isAtom)
    {
      ss << atom;
    }
    else 
    {
      ss << "[ ";
      for (auto e : list)
      {
        ss << e->toString();
        ss << ", ";
      }
      ss << " ]";
    }

    return ss.str();
  }
};

Expression* readFromTokens(std::vector<std::string>::const_iterator& begin, std::vector<std::string>::const_iterator& end)
{
    // Read an expression from a sequence of tokens.
    if (begin == end)
        throw std::runtime_error("unexpected EOF");

    Expression* expr = new Expression;

    auto token = *begin;
    if (token == "(")
    {

        while (++begin != end && *begin != ")") {
            expr->list.push_back(readFromTokens(begin, end));
        }

        if (begin == end)
          throw std::runtime_error("missing )");

        ++begin; // pop off ')'
    }
    else if (token == ")")
        throw std::runtime_error("unexpected )");
    else {
      expr->isAtom = true;
      expr->atom = token;
    }

    return expr;
}

int main()
{
  std::string schemeCode = "(begin (define r 10) (* pi (* r r)))";

  std::vector<std::string> tokens = tokenize(schemeCode);

  auto begin = tokens.cbegin();
  auto end = tokens.cend();

  Expression* expr = readFromTokens(begin, end);

  std::cout << expr->list.size() << std::endl;
  std::cout << expr->toString() << std::endl;

  //for (auto& s : tokenize(schemeCode))
  //  std::cout << s << std::endl;


  return 0;
}

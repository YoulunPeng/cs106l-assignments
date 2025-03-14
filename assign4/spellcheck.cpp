#include "spellcheck.h"

#include <algorithm>
#include <cctype>
#include <ios>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

template <typename Iterator, typename UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(const std::string &source) {
  /* TODO: Implement this method */
  Corpus result{};
  // Find all of the spaces in the source
  auto space_iters = find_all(source.begin(), source.end(), isspace);

  // Separate tokens between spaces
  auto get_token = [&source](const auto &it1, const auto &it2) {
    return Token(source, it1, it2);
  };
  auto begin_spaces{space_iters}, end_spaces{space_iters};
  end_spaces.erase(end_spaces.begin());
  begin_spaces.pop_back();
  std::transform(begin_spaces.begin(), begin_spaces.end(), end_spaces.begin(),
                 std::inserter(result, result.end()), get_token);

  // Remove empty
  std::erase_if(result,
                [](const Token &token) { return token.content.empty(); });
  return result;
}

std::set<Mispelling> spellcheck(const Corpus &source,
                                const Dictionary &dictionary) {
  /* TODO: Implement this method */
  auto view =
      source | std::ranges::views::filter([&dictionary](const Token &token) {
        return !dictionary.contains(token.content);
      }) |
      std::ranges::views::transform([&dictionary](Token token) {
        auto view = dictionary | std::ranges::views::filter([token](auto item) {
                      return levenshtein(token.content, item) == 1;
                    });
        std::set<std::string> suggestions(view.begin(), view.end());
        return Mispelling{token, suggestions};
      }) |
      std::ranges::views::filter(
          [](auto misspell) { return (!misspell.suggestions.empty()); });
  std::set<Mispelling> result(view.begin(), view.end());
  return result;
};

/* Helper methods */

#include "utils.cpp"

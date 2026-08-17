#include <gtest/gtest.h>

#include <string_view>

#include <argpp/argparser.hpp>

using namespace argparser;

namespace {

class Mock
{
public:
  Mock() : m_counter() {}
  void call(std::string const& name) { m_counter[name] += 1; }
  int times_called(std::string const& name) { return m_counter[name]; }

private:
  std::map<std::string, int> m_counter;
};

void run_test(std::span<char const* const> argv, Mock& mock)
{
  OptionGroup opts;
  opts.option('h', "help", "Display help").then([&]{ mock.call("help"); });
  opts.positional(Argument<std::string>("POS1")).then([&](std::string const& /* unused */){ mock.call("pos1"); });
  opts.positional(Argument<std::string>("POS2")).then([&](std::string const& /* unused */){ mock.call("pos2"); });
  opts.rest(Argument<std::string>("REST")).then([&](std::string const& /* unused */){ mock.call("rest"); });

  Parser(opts).parse_args(argv);
}

} // namespace

TEST(ArgParserTest, option__short)
{
  Mock mock;
  run_test({{"example", "-h"}}, mock);
  EXPECT_EQ(mock.times_called("help"), 1);
}

TEST(ArgParserTest, option__long)
{
  Mock mock;
  run_test({{"example", "--help"}}, mock);
  EXPECT_EQ(mock.times_called("help"), 1);
}

TEST(ArgParserTest, rest)
{
  Mock mock;
  run_test({{"example", "pos1", "pos2", "rest1", "rest2", "rest3"}}, mock);
  EXPECT_EQ(mock.times_called("rest"), 3);
}

TEST(ArgParserTest, positional)
{
  Mock mock;
  run_test({{"example", "pos1", "pos2"}}, mock);
  EXPECT_EQ(mock.times_called("pos1"), 1);
  EXPECT_EQ(mock.times_called("pos2"), 1);
}

/* EOF */

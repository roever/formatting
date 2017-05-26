#include "format.hpp"

#include <string>
#include <cstdio>

template <int E = '%', typename... Ts>
void test(const char * result, const char * format, Ts ... a)
{
  std::string s = formatting::format<E>(format, a ...);

  if (s != result)
    printf("fail: |%s| <--> |%s|\n", s.c_str(), result);
}

int main()
{
  // normal tests
  test("test 100 muh 1c6 blabn",
       "test %0% %1% %2% blabn", 100, "muh", formatting::hex(454) );

  test("test 100",
       "test %0%", 100);

  // out of order
  test("test  muh 100 1c6 blabn",
       "test  %1% %0% %2% blabn", 100, "muh", formatting::hex(454) );

  // skip one
  test("test  muh 12 blabn",
       "test  %1% %12% blabn", 100, "muh", formatting::hex(454), 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 );

  // too many formats
  test("test  100 muh 1c6 blabn",
       "test  %0% %1% %2% blabn", 100, "muh", formatting::hex(454), 304, "ttt" );

  // too little formats and escape
  test("test  100 %0% %1% blabn % ",
       "test  %0% %1% %2% blabn %% ", 100 );

  // strange formats
  test("100 200 300",
       "%0% %1% %2%", 100, 200, 300 );

  // fault formats ... these are just a "must not crash"
  formatting::print<'%'>("%0% %1% %2\n", 100, 200, 300 );
  formatting::print<'%'>("0% %1% %2\n", 100, 200, 300 );

  // double use
  test("test  muh 100 100 1c6 blabn",
       "test  %1% %0% %0% %2% blabn", 100, "muh", formatting::hex(454) );
  test("test  100 100 100 1c6 blabn",
       "test  %s0% %0% %0% %2% blabn", 100, "muh", formatting::hex(454) );

  // other escape
  test<'|'>("test pipe  100 100 100 blabn",
            "test pipe  |s0| |0| |0| blabn", 100, "muh", formatting::hex(454) );

  // escaped escape
  test("test pipe  % 100 100 100 blabn",
       "test pipe  %% %s0% %0% %0% blabn", 100, "muh", formatting::hex(454) );
}

#include "format.hpp"

#include <string>
#include <cstdio>

template <int E = '%', typename... Ts>
void test(const char * result, const char * format, Ts ... a)
{
  std::string s = format::format<E>(format, a ...);

  if (s != result)
    printf("fail: |%s| <--> |%s|\n", s.c_str(), result);
}

int main()
{
  // normal tests
  test<'%'>("test 100 muh 1c6 blabn",
            "test %0% %1% %2% blabn", 100, "muh", format::hex(454) );

  test<'%'>("test 100",
            "test %0%", 100);

  // out of order
  test<'%'>("test  muh 100 1c6 blabn",
            "test  %1% %0% %2% blabn", 100, "muh", format::hex(454) );

  // skip one
  test<'%'>("test  muh 12 blabn",
            "test  %1% %12% blabn", 100, "muh", format::hex(454), 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 );

  // too many formats
  test<'%'>("test  100 muh 1c6 blabn",
            "test  %0% %1% %2% blabn", 100, "muh", format::hex(454), 304, "ttt" );

  // too litte formats
  test<'%'>("test  100   blabn",
            "test  %0% %1% %2% blabn", 100 );

  // strange formats
  test<'%'>("100 200 300",
            "%0% %1% %2%", 100, 200, 300 );

  // fault formats ... these are just a "must not crash"
  format::print<'%'>("%0% %1% %2\n", 100, 200, 300 );
  format::print<'%'>("0% %1% %2\n", 100, 200, 300 );

  // double use
  test<'%'>("test  muh 100 100 1c6 blabn",
            "test  %1% %0% %0% %2% blabn", 100, "muh", format::hex(454) );
  test<'%'>("test  100 100 100 1c6 blabn",
            "test  %s0% %0% %0% %2% blabn", 100, "muh", format::hex(454) );

  // other escape
  test<'|'>("test pipe  100 100 100 blabn",
            "test pipe  |s0| |0| |0| blabn", 100, "muh", format::hex(454) );

//  for (int i = 0; i < 10000000; i++)
  {


  format::print<'%'>("test  %0% %1% %2% blabn \n", 100, "muh", format::hex(454) );
  format::print<'%'>("test %0%\n", 100);

//  format::print("test %%  %0% %1% %2% blabn \n", 100, "muh" );

  //std::cout << "test  " << 100 << "muh" << std::hex << 454 << std::dec << "\n";
  //std::cout << "test " << 100 << "\n";

  //printf("test %i\n", 100);
  //printf("test  %i %s %x blabn\n", 100, "muh", 454);

  }
}

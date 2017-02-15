#include "format.hpp"

// a simple example class to show how to create additional output modules for those
struct Example
{
  double x, y, z;
};

void out(std::ostream & s, const char * p, const Example & v) {
  if (*p == 'i')
    s << "[" << v.x << "; " << v.y << "; " << v.z <<"]";
  else
    s << "{" << v.x << ", " << v.y << ", " << v.z <<"}";
}

using namespace formatting;

int main()
{
  // simple example, the placeholder %0% stands for the first argument that is beeing included, they
  // start counting with zero, the next argument will be placed, where the %1% is within the string
  print("format integer %0%\n", 100);

  // the type doesn't matter, just add it
  print("format with multiple types %0% %1% %2%\n", "test text", 23.4, 372834743L);

  // the order of the placeholders can be arbitraty and the arguments will be added accordingly
  // be aware though that this costs you quite a bit of performance
  print("format with different order %1% %0%\n", "argument-1", "argument-2");

  // you don't  need to include all placeholders, just the ones you need, again this will
  // cost you a bit of performance
  print("include subset of argument %0% %12% %3%\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);

  // if you don't provide values to all placeholders, they will not be included
  print("don't provide all values  %0% %1% %2%\n", 100, "muh");
  print("don't provide all values  %0% %2% %1%\n", 100, "muh");

  // if you provide more values than placeholders the additional values will be ignored
  print("more values %0%\n", 100, "muh", 3.1415);

  // you can use one value several times, but you need to tell that in the placeholder
  // by putting an 's' for 'store' in front of the index
  // this again costs performance
  print("multiple uses %s0% %1% %0%\n", 100, "meau");

  // if you don't it might or might not work, depending on other properties
  // of the format string, don't rely on this
  print("multiple uses with no store and not working %0% %1% %0%\n", 100, "meau");
  print("multiple uses with no store but working %1% %0% %0%\n", 100, "meau");

  // the library only provides some basic type support: all arithmetic types, c and c++ strings
  print("all int types, even char are treated as numbers %0% %1% %2% %3%\n", (char)8, (short)5, (int)100, (long)1000);
  print("%0% and %1% strings are supported\n", "c", std::string("c++"));
  print("double and float %0%, %1%\n", (double)3.1415, (float)2.7183);

  // for the integers you can request hex output.. this is done using a separate
  // type.. so the function does in effect nothing
  print("test %0% in hex is 0x%1%\n", 200, hex(200));

  // you can output your own types by providing an "out" function for them the
  // out function for the Example class is given above. the prototype needs to
  // be like that above, 1st argument is the stream to output to, 2nd is explained
  // below, 2rd is a reference (or copy, when cheap) to the value to output
  print("my own type %0%\n", Example{1.2, 3.6, 7.9});

  // the placeholder starts with % followed by an optional s for store followed by digits
  // for the index of the argument to include in its place. The number is finished after
  // the first non digit character... the placeholder goes on until the next percent
  // you can add additional characters and use those in your output routine
  // the 2nd argument of the out function will point just behind the number, so either to 'i'
  // or to '%' in this case
  print("formatting arguments %0i% %1%\n", Example{1.2, 3.6, 7.9}, Example{1.2, 3.6, 7.9});

  // if you don't close a placeholder it will incompass the remainder of the string
  print("unclosed placeholder %0% %1% %2 a lot more text that will be missing\n", 100, 200, 300 );
  print("\n");

  // you can print the % symbol by using two in a row
  print("test percent %% and more text\n");

  // if you don't like the % symbol as escabe character you can use others.. and again
  // using two instances of your escape symbol represents it itself
  print<'|'>("format integer with pipe (||) symbol as escape |0|\n", 100);

  // besides the normal print function that always outputs to stdout, there is
  // a function where you can specify the output strem
  write(std::cerr, "Error message");

  // you can even specify a string and the function will append to that string
  std::string message = "Error message: ";
  write(message, "too much information! (%0% lines)", 1023);

  // finally there is a function that will return a string
  message = format("%0% number", 42);



  // that is all there is.. if you need more... well .. do it yourself :)
  // the basic case where you use all placeholders only once and in order is
  // "only" about half as fast as a printf. If you use additional features you
  // loose much more speed. But it is still faster than boost::format... which is
  // supposed to do something similar... although I guess with more features
  //
  // there is no limitation to ascii, except that the escape character needs to be encodable
  // as a single byte character, so utf-8 can be used
  //
  // you might ask, why there is no width adjustment (e.g %05i in the printf placeholder)? Because
  // such a width operator is dependent on encoding. It needs to behave differently depending
  // on whether you use utf-8 or an ASCII variation...
}

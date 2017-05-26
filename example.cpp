#include "format.hpp"

// a simple example class to show how to create additional output modules for those
struct Example
{
  double x, y, z;
};

template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> p, const Example & v) {
  if (p[0] == 'i')
    s << "[" << v.x << "; " << v.y << "; " << v.z <<"]";
  else
    s << "{" << v.x << ", " << v.y << ", " << v.z <<"}";
}

using namespace formatting;

int main()
{
  // simple example, the placeholder %0% stands for the first argument that is being included, they
  // start counting with zero, the next argument will be placed, where the %1% is within the string
  print("format integer %0%\n", 100);

  // the type doesn't matter, just add it to string and argument list
  print("format with multiple types %0% %1% %2% %3% %4%\n", "test text", 23.4, 372834743L, std::string("string"), std::string_view("view"));

  // the order of the placeholders can be arbitrary and the arguments will be added accordingly
  // be aware though that this costs you quite a bit of performance
  print("format with different order %1% %0%\n", "argument-1", "argument-2");

  // you don't need to include all placeholders, just the ones you need, again this will
  // cost you a bit of performance
  print("include subset of argument %0% %12% %3%\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);

  // if you don't provide values to all placeholders, they will be empty and not output
  print("don't provide all values  %0% %1% %2%\n", 100, "muh");
  print("don't provide all values  %0% %2% %1%\n", 100, "muh");

  // if you provide more values than placeholders the additional values will be ignored
  print("more values %0%\n", 100, "muh", 3.1415);

  // you can use one value several times, but you need to tell that in the first placeholder
  // for the value by putting an 's' for 'store' in front of the index
  // again, this again costs performance
  print("multiple uses %s0% %1% %0%\n", 100, "meau");

  // if you don't include the s it might or might not work, depending on other properties
  // of the format string
  print("multiple uses with no store and not working %0% %1% %0%\n", 100, "meau");
  print("multiple uses with no store but working %1% %0% %0%\n", 100, "meau");

  // the library only provides some basic type support: all arithmetic types, c and c++ strings, bool and nullptr_t
  print("all int types, even char are treated as numbers %0% %1% %2% %3%\n", (char)8, (short)5, (int)100, (long)1000);
  print("%0% and %1% strings are supported\n", "c", std::string("c++"));
  print("double and float %0%, %1%\n", (double)3.1415, (float)2.7183);
  print("bool %0% and nullptr %1%\n", true, nullptr);

  // for the integers you can request hex output.. this is done using the "hex"
  // function. This function will only convert the type to another type but does
  // nothing computationally
  print("test %0% in hex is 0x%1%\n", 200, hex(200));

  // you can output your own types by providing an "out" function for them. the
  // out function for the Example class is given above. the prototype needs to
  // be like that above, 1st argument is the stream to output to, 2nd is explained
  // below, 3rd is a reference to (or copy of, when cheap) the value to output
  print("my own type %0%\n", Example{1.2, 3.6, 7.9});

  // the placeholder starts with % followed by an optional s for store followed by digits
  // for the index of the argument to include in its place. The number is finished after
  // the first non digit character. the placeholder goes on until the next percent
  // you can add additional characters and use those in your output routine.
  // the 2nd argument of the out function will point just behind the number, so either to 'i'
  // or to '%' in the example below. with this you can influence the output
  print("formatting arguments %0i% %1%\n", Example{1.2, 3.6, 7.9}, Example{1.2, 3.6, 7.9});

  // if you don't close a placeholder it will encompass the remainder of the string
  print("unclosed placeholder %0% %1% %2 a lot more text that will be missing\n", 100, 200, 300 );
  print("\n");

  // you can print the % symbol by using two in a row
  print("test percent %% and more text\n");

  // if you don't like the % symbol as escape character you can use others. again:
  // using two instances of your escape symbol represents it itself
  // the only limit is that you have to use a symbol that is only one symbol
  // even in multi-byte encodings
  print<'|'>("format integer with pipe (||) symbol as escape |0|\n", 100);

  // besides the normal print function that always outputs to stdout, there is
  // a function where you can specify the output stream
  write(std::cerr, "Error message\n");

  // to this function you can also specify a string and the function will append to that string
  std::string message = "Error message: ";
  write(message, "too much information! (%0% lines)", 1023);

  // finally there is a function that will return a string
  message = format("%0% number", 42);

  // you can also use other string types, besides the byte based ones
  // print also automatically works with wide-characters
  print(L"A wide Character format %0%\n", 123);

  // format and write also work with all string types
  std::u32string u32string = format(U"32 bit wide encoded %0%", U"string");

  // you can also use strings and string_views as arguments with the format string
  std::string fmt = "%0% number\n";
  print(fmt, 43);
  message = format(fmt, 44);  print(message);

  print((std::string_view)fmt, 45);
  message = format((std::string_view)fmt, 46); print(message);

  // all function except for the print function will output placeholders
  // in such a way that the output string is again a format string
  // with valid placeholders so that you can replace the placeholders one after
  // the other in separate function calls
  fmt = "string %s1% with %0% %1% multiple %2% placeholders\n";
  fmt = format(fmt, "000");
  fmt = format(fmt, "111");
  print(fmt, "222");

  // alignment out output is provided using the function align
  // in its most basic form you can specify a width, integral types are automatically
  // padded with 0s all others with spaces, alignment is always right
  print("Test padded to 11 characters, |%0%|, |%1%|\n", align(100, 11), align("text", 11));

  // if you need more provide all arguments
  print(L"Test padded to 11 characters, |%0%|, |%1%|\n", align(100, 11, '.', 'c'), align("text", 11, '-', 'l'));

  // be aware though that this is quite limited:
  // - no multi byte utf8 sequences can be used for padding
  // - string length is measured in basic units, e.g. bytes for utf8
  // but it might be good enough to align numbers, positive numbers that is :)

  // one final remark: you must make sure, that all types fit together...
  //
  // for example you can not format 32-bit character strings into 8-bit char strings
  // u32string = format("32 bit wide encoded %0%", U"string");
  //
  // you can also not output 32-bit strings to std::cout
  // write(std::cout, U"32 bit wide encoded %0%", U"string");
  //
  // other combinations also might not work....

  // that is all there is.. if you need more... well .. do it yourself :)
  // the basic case where you use all placeholders only once and in ascending order is
  // about half as fast as a printf. If you use additional features you
  // loose much more speed. But it is still faster than boost::format... which is
  // supposed to do something similar... although, I guess, with more features
  //
  // with the byte strings there is no limitation to ascii, except that the escape
  // character needs to be encodable as a single byte character, so utf-8 can be used with that

  return 0;
}


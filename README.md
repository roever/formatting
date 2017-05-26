# formatting

A simple library to format values into strings with placeholders, a bit like printf or boost::format, but with greater emphasis 
on internationalisation and not the goal of actually formatting text.

- placeholders are numbered
- placeholders require no type information, this is all taken directly from the values to add
- nearly as fast as printf (half as fast for simple strings) boost::format is only a tenth as fast as printf
- I implemented only a basic form of alignment as proper formatting is quite complicated


# documentation

Have a look at the example.cpp file for a detailed example with a lot of
explanations. As there is not much more functionality there is no more 
documentation.

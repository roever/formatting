# formatting

A simple library to format values into strings with placeholders, a bit like printf or boost::format, but with greater emphasis 
on internationalisation and not the goal of actually formatting text.

- placeholders are numbered
- placeholders require no type information, this is all taken directly from the values to add
- nearly as fast as printf (half as fast for simple strings) boost::format is only a tenth as fast as printf
- I intentionally left out text width formatting as this requires encoding specific knowledge


#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <tuple>
#include <string>

namespace formatting {

namespace internal {

template <typename T> struct hex { hex(T v) : val(v) {} T val; };

// a list of trivial output functions... all further ones you have to
// provide on your own
template <typename C> void out(std::basic_ostream<C> & s, const C * p, double v) { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, float v) { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, long double v) { s << v; }

template <typename C> void out(std::basic_ostream<C> & s, const C * p, uint8_t v)  { s << (uint32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, uint16_t v) { s << (uint32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, uint32_t v) { s << (uint32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, uint64_t v) { s << (uint64_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, int8_t v)   { s << ( int32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, int16_t v)  { s << ( int32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, int32_t v)  { s << ( int32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, int64_t v)  { s << ( int64_t)v; }

template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<uint8_t> v)  { s << std::hex << (uint32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<uint16_t> v) { s << std::hex << (uint32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<uint32_t> v) { s << std::hex << (uint32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<uint64_t> v) { s << std::hex << (uint64_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<int8_t> v)   { s << std::hex << ( int32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<int16_t> v)  { s << std::hex << ( int32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<int32_t> v)  { s << std::hex << ( int32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, hex<int64_t> v)  { s << std::hex << ( int64_t)v.val << std::dec; }

template <typename C> void out(std::basic_ostream<C> & s, const C * p, const char * v)   { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, std::nullptr_t v) { s << "NULL"; }
template <typename C> void out(std::basic_ostream<C> & s, const C * p, bool v)           { if (v) s << "true"; else s << "false"; }

template <typename C> void out(std::basic_ostream<C> & s, const C * p, const std::string & v) { s << v; }

// E is the escape character, C the type to use for characters
template <int E, typename C>
class Format {
    std::basic_ostream<C> & stream;     // the stream to output to
    int placeholderValueIndex = 0;      // the current placeholder value index that is fed in via operator%
    int placeholderFormatIndex = 0;     // the index of the placeholder index in the format string we are currently on
    bool storePlaceholderValue = false; // whether that placeholder has requested storage
    std::vector<int> hardPlaceholders;  // indices of the stored placeholders
    std::vector<std::basic_ostringstream<C>> hardPlaceholderValues; // stringstreams with the values of the current placeholders
    const C * format;                   // the format string

    // like the c++ container ... find E inside of the format string
    // if E is not inside format, advance to the null terminator
    void skipToE()
    {
      while (*format)
      {
        if (*format == E) return;
        format++;
      }
    }

    // do as much output of the format string as possible
    // it assumes that format points into normal text and not
    // into a placeholder. It outputs the normal text up to
    // the next placeholder
    void advance()
    {
      const C * start = format;

      while (true)
      {
        // find nect possible placeholder start
        skipToE();

        // output everything up to there
        stream.write(start, format-start);

        // check where we are
        if (format[0] != 0 && format[1] == E)
        {
          // it was only an escaped escape character
          // output a single escape and skip over it
          // and continue outputting
          start = format+1;
          format += 2;
        }
        else
        {
          // end of string has been reached or
          // normal placeholder start has been found
          return;
        }
      }
    }

    std::pair<int, bool> getPlaceholderIndex()
    {
      int ret = 0;
      format++;
      bool store = *format == 's';
      if (store) format++;

      while (*format >= '0' && *format <= '9')
      {
        ret = ret * 10 + *format - '0';
        format++;
      }

      return std::make_pair(ret, store);
    }

    // output all of the remaining format string that we can
    // this function assumes that format points somewhere behind
    // the start excape character of the placeholder... maybe even
    // directly to the end escape of the placeholder
    void outputMore()
    {
      while (true)
      {
        // skip current placeholder
        skipToE();
        if (*format == 0) return;
        // skip over the escape character of the end placeholder
        format++;
        if (*format == 0) return;
        // output until next placeholder or end
        advance();
        if (*format == 0) return;

        // get the placeholder information
        std::tie(placeholderFormatIndex, storePlaceholderValue) = getPlaceholderIndex();

        // if the index of the new placeholder is not reached yet, exit
        if (placeholderFormatIndex > placeholderValueIndex) return;

        // find the placeholder in the stored list and output it
        // if not found... well output nothing
        if (hardPlaceholders.size() > placeholderFormatIndex && hardPlaceholders[placeholderFormatIndex])
        {
          // TODO this unnecessarily makes a copy... yuck there
          // must be a way around that
          stream << hardPlaceholderValues[hardPlaceholders[placeholderFormatIndex]-1].rdbuf()->str();
        }
      }
    }

  public:

    Format(std::basic_ostream<C> & s, const C * f) : stream(s), format(f)
    {
      // check for nullptr, if we get one we output nothing
      if (f == nullptr)
      {
        // well, this is ugly, is there no better way ???? TODO
        static std::basic_string<C> str;
        format = str.c_str();
      }
      else
      {
        // output up to the first placeholder and prepare
        // for that placeholder... except if there are no placeholder,
        // then we have already output all there is
        advance();
        if (*format)
        {
          std::tie(placeholderFormatIndex, storePlaceholderValue) = getPlaceholderIndex();
        }
      }
    }

    ~Format()
    {
      // output whatever is left, including placeholders
      if (*format)
      {
        // by setting the placeholder index to this big value
        // we make outputMore output all that is left
        placeholderValueIndex = std::numeric_limits<int>::max();
        outputMore();
      }
    }

    template <typename V>
    Format & operator%(const V & v)
    {
      // if we get additional arguments after all placeholders
      // have been handled, do nothing
      if (*format)
      {
        // check, if our current placeholder index is the one of
        // the current argument or if the placeholder requests storing
        if (placeholderValueIndex != placeholderFormatIndex || storePlaceholderValue)
        {
          // storing required, output into a string stream
          hardPlaceholderValues.emplace_back();
          out(hardPlaceholderValues.back(), format, v);
          hardPlaceholders.resize(placeholderValueIndex+1);
          hardPlaceholders[placeholderValueIndex] = hardPlaceholderValues.size();
        }

        if (placeholderValueIndex == placeholderFormatIndex)
        {
          out(stream, format, v);
          outputMore();
        }

        placeholderValueIndex++;
      }
      return *this;
    }
};

}

/// TODO use string_view once the compilers support it

/// write something to an output stream
template <int E = '%', typename C, typename... Ts>
void write(std::basic_ostream<C> & str, const C * format, Ts ... a)
{
  (internal::Format<E, C>(str, format) % ... % a);
}

/// append something to a string
template <int E = '%', typename C, typename... Ts>
void write(std::basic_string<C> & out, const C * format, Ts ... a)
{
  std::basic_ostringstream<C> str;
  write<E>(str, format, a...);
  out += str.str();
}

/// print to std::cout or std::wcout
template <int E = '%', typename... Ts> void print(const char    * format, Ts ... a) { write<E>(std::cout,  format, a...); }
template <int E = '%', typename... Ts> void print(const wchar_t * format, Ts ... a) { write<E>(std::wcout, format, a...); }

/// format and return the result as a string, the type of the result string depends
/// on the type of the input string
template <int E = '%', typename C, typename... Ts>
std::basic_string<C> format(const C * format, Ts ... a)
{
  std::basic_ostringstream<C> str;
  write<E>(str, format, a...);
  return str.str();
}

template <typename I> internal::hex<I> hex(I i) { return internal::hex<I>{i}; }

}


#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <tuple>
#include <string_view>
#include <string>
#include <type_traits>

namespace formatting {

namespace internal {

template <typename T> struct hex { hex(T v) : val(v) {} T val; };
template <typename T> struct align { align(T v, size_t w, int p, char a) : val(v), width(w), padding(p), algn(a) {} T val; size_t width; int padding; char algn; };

// a list of trivial output functions... all further ones you have to
// provide on your own
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, double v) { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, float v) { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, long double v) { s << v; }

template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, uint8_t v)  { s << (uint32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, uint16_t v) { s << (uint32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, uint32_t v) { s << (uint32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, uint64_t v) { s << (uint64_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, int8_t v)   { s << ( int32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, int16_t v)  { s << ( int32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, int32_t v)  { s << ( int32_t)v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, int64_t v)  { s << ( int64_t)v; }

template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<uint8_t> v)  { s << std::hex << (uint32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<uint16_t> v) { s << std::hex << (uint32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<uint32_t> v) { s << std::hex << (uint32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<uint64_t> v) { s << std::hex << (uint64_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<int8_t> v)   { s << std::hex << ( int32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<int16_t> v)  { s << std::hex << ( int32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<int32_t> v)  { s << std::hex << ( int32_t)v.val << std::dec; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, hex<int64_t> v)  { s << std::hex << ( int64_t)v.val << std::dec; }

template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, const char * v)   { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, std::nullptr_t /*v*/) { s << "NULL"; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, bool v)           { if (v) s << "true"; else s << "false"; }

template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, std::basic_string_view<C> v) { s << v; }
template <typename C> void out(std::basic_ostream<C> & s, std::basic_string_view<C> /*p*/, const std::basic_string<C> & v) { s << v; }

template <typename C, typename A> void out(std::basic_ostream<C> & s, std::basic_string_view<C> p, const align<A> & v)
{
  std::basic_ostringstream<C> tmp;
  out(tmp, p, v.val);

  std::basic_string<C> st = tmp.str();

  if (st.size() < v.width)
  {
    switch (v.algn)
    {
      case 'c':
        {
          size_t c = v.width - st.size();
          s << std::basic_string<C>(c/2, v.padding);
          c = c-c/2;
          s << st;
          s << std::basic_string<C>(c, v.padding);
        }
        break;

      case 'l':
        s << st;
        s << std::basic_string<C>(v.width - st.size(), v.padding);
        break;

      default:
        s << std::basic_string<C>(v.width - st.size(), v.padding);
        s << st;
        break;
    }
  }
  else
  {
    s << st;
  }
}

// E is the escape character, C the type to use for characters
template <int E, typename C>
class Format {
    std::basic_ostream<C> & stream;     // the stream to output to
    size_t placeholderValueIndex = 0;      // the current placeholder value index that is fed in via operator%
    size_t placeholderFormatIndex = 0;     // the index of the placeholder index in the format string we are currently on
    bool storePlaceholderValue = false; // whether that placeholder has requested storage
    std::vector<int> hardPlaceholders;  // indices of the stored placeholders
    std::vector<std::basic_string<C>> hardPlaceholderValues; // stringstreams with the values of the current placeholders
    std::basic_string_view<C> format;   // the format string
    size_t position = 0;

    // like the c++ container ... find E inside of the format string
    // if E is not inside format, advance to the null terminator
    void skipToE()
    {
      while (position < format.size())
      {
        if (format[position] == E) return;
        position++;
      }
    }

    // do as much output of the format string as possible
    // it assumes that format points into normal text and not
    // into a placeholder. It outputs the normal text up to
    // the next placeholder
    void advance()
    {
      auto start = position;

      while (true)
      {
        // find nect possible placeholder start
        skipToE();

        // output everything up to there
        stream.write(format.data()+start, position-start);

        // check where we are
        if (position+1 < format.size() && format[position+1] == E)
        {
          // it was only an escaped escape character
          // output a single escape and skip over it
          // and continue outputting
          start = position+1;
          position += 2;
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
      position++;
      bool store = format[position] == 's';
      if (store) position++;

      while (format[position] >= '0' && format[position] <= '9')
      {
        ret = ret * 10 + format[position] - '0';
        position++;
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
        // skip over the escape character of the end placeholder
        skipToE();
        position++;
        if (position >= format.size()) return;

        // output until next placeholder or end
        advance();
        if (position >= format.size()) return;

        // get the placeholder information
        std::tie(placeholderFormatIndex, storePlaceholderValue) = getPlaceholderIndex();

        // if the index of the new placeholder is not reached yet, exit
        if (placeholderFormatIndex > placeholderValueIndex) return;

        // find the placeholder in the stored list and output it
        // if not found... well output nothing
        if (hardPlaceholders.size() > placeholderFormatIndex && hardPlaceholders[placeholderFormatIndex])
        {
          stream << hardPlaceholderValues[hardPlaceholders[placeholderFormatIndex]-1];
        }
      }
    }

  public:

    Format(std::basic_ostream<C> & s, std::basic_string_view<C> f) : stream(s), format(f)
    {
      // output up to the first placeholder and prepare
      // for that placeholder... except if there are no placeholder,
      // then we have already output all there is
      advance();
      if (position < format.size())
      {
        std::tie(placeholderFormatIndex, storePlaceholderValue) = getPlaceholderIndex();
      }
    }

    ~Format()
    {
      // output whatever is left, including placeholders
      if (position < format.size())
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
      if (position < format.size())
      {
        // check, if our current placeholder index is the one of
        // the current argument or if the placeholder requests storing
        if (placeholderValueIndex != placeholderFormatIndex || storePlaceholderValue)
        {
          // storing required, output into a string stream
          std::basic_ostringstream<C> tmp;
          out(tmp, format.substr(position), v);

          hardPlaceholderValues.emplace_back(tmp.str());
          hardPlaceholders.resize(placeholderValueIndex+1);
          hardPlaceholders[placeholderValueIndex] = hardPlaceholderValues.size();
        }

        if (placeholderValueIndex == placeholderFormatIndex)
        {
          out(stream, format.substr(position), v);
          outputMore();
        }

        placeholderValueIndex++;
      }
      return *this;
    }
};

}

/// write something to an output stream
template <int E = '%', typename C, typename... Ts>
void write(std::basic_ostream<C> & str, std::basic_string_view<std::remove_cv_t<C>> format, Ts ... a)
{
  (internal::Format<E, std::remove_cv_t<C>>(str, format) % ... % a);
}

/// append something to a string
template <int E = '%', typename C, typename... Ts>
void write(std::basic_string<C> & out, std::basic_string_view<std::remove_cv_t<C>> format, Ts ... a)
{
  std::basic_ostringstream<C> str;
  write<E>(str, format, a...);
  out += str.str();
}

/// print to std::cout or std::wcout
template <int E = '%', typename... Ts> void print(std::basic_string_view<char> format, Ts ... a) { write<E>(std::cout,  format, a...); }
template <int E = '%', typename... Ts> void print(std::basic_string_view<wchar_t> format, Ts ... a) { write<E>(std::wcout, format, a...); }

/// format and return the result as a string, the type of the result string depends
/// on the type of the input string
template <int E = '%', typename C, typename... Ts>
std::basic_string<C> format(std::basic_string_view<C> f, Ts ... a)
{
  std::basic_ostringstream<C> str;
  write<E>(str, f, a...);
  return str.str();
}

// format function overloads for some other types... need to do this because the above function
// can not deduce C
template <int E = '%', typename C, typename... Ts>
std::basic_string<C> format(const C * f, Ts ... a) { return format(std::basic_string_view<std::remove_cv_t<C>>(f), a...); }

template <int E = '%', typename C, typename... Ts>
std::basic_string<C> format(const std::basic_string<C> & f, Ts ... a) { return format((std::basic_string_view<C>)f, a...); }

template <typename I> internal::hex<I> hex(I i) { return internal::hex<I>{i}; }

template <typename I> internal::align<I> align(I i, size_t w) {
  if constexpr (std::is_integral_v<I>)
    return internal::align<I>{i, w, '0', 'r'};
  else
    return internal::align<I>{i, w, ' ', 'r'};
}

template <typename I> internal::align<I> align(I i, size_t w, int padding, char align) {
  return internal::align<I>{i, w, padding, align};
}

}

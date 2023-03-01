# cpp-pgm8

A small library for reading and writing 8-bit grayscale PGM image files in C++.

## Using

This is a source-based library, so copy [pgm8.cpp](/pgm8.cpp) and [pgm8.hpp](/pgm8.hpp) into your project and `#include "pgm8.hpp"` where you need it. That's it.

## Examples

Images can be read into and written from any contiguous `uint8_t` array:

```cpp
/* static array  */ uint8_t pixels[50];
/* dynamic array */ uint8_t *pixels = new uint8_t[50];
/* unique_ptr    */ std::unique_ptr<uint8_t []> pixels(new uint8_t[50]);
/* std::array    */ std::array<uint8_t, 50> pixels;
/* std::vector   */ std::vector<uint8_t> pixels(50);
// etc.
```

Example for writing a PGM file from a `std::vector`:

```cpp
{
  std::vector<uint8_t> const pixels { 1, 2, 3, 4 };

  std::ofstream file("image.pgm");

  pgm8::image_properties img_props;

  try {
    // setters do validation and throw if bad values are set:
    img_props.set_width(2); // std::runtime_error if width = 0
    img_props.set_height(2); // std::runtime_error if height = 0
    img_props.set_maxval(4); // std::runtime_error if maxval = 0
    img_props.set_format(pgm8::format::PLAIN); // std::runtime_error if invalid format
  } catch (std::runtime_error const &err) {
    std::cerr << "pgm8::image_properties failure - " << err.what() << '\n';
    std::exit(1);
  }

  try {
    pgm8::write(file, img_props, pixels.data());
    // std::runtime_error if one of
    //  - width
    //  - height
    //  - maxval
    //  - format
    // was not explicitly set in `img_props`
    // OR if `file` is closed or in bad state
    // OR if a write to `file` fails
  } catch (std::runtime_error const &err) {
    std::cerr << "ERROR: pgm8::write failed - " << err.what() << '\n';
    std::exit(1);
  }
}
```

Example for reading a PGM file into a `std::unique_ptr`:

```cpp
{
  std::ifstream file("image.pgm");

  // first, query the image dimensions to figure out
  // how much space to allocate for pixels:
  pgm8::image_properties img_props;
  try {
    img_props = pgm8::read_properties(file);
  } catch (std::runtime_error const &err) {
    std::cerr << "pgm8::read_properties failed - " << err.what() << '\n';
    std::exit(1);
  }

  // allocate buffer for pixels:
  std::unique_ptr<uint8_t []> pixels(new uint8_t[img_props.num_pixels()]);
  // (not bothering to handle an allocation failure)

  // read pixels from file into buffer:
  try {
    pgm8::read_pixels(file, img_props, pixels.get());
  } catch (std::runtime_error const &err) {
    std::cerr << "pgm8::read_pixels failed - " << err.what() << '\n';
    std::exit(1);
  }

  // query width, height, maxval:
  std::cout
    << "width = " << img_props.get_width() << '\n'
    << "height = " << img_props.get_height() << '\n'
    << "maxval = " << img_props.get_maxval() << '\n'
  ;
}
```

## File Format

| | element | size in bytes | format | value |
| - | - | - | - | - |
| 1 | magic number | 2 | ASCII decimal | `P2` for plain, `P5` for raw |
| 2 | newline | 1 | ASCII | `\n` |
| 3 | width | 1-5 | ASCII decimal | `1-65535` |
| 4 | whitespace | 1 | ASCII |  |
| 5 | height | 1-5 | ASCII decimal | `1-65535` |
| 6 | newline | 1 | ASCII | `\n` |
| 7 | maxval | 1-3 | ASCII decimal | `1-255` |
| 8 | newline | 1 | ASCII | `\n` |
| 9 | pixel data | --- | [see here](http://davis.lbl.gov/Manuals/NETPBM/doc/pgm.html) | --- |

\* Comments (lines starting with `#`) are not supported

## FAQ

Q: Why use a special `pgm8::image_properties` object with setters instead of just passing the width, height, maxval, and format directly to `pgm8::write`? - something like:

```cpp
namespace pgm8 {
  void write(
    std::ofstream &file,
    uint16_t width,
    uint16_t height,
    uint8_t maxval,
    format fmt,
    uint8_t const *pixels
  );
}
```

A: Because width and height are the same type and easily swappable, which is bugprone. Also, `pgm8::image_properties` does validation which catches mistakes such as using uninitialized values.

## License

This library is under the MIT license, use it however you please - commercially or not.

You do not need to give credit.

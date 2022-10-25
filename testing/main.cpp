#include <iostream>

#include "ntest.hpp"
#include "../pgm8.hpp"

void assert_image(
  pgm8::image const &expected, pgm8::image const &actual,
  std::source_location const loc = std::source_location::current())
{
  ntest::assert_uint16(expected.width, actual.width, loc);
  ntest::assert_uint16(expected.height, actual.height, loc);
  ntest::assert_uint8(expected.maxval, actual.maxval, loc);
  size_t const num_pixels = static_cast<size_t>(actual.width) * actual.height;
  ntest::assert_arr(expected.pixels, num_pixels, actual.pixels, num_pixels, loc);
}

void write_and_read_back_plain_test(
  char const *const name, pgm8::image const &img,
  std::source_location const loc = std::source_location::current())
{
  std::string const
    plain_pathname = std::string("./files/") + name + ".plain.pgm",
    raw_pathname = std::string("./files/") + name + ".raw.pgm";

  {
    std::ofstream file(plain_pathname);
    pgm8::write(file, img, pgm8::format::PLAIN);
  }
  {
    std::ifstream file(plain_pathname);
    pgm8::image const read_img = pgm8::read(file);
    assert_image(img, read_img, loc);
  }
}

void write_and_read_back_raw_test(
  char const *const name, pgm8::image const &img,
  std::source_location const loc = std::source_location::current())
{
  std::string const
    plain_pathname = std::string("./files/") + name + ".plain.pgm",
    raw_pathname = std::string("./files/") + name + ".raw.pgm";

  {
    std::ofstream file(raw_pathname, std::ios::binary);
    pgm8::write(file, img, pgm8::format::RAW);
  }
  {
    std::ifstream file(raw_pathname, std::ios::binary);
    pgm8::image const read_img = pgm8::read(file);
    assert_image(img, read_img, loc);
  }
}

int main()
{
  try
  {
    // horizontal and vertical line
    {
      uint8_t pixels[256];
      for (size_t i = 0; i <= UINT8_MAX; ++i)
        pixels[i] = static_cast<uint8_t>(i);

      pgm8::image const img_horizontal {
        256, // width
        1, // height
        UINT8_MAX, // maxval
        pixels
      };

      pgm8::image const img_vertical {
        1, // width
        256, // height
        UINT8_MAX, // maxval
        pixels
      };

      write_and_read_back_plain_test("horiz", img_horizontal);
      write_and_read_back_raw_test("horiz", img_horizontal);
      write_and_read_back_plain_test("vert", img_vertical);
      write_and_read_back_raw_test("vert", img_vertical);
    }

    // horizontal gradient
    {
      uint16_t const width = 6, height = 3;
      uint8_t pixels[width * height] {
        0, 1, 3, 6, 10, 15,
        0, 1, 3, 6, 10, 15,
        0, 1, 3, 6, 10, 15,
      };

      pgm8::image const expected {
        width,
        height,
        15, // maxval
        pixels
      };

      write_and_read_back_plain_test("horiz-grad", expected);
      write_and_read_back_raw_test("horiz-grad", expected);
    }

    // vertical gradient
    {
      uint16_t const width = 3, height = 6;
      uint8_t pixels[width * height] {
        0, 0, 0,
        1, 1, 1,
        3, 3, 3,
        6, 6, 6,
        10, 10, 10,
        15, 15, 15,
      };

      pgm8::image const img {
        width,
        height,
        15, // maxval
        pixels
      };

      write_and_read_back_plain_test("vert-grad", img);
      write_and_read_back_raw_test("vert-grad", img);
    }

    // diagonal gradient
    {
      uint16_t const width = 5, height = 5;
      uint8_t pixels[width * height] {
        0, 1, 2, 3, 4,
        1, 2, 3, 4, 5,
        2, 3, 4, 5, 6,
        3, 4, 5, 6, 7,
        4, 5, 6, 7, 8,
      };

      pgm8::image const img {
        width,
        height,
        8, // maxval
        pixels
      };

      write_and_read_back_plain_test("diag-grad", img);
      write_and_read_back_raw_test("diag-grad", img);
    }

    ntest::generate_report("pgm8");
  }
  catch (std::runtime_error const &err)
  {
    std::cerr << "fatal: " << err.what() << '\n';
    return 1;
  }
  catch (...)
  {
    std::cerr << "fatal: unknown error\n";
    return 2;
  }

  return 0;
}

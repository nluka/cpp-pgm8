#include <iostream>

#include "ntest.hpp"
#include "../pgm8.hpp"

struct readonly_image
{
  pgm8::image_properties const props;
  uint8_t const *pixels;
};

void assert_image(
  readonly_image const &expected,
  readonly_image const &actual,
  std::source_location const loc = std::source_location::current())
{
  ntest::assert_uint16(expected.props.get_width(), actual.props.get_width(), loc);
  ntest::assert_uint16(expected.props.get_height(), actual.props.get_height(), loc);
  ntest::assert_uint8(expected.props.get_maxval(), actual.props.get_maxval(), loc);
  size_t const num_pixels =
    static_cast<size_t>(actual.props.get_width()) *
    actual.props.get_height();
  ntest::assert_arr(expected.pixels, num_pixels, actual.pixels, num_pixels, loc);
}

void write_and_read_back_plain_test(
  char const *const name,
  readonly_image const &img,
  std::source_location const loc = std::source_location::current())
{
  std::string const
    plain_pathname = std::string("./files/") + name + ".plain.pgm",
    raw_pathname = std::string("./files/") + name + ".raw.pgm";

  {
    std::ofstream file(plain_pathname);
    pgm8::write(file, img.props, img.pixels);
  }
  {
    std::ifstream file(plain_pathname);
    auto const props_found = pgm8::read_properties(file);
    auto const num_pixels = props_found.num_pixels();
    std::unique_ptr<uint8_t []> pixels(new uint8_t[num_pixels]);
    pgm8::read_pixels(file, props_found, pixels.get());
    assert_image(img, {props_found, pixels.get()}, loc);
  }
}

void write_and_read_back_raw_test(
  char const *const name,
  readonly_image const &img,
  std::source_location const loc = std::source_location::current())
{
  std::string const
    plain_pathname = std::string("./files/") + name + ".plain.pgm",
    raw_pathname = std::string("./files/") + name + ".raw.pgm";

  {
    std::ofstream file(raw_pathname, std::ios::binary);
    pgm8::write(file, img.props, img.pixels);
  }
  {
    std::ifstream file(raw_pathname, std::ios::binary);
    auto const props_found = pgm8::read_properties(file);
    auto const num_pixels = props_found.num_pixels();
    std::unique_ptr<uint8_t []> pixels(new uint8_t[num_pixels]);
    pgm8::read_pixels(file, props_found, pixels.get());
    assert_image(img, {props_found, pixels.get()}, loc);
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

      // horizontal
      {
        pgm8::image_properties props;
        props.set_width(256);
        props.set_height(1);
        props.set_maxval(UINT8_MAX);

        props.set_format(pgm8::format::PLAIN);
        write_and_read_back_plain_test("horiz", {props, pixels});

        props.set_format(pgm8::format::RAW);
        write_and_read_back_raw_test("horiz", {props, pixels});
      }

      // vertical
      {
        pgm8::image_properties props;
        props.set_width(1);
        props.set_height(256);
        props.set_maxval(UINT8_MAX);

        props.set_format(pgm8::format::PLAIN);
        write_and_read_back_plain_test("vert", {props, pixels});

        props.set_format(pgm8::format::RAW);
        write_and_read_back_raw_test("vert", {props, pixels});
      }
    }

    // horizontal gradient
    {
      uint16_t const width = 6, height = 3;
      uint8_t pixels[width * height] {
        0, 1, 3, 6, 10, 15,
        0, 1, 3, 6, 10, 15,
        0, 1, 3, 6, 10, 15,
      };

      pgm8::image_properties props;
      props.set_width(width);
      props.set_height(height);
      props.set_maxval(15);

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("horiz-grad", {props, pixels});

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("horiz-grad", {props, pixels});
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

      pgm8::image_properties props;
      props.set_width(width);
      props.set_height(height);
      props.set_maxval(15);

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("vert-grad", {props, pixels});

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("vert-grad", {props, pixels});
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

      pgm8::image_properties props;
      props.set_width(width);
      props.set_height(height);
      props.set_maxval(8);

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("diag-grad", {props, pixels});

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("diag-grad", {props, pixels});
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

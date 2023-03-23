#include <iostream>

#include "ntest.hpp"
#include "../pgm8.hpp"

struct readonly_image
{
  pgm8::image_properties const props;
  std::vector<std::string> const &comments;
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
  ntest::assert_arr(expected.pixels, expected.props.num_pixels(), actual.pixels, actual.props.num_pixels(), loc);
}

void write_and_read_back_plain_test(
  std::string const &path_without_ext,
  readonly_image const &input_img,
  std::source_location const loc = std::source_location::current())
{
  std::string const full_path = path_without_ext + ".plain.pgm";
  {
    std::ofstream file(full_path);
    pgm8::write(file, input_img.props, input_img.comments, input_img.pixels);
  }
  {
    std::ifstream file(full_path);
    auto const props_found = pgm8::read_properties(file);
    auto const comments_found = pgm8::read_comments(file);
    auto const num_pixels_found = props_found.num_pixels();
    std::unique_ptr<uint8_t []> pixels_found(new uint8_t[num_pixels_found]);
    pgm8::read_pixels(file, props_found, pixels_found.get());
    assert_image(input_img, { props_found, comments_found, pixels_found.get() }, loc);
  }
}

void read_but_skip_comments_test(
  std::string const &path_without_ext,
  readonly_image const &input_img,
  size_t const expected_num_comments_skipped,
  std::source_location const loc = std::source_location::current())
{
  std::string full_path = path_without_ext;

  if (input_img.props.get_format() == pgm8::format::PLAIN)
    full_path += ".plain";
  else
    full_path += ".raw";

  full_path += ".pgm";

  {
    std::ifstream file(full_path);
    auto const props_found = pgm8::read_properties(file);
    size_t const num_comments_skipped = pgm8::skip_comments(file);
    auto const num_pixels_found = props_found.num_pixels();
    std::unique_ptr<uint8_t []> pixels_found(new uint8_t[num_pixels_found]);
    pgm8::read_pixels(file, props_found, pixels_found.get());

    ntest::assert_uint16(input_img.props.get_width(), props_found.get_width(), loc);
    ntest::assert_uint16(input_img.props.get_height(), props_found.get_height(), loc);
    ntest::assert_uint8(input_img.props.get_maxval(), props_found.get_maxval(), loc);
    ntest::assert_arr(input_img.pixels, input_img.props.num_pixels(), pixels_found.get(), props_found.num_pixels(), loc);
    ntest::assert_uint64(expected_num_comments_skipped, num_comments_skipped, loc);
  }
}

void write_and_read_back_raw_test(
  std::string const &path_without_ext,
  readonly_image const &input_img,
  std::source_location const loc = std::source_location::current())
{
  std::string const full_path = path_without_ext + ".raw.pgm";
  {
    std::ofstream file(full_path, std::ios::binary);
    pgm8::write(file, input_img.props, input_img.comments, input_img.pixels);
  }
  {
    std::ifstream file(full_path, std::ios::binary);
    auto const props_found = pgm8::read_properties(file);
    auto const comments_found = pgm8::read_comments(file);
    auto const num_pixels_found = props_found.num_pixels();
    std::unique_ptr<uint8_t []> pixels_found(new uint8_t[num_pixels_found]);
    pgm8::read_pixels(file, props_found, pixels_found.get());
    assert_image(input_img, { props_found, comments_found, pixels_found.get() }, loc);
  }
}

int main()
{
  try
  {
    {
      auto const res = ntest::init();
      std::cout
        << res.num_files_removed << " residual files removed, "
        << res.num_files_failed_to_remove << " failed to be removed\n";
    }

    // horizontal and vertical line
    {
      uint8_t pixels[256];
      for (size_t i = 0; i <= UINT8_MAX; ++i)
        pixels[i] = static_cast<uint8_t>(i);

      // horizontal, no comments
      {
        pgm8::image_properties props;
        props.set_width(256);
        props.set_height(1);
        props.set_maxval(UINT8_MAX);

        std::vector<std::string> const comments{};

        props.set_format(pgm8::format::PLAIN);
        write_and_read_back_plain_test("files/no_comments/horiz", { props, comments, pixels });
        read_but_skip_comments_test("files/no_comments/horiz", { props, comments, pixels }, comments.size());

        props.set_format(pgm8::format::RAW);
        write_and_read_back_raw_test("files/no_comments/horiz", { props, comments, pixels });
        read_but_skip_comments_test("files/no_comments/horiz", { props, comments, pixels }, comments.size());
      }
      // horizontal, with comments
      {
        pgm8::image_properties props;
        props.set_width(256);
        props.set_height(1);
        props.set_maxval(UINT8_MAX);

        std::vector<std::string> const comments {
          "comment 1",
        };

        props.set_format(pgm8::format::PLAIN);
        write_and_read_back_plain_test("files/with_comments/horiz", { props, comments, pixels });
        read_but_skip_comments_test("files/with_comments/horiz", { props, comments, pixels }, comments.size());

        props.set_format(pgm8::format::RAW);
        write_and_read_back_raw_test("files/with_comments/horiz", { props, comments, pixels });
        read_but_skip_comments_test("files/with_comments/horiz", { props, comments, pixels }, comments.size());
      }

      // vertical, no comments
      {
        pgm8::image_properties props;
        props.set_width(1);
        props.set_height(256);
        props.set_maxval(UINT8_MAX);

        std::vector<std::string> const comments{};

        props.set_format(pgm8::format::PLAIN);
        write_and_read_back_plain_test("files/no_comments/vert", { props, comments, pixels });
        read_but_skip_comments_test("files/no_comments/vert", { props, comments, pixels }, comments.size());

        props.set_format(pgm8::format::RAW);
        write_and_read_back_raw_test("files/no_comments/vert", { props, comments, pixels });
        read_but_skip_comments_test("files/no_comments/vert", { props, comments, pixels }, comments.size());
      }
      // vertical, with comments
      {
        pgm8::image_properties props;
        props.set_width(1);
        props.set_height(256);
        props.set_maxval(UINT8_MAX);

        std::vector<std::string> const comments {
          "comment 1",
          "comment 2",
        };

        props.set_format(pgm8::format::PLAIN);
        write_and_read_back_plain_test("files/with_comments/vert", { props, comments, pixels });
        read_but_skip_comments_test("files/with_comments/vert", { props, comments, pixels }, comments.size());

        props.set_format(pgm8::format::RAW);
        write_and_read_back_raw_test("files/with_comments/vert", { props, comments, pixels });
        read_but_skip_comments_test("files/with_comments/vert", { props, comments, pixels }, comments.size());
      }
    }

    // horizontal gradient, no comments
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

      std::vector<std::string> const comments{};

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/no_comments/horiz-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/horiz-grad", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/no_comments/horiz-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/horiz-grad", { props, comments, pixels }, comments.size());
    }
    // horizontal gradient, with comments
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

      std::vector<std::string> const comments {
        "comment 1",
        "comment 2",
        "comment 3",
      };

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/with_comments/horiz-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/with_comments/horiz-grad", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/with_comments/horiz-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/with_comments/horiz-grad", { props, comments, pixels }, comments.size());
    }

    // vertical gradient, no comments
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

      std::vector<std::string> const comments{};

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/no_comments/vert-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/vert-grad", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/no_comments/vert-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/vert-grad", { props, comments, pixels }, comments.size());
    }
    // vertical gradient, with comments
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

      std::vector<std::string> const comments {
        "comment 1",
        "comment 2",
        "comment 3",
        "comment 4",
      };

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/with_comments/vert-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/with_comments/vert-grad", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/with_comments/vert-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/with_comments/vert-grad", { props, comments, pixels }, comments.size());
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

      std::vector<std::string> const comments{};

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/no_comments/diag-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/diag-grad", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/no_comments/diag-grad", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/diag-grad", { props, comments, pixels }, comments.size());
    }

    // double digit maxval, no comments
    {
      uint16_t const width = 5, height = 5;
      uint8_t pixels[width * height] {
        0,  0,  0,   0,   0,
        0,  25, 25,  25,  25,
        0,  0,  25,  25,  25,
        0,  0,  0,   25,  25,
        0,  0,  0,   0,   25,
      };

      pgm8::image_properties props;
      props.set_width(width);
      props.set_height(height);
      props.set_maxval(25);

      std::vector<std::string> const comments{};

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/no_comments/double-digit-maxval", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/double-digit-maxval", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/no_comments/double-digit-maxval", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/double-digit-maxval", { props, comments, pixels }, comments.size());
    }

    // triple digit maxval, no comments
    {
      uint16_t const width = 5, height = 5;
      uint8_t pixels[width * height] {
        0,    0,    0,    0,    0,
        0,    255,  255,  255,  255,
        0,    0,    255,  255,  255,
        0,    0,    0,    255,  255,
        0,    0,    0,    0,    255,
      };

      pgm8::image_properties props;
      props.set_width(width);
      props.set_height(height);
      props.set_maxval(255);

      std::vector<std::string> const comments{};

      props.set_format(pgm8::format::PLAIN);
      write_and_read_back_plain_test("files/no_comments/triple-digit-maxval", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/triple-digit-maxval", { props, comments, pixels }, comments.size());

      props.set_format(pgm8::format::RAW);
      write_and_read_back_raw_test("files/no_comments/triple-digit-maxval", { props, comments, pixels });
      read_but_skip_comments_test("files/no_comments/triple-digit-maxval", { props, comments, pixels }, comments.size());
    }

    {
      auto const res = ntest::generate_report("cpp-pgm8");
      std::cout << res.num_fails << " failed, " << res.num_passes << " passed\n";
    }
  }
  catch (std::exception const &err)
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

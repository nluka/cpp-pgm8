#ifndef NLUKA_PGM8_HPP
#define NLUKA_PGM8_HPP

#include <fstream>

// Module for reading and writing 8-bit PGM images.
namespace pgm8 {

enum class format : uint8_t
{
  // Pixels stored in ASCII decimal.
  PLAIN = 2,
  // Pixels stored in binary raster.
  RAW = 5,
};

struct image_properties
{
public:
  [[nodiscard]] uint16_t get_width() const noexcept;
  [[nodiscard]] uint16_t get_height() const noexcept;
  [[nodiscard]] uint8_t get_maxval() const noexcept;
  [[nodiscard]] pgm8::format get_format() const noexcept;

  void set_width(uint16_t) noexcept;
  void set_height(uint16_t) noexcept;
  void set_maxval(uint8_t) noexcept;
  void set_format(format) noexcept;

  [[nodiscard]] size_t num_pixels() const noexcept;

private:
  uint16_t m_width, m_height;
  uint8_t m_maxval;
  format m_fmt;
};

[[nodiscard]] image_properties read_properties(std::ifstream &file);

void read_pixels(
  std::ifstream &file,
  image_properties props,
  uint8_t *buffer
);

void write(
  std::ofstream &file,
  image_properties props,
  uint8_t const *pixels
);

} // namespace pgm8

#endif // NLUKA_PGM8_HPP

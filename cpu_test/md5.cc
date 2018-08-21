#include <algorithm>
#include <iomanip>
#include <sstream>

#include "md5.hh"

// Original code from Alexander Peslyak.
// http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5

/*------------------------------------------------------------------------------------------------*/

namespace {

constexpr auto f = [](auto x, auto y, auto z)
{
  return z ^ (x & (y ^ z));
};

constexpr auto g = [](auto x, auto y, auto z)
{
  return y ^ (z & (x ^ y));
};

constexpr auto h = [](auto x, auto y, auto z)
{
  return (((x) ^ (y)) ^ (z));
};

constexpr auto h2 = [](auto x, auto y, auto z)
{
  return ((x) ^ ((y) ^ (z)));
};

constexpr auto i = [](auto x, auto y, auto z)
{
  return ((y) ^ ((x) | ~(z)));
};

constexpr auto step = [](auto&& fun, auto& a, auto b, auto c, auto d, auto x, auto t, auto s)
{
  a += fun(b, c, d) + x + t;
  a = (a << s) | ((a & 0xffffffff) >> (32 - s));
  a += b;
};

} // unnamed namespace

/*------------------------------------------------------------------------------------------------*/

md5::md5()
  : a_{0x67452301}
  , b_{0xefcdab89}
  , c_{0x98badcfe}
  , d_{0x10325476}
  , lo_{0}
  , hi_{0}
  , buffer_{}
{}

/*------------------------------------------------------------------------------------------------*/

// This processes one or more 64-byte data blocks, but does NOT update the bit
// counters. There are no alignment requirements.
const std::uint8_t*
md5::body(const std::uint8_t* ptr, std::size_t size)
noexcept
{
  auto block = std::array<std::uint_fast32_t, 16>{};

  auto a = std::uint_fast32_t{};
  auto b = std::uint_fast32_t{};
  auto c = std::uint_fast32_t{};
  auto d = std::uint_fast32_t{};

  auto saved_a = std::uint_fast32_t{};
  auto saved_b = std::uint_fast32_t{};
  auto saved_c = std::uint_fast32_t{};
  auto saved_d = std::uint_fast32_t{};

	a = a_;
	b = b_;
	c = c_;
	d = d_;

  const auto set = [&](auto n)
  {
    block[n] =
     (std::uint_fast32_t)ptr[n * 4] |
     ((std::uint_fast32_t)ptr[n * 4 + 1] << 8) |
     ((std::uint_fast32_t)ptr[n * 4 + 2] << 16) |
     ((std::uint_fast32_t)ptr[n * 4 + 3] << 24);

    return block[n];
  };

  const auto get = [&](auto n)
  {
    return block[n];
  };

	do
  {
		saved_a = a;
		saved_b = b;
		saved_c = c;
		saved_d = d;

    // Round 1
		step(f, a, b, c, d, set(0), 0xd76aa478, 7);
		step(f, d, a, b, c, set(1), 0xe8c7b756, 12);
		step(f, c, d, a, b, set(2), 0x242070db, 17);
		step(f, b, c, d, a, set(3), 0xc1bdceee, 22);
		step(f, a, b, c, d, set(4), 0xf57c0faf, 7);
		step(f, d, a, b, c, set(5), 0x4787c62a, 12);
		step(f, c, d, a, b, set(6), 0xa8304613, 17);
		step(f, b, c, d, a, set(7), 0xfd469501, 22);
		step(f, a, b, c, d, set(8), 0x698098d8, 7);
		step(f, d, a, b, c, set(9), 0x8b44f7af, 12);
		step(f, c, d, a, b, set(10), 0xffff5bb1, 17);
		step(f, b, c, d, a, set(11), 0x895cd7be, 22);
		step(f, a, b, c, d, set(12), 0x6b901122, 7);
		step(f, d, a, b, c, set(13), 0xfd987193, 12);
		step(f, c, d, a, b, set(14), 0xa679438e, 17);
		step(f, b, c, d, a, set(15), 0x49b40821, 22);

    // Round 2
		step(g, a, b, c, d, get(1), 0xf61e2562, 5);
		step(g, d, a, b, c, get(6), 0xc040b340, 9);
		step(g, c, d, a, b, get(11), 0x265e5a51, 14);
		step(g, b, c, d, a, get(0), 0xe9b6c7aa, 20);
		step(g, a, b, c, d, get(5), 0xd62f105d, 5);
		step(g, d, a, b, c, get(10), 0x02441453, 9);
		step(g, c, d, a, b, get(15), 0xd8a1e681, 14);
		step(g, b, c, d, a, get(4), 0xe7d3fbc8, 20);
		step(g, a, b, c, d, get(9), 0x21e1cde6, 5);
		step(g, d, a, b, c, get(14), 0xc33707d6, 9);
		step(g, c, d, a, b, get(3), 0xf4d50d87, 14);
		step(g, b, c, d, a, get(8), 0x455a14ed, 20);
		step(g, a, b, c, d, get(13), 0xa9e3e905, 5);
		step(g, d, a, b, c, get(2), 0xfcefa3f8, 9);
		step(g, c, d, a, b, get(7), 0x676f02d9, 14);
		step(g, b, c, d, a, get(12), 0x8d2a4c8a, 20);

    // Round 3
		step(h, a, b, c, d, get(5), 0xfffa3942, 4);
		step(h2, d, a, b, c, get(8), 0x8771f681, 11);
		step(h, c, d, a, b, get(11), 0x6d9d6122, 16);
		step(h2, b, c, d, a, get(14), 0xfde5380c, 23);
		step(h, a, b, c, d, get(1), 0xa4beea44, 4);
		step(h2, d, a, b, c, get(4), 0x4bdecfa9, 11);
		step(h, c, d, a, b, get(7), 0xf6bb4b60, 16);
		step(h2, b, c, d, a, get(10), 0xbebfbc70, 23);
		step(h, a, b, c, d, get(13), 0x289b7ec6, 4);
		step(h2, d, a, b, c, get(0), 0xeaa127fa, 11);
		step(h, c, d, a, b, get(3), 0xd4ef3085, 16);
		step(h2, b, c, d, a, get(6), 0x04881d05, 23);
		step(h, a, b, c, d, get(9), 0xd9d4d039, 4);
		step(h2, d, a, b, c, get(12), 0xe6db99e5, 11);
		step(h, c, d, a, b, get(15), 0x1fa27cf8, 16);
		step(h2, b, c, d, a, get(2), 0xc4ac5665, 23);

    // Round 4
		step(i, a, b, c, d, get(0), 0xf4292244, 6);
		step(i, d, a, b, c, get(7), 0x432aff97, 10);
		step(i, c, d, a, b, get(14), 0xab9423a7, 15);
		step(i, b, c, d, a, get(5), 0xfc93a039, 21);
		step(i, a, b, c, d, get(12), 0x655b59c3, 6);
		step(i, d, a, b, c, get(3), 0x8f0ccc92, 10);
		step(i, c, d, a, b, get(10), 0xffeff47d, 15);
		step(i, b, c, d, a, get(1), 0x85845dd1, 21);
		step(i, a, b, c, d, get(8), 0x6fa87e4f, 6);
		step(i, d, a, b, c, get(15), 0xfe2ce6e0, 10);
		step(i, c, d, a, b, get(6), 0xa3014314, 15);
		step(i, b, c, d, a, get(13), 0x4e0811a1, 21);
		step(i, a, b, c, d, get(4), 0xf7537e82, 6);
		step(i, d, a, b, c, get(11), 0xbd3af235, 10);
		step(i, c, d, a, b, get(2), 0x2ad7d2bb, 15);
		step(i, b, c, d, a, get(9), 0xeb86d391, 21);

		a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;

		ptr += 64;
	} while (size -= 64);

	a_ = a;
	b_ = b;
	c_ = c;
	d_ = d;

	return ptr;
}

/*------------------------------------------------------------------------------------------------*/

md5&
md5::update(const std::uint8_t* data, std::size_t size)
noexcept
{
	const auto saved_lo = lo_;

	if ((lo_ = (saved_lo + size) & 0x1fffffff) < saved_lo)
  {
		hi_++;
  }
	hi_ += size >> 29;

	if (const auto used = saved_lo & 0x3f; used)
  {
		const auto available = 64 - used;

		if (size < available)
    {
      std::copy_n(data, size, begin(buffer_) + used);
			return *this;
		}

    std::copy_n(data, available, begin(buffer_) + used);
		data = data + available;
		size -= available;
		body(buffer_.data(), 64);
	}

	if (size >= 64)
  {
		data = body(data, size & ~std::size_t{0x3f});
		size &= 0x3f;
	}

  std::copy_n(data, size, begin(buffer_));

  return *this;
}

/*------------------------------------------------------------------------------------------------*/

#define OUT(dst, src) \
	(dst)[0] = (unsigned char)(src); \
	(dst)[1] = (unsigned char)((src) >> 8); \
	(dst)[2] = (unsigned char)((src) >> 16); \
	(dst)[3] = (unsigned char)((src) >> 24);

md5::checksum
md5::operator()()
noexcept
{
  auto result = checksum{};
  auto used = std::size_t{lo_ & 0x3f};
  auto available = std::size_t{64 - used};

	buffer_[used++] = 0x80;

	if (available < 8)
  {
    std::fill_n(begin(buffer_) + used, available, 0);
		body(buffer_.data(), 64);
		used = 0;
		available = 64;
	}

  std::fill_n(begin(buffer_) + used, available - 8, 0);

	lo_ <<= 3;
	OUT(&buffer_[56], lo_)
	OUT(&buffer_[60], hi_)

	body(buffer_.data(), 64);

	OUT(&result[0], a_)
	OUT(&result[4], b_)
	OUT(&result[8], c_)
	OUT(&result[12], d_)

  return result;
}

/*------------------------------------------------------------------------------------------------*/

std::string
md5::digest(const checksum& cs)
{
  auto oss = std::ostringstream{};
  for (const auto c : cs)
  {
    oss << std::setfill('0') << std::setw(2) << std::hex << +c;
  }
  return oss.str();
}

/*------------------------------------------------------------------------------------------------*/

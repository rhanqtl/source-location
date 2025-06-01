#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <string_view>
#include <vector>

#ifndef SLNS
#  define SLNS_BEGIN
#  define SLNS_END
#else
#  ifndef SLNS_BEGIN
#    define SLNS_BEGIN namespace SLNS {
#  endif
#  ifndef SLNS_END
#    define SLNS_END }
#  endif
#endif

#ifndef SL_FILE_ID_BITS
#  define SL_FILE_ID_BITS 20
#endif

SLNS_BEGIN

class SourceLocation {
 public:
  static constexpr std::size_t FileIDBits = SL_FILE_ID_BITS;
  static constexpr std::size_t OffsetBits = 64 - FileIDBits;

 private:
  uint64_t file_id_ : FileIDBits;
  uint64_t offset_ : OffsetBits;
};
static_assert(sizeof(SourceLocation) == 8);

class SourceBuffer {
 public:
  explicit SourceBuffer(const std::filesystem::path& path) {
    std::ifstream fin{path};
    if (!fin)
      throw std::runtime_error{"Failed to open file: " + path.string()};
    fin.seekg(0, std::ios::end);
    std::size_t sz = fin.tellg();
    fin.seekg(0, std::ios::beg);
    content_.resize(sz);
    fin.read(content_.data(), sz);

    init_line_starts(content_);
  }
  explicit SourceBuffer(std::string_view content) : content_{content}{
    init_line_starts(content_);
  }

  std::size_t line(std::size_t offset) const {
    auto it = std::ranges::upper_bound(line_starts_, offset);
    return std::distance(line_starts_.begin(), it);
  }
  /// \return 1-based column number
  std::size_t column(std::size_t offset) const {
    auto it = std::ranges::lower_bound(line_starts_, offset);
    assert(it != line_starts_.end());
    if (*it == offset)
      return 1;
    return offset - *(it - 1) + 1;
  }

 private:
  void init_line_starts(std::string_view content) {
    line_starts_.clear();
    line_starts_.push_back(0);
    for (std::size_t i = 0; i < content.size(); ++i) {
      if (content[i] == '\n')
        line_starts_.push_back(i + 1);
    }
    if (line_starts_.back() != content.length())
      line_starts_.push_back(content.length());
  }

 private:
  std::string content_;
  std::vector<std::size_t> line_starts_;
#ifndef NDEBUG
  std::string_view name_;
#endif
};

class SourceManager {
 public:
  SourceManager();

 public:
  std::string_view file(SourceLocation loc) const;
  std::size_t line(SourceLocation loc) const;
  std::size_t column(SourceLocation loc) const;

 private:
  SourceBuffer& from_file(const std::filesystem::path& path);
  SourceBuffer& from_string(std::string_view content);
};

SLNS_END

#endif  // SOURCE_LOCATION_H

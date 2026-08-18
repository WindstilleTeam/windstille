#include "plugins/stb_image_loader.hpp"

#include <stdexcept>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstring>

#include <geom/size.hpp>

#include "software_surface_factory.hpp"
#include "software_surface_loader.hpp"
#include "pixel.hpp"
#include "pixel.hpp"
#include "pixel_data.hpp"

// Implementation lives in mk/android img_stb_min.c when building the APK.
// For other targets that compile this TU without a separate STB implementation,
// define STB_IMAGE_IMPLEMENTATION here.
#if !defined(STB_IMAGE_IMPLEMENTATION) && !defined(__ANDROID__)
#  define STB_IMAGE_IMPLEMENTATION
#  define STBI_NO_THREAD_LOCALS
#  define STBI_NO_HDR
#  define STBI_NO_LINEAR
#endif
#include "stb_image.h"

namespace surf {
namespace stb_image_loader {

SoftwareSurface load_from_file(std::filesystem::path const& filename)
{
  int w = 0, h = 0, n = 0;
  unsigned char* data = stbi_load(filename.string().c_str(), &w, &h, &n, 4);
  if (!data) {
    std::ostringstream out;
    out << "stb_image: failed to load '" << filename.string() << "': "
        << (stbi_failure_reason() ? stbi_failure_reason() : "unknown");
    throw std::runtime_error(out.str());
  }

  PixelData<RGBAPixel> dst(geom::isize(w, h));
  memcpy(dst.get_data(), data, static_cast<size_t>(w) * static_cast<size_t>(h) * 4u);
  stbi_image_free(data);
  return SoftwareSurface(dst);
}

void register_loader(SoftwareSurfaceFactory& factory)
{
  auto loader = make_loader("stb", load_from_file, nullptr);
  factory.register_by_extension(*loader, "png");
  factory.register_by_extension(*loader, "jpg");
  factory.register_by_extension(*loader, "jpeg");
  factory.register_by_mime_type(*loader, "image/png");
  factory.register_by_mime_type(*loader, "image/jpeg");
  factory.add_loader(std::move(loader));
}

} // namespace stb_image_loader
} // namespace surf

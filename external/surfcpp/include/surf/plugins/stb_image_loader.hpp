// Minimal PNG/JPEG loader via stb_image (used when libpng is unavailable, e.g. Android).
#ifndef HEADER_SURF_PLUGINS_STB_IMAGE_LOADER_HPP
#define HEADER_SURF_PLUGINS_STB_IMAGE_LOADER_HPP

#include <filesystem>

#include <surf/fwd.hpp>

namespace surf {
namespace stb_image_loader {

SoftwareSurface load_from_file(std::filesystem::path const& filename);
void register_loader(SoftwareSurfaceFactory& factory);

} // namespace stb_image_loader
} // namespace surf

#endif

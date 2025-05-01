set(md_dir material-color/cpp)

file(GLOB_RECURSE material_color_sources
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${md_dir}/utils/*.cc"
    "${md_dir}/quantize/*.cc"
    "${md_dir}/dynamiccolor/*.cc"
    "${md_dir}/contrast/*.cc"
    "${md_dir}/palettes/*.cc"
    "${md_dir}/dislike/*.cc"
    "${md_dir}/cam/*.cc"
    "${md_dir}/score/*.cc"
    "${md_dir}/scheme/*.cc"
)

add_library(
  material_color STATIC
  ${material_color_sources}
)


target_compile_features(material_color PRIVATE cxx_std_20)
target_compile_options(material_color PRIVATE -w)
target_include_directories(material_color PUBLIC material-color)
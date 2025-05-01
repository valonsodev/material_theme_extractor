#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <optional>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/quantize/celebi.h"
#include "cpp/score/score.h"
#include "cpp/utils/utils.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include "cpp/dynamiccolor/material_dynamic_colors.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "CLI11.hpp"
#include "json.hpp"

material_color_utilities::Argb pack_argb(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)a << 24) |
        ((uint32_t)r << 16) |
        ((uint32_t)g << 8) |
        ((uint32_t)b);
}

std::vector<material_color_utilities::Argb> loadImagePixels(const std::string& imagePath, bool preserve_alpha = false)
// PATCHED
{
    std::vector<material_color_utilities::Argb> pixels;
    int width, height, channels;
    unsigned char* img_data = stbi_load(imagePath.c_str(), &width, &height, &channels, 4);
    if (img_data == nullptr)
    {
        std::cerr << "Error loading image: " << imagePath << std::endl;
        return pixels;
    }
    int num_pixels = width * height;
    pixels.reserve(num_pixels);
    for (int i = 0; i < num_pixels; ++i)
    {
        unsigned char r = img_data[i * 4 + 0];
        unsigned char g = img_data[i * 4 + 1];
        unsigned char b = img_data[i * 4 + 2];
        unsigned char a = channels >= 4 ? img_data[i * 4 + 3] : 0xff;
        if (!preserve_alpha)
        {
            a = 0xff;
        }
        material_color_utilities::Argb argb = pack_argb(a, r, g, b);
        pixels.push_back(argb);
    }
    stbi_image_free(img_data);
    return pixels;
}

enum class ColorFormat
{
    ARGB, // #AARRGGBB
    RGB, // #RRGGBB
    DEFAULT
};

std::string ArgbToHex(const material_color_utilities::Argb argb, ColorFormat color_format = ColorFormat::RGB)
{
    std::stringstream ss;
    int a = material_color_utilities::AlphaFromInt(argb);
    int r = material_color_utilities::RedFromInt(argb);
    int g = material_color_utilities::GreenFromInt(argb);
    int b = material_color_utilities::BlueFromInt(argb);
    switch (color_format)
    {
    case ColorFormat::ARGB:
    default:
        ss << "#" << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << a
            << std::setfill('0') << std::setw(2) << r
            << std::setfill('0') << std::setw(2) << g
            << std::setfill('0') << std::setw(2) << b;
        break;
    case ColorFormat::RGB:
        ss << "#" << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << r
            << std::setfill('0') << std::setw(2) << g
            << std::setfill('0') << std::setw(2) << b;
        break;
    }
    return ss.str();
}

nlohmann::ordered_json color_scheme_to_json(
    const material_color_utilities::DynamicScheme& scheme,
    ColorFormat color_format)
{
    using DC = material_color_utilities::MaterialDynamicColors;
    nlohmann::ordered_json colors_json;

    const std::vector<material_color_utilities::DynamicColor> color_list = {
        DC::Primary(), // "primary"
        DC::SurfaceTint(), // "surfaceTint"
        DC::OnPrimary(), // "onPrimary"
        DC::PrimaryContainer(), // "primaryContainer"
        DC::OnPrimaryContainer(), // "onPrimaryContainer"
        DC::Secondary(), // "secondary"
        DC::OnSecondary(), // "onSecondary"
        DC::SecondaryContainer(), // "secondaryContainer"
        DC::OnSecondaryContainer(), // "onSecondaryContainer"
        DC::Tertiary(), // "tertiary"
        DC::OnTertiary(), // "onTertiary"
        DC::TertiaryContainer(), // "tertiaryContainer"
        DC::OnTertiaryContainer(), // "onTertiaryContainer"
        DC::Error(), // "error"
        DC::OnError(), // "onError"
        DC::ErrorContainer(), // "errorContainer"
        DC::OnErrorContainer(), // "onErrorContainer"
        DC::Background(), // "background"
        DC::OnBackground(), // "onBackground"
        DC::Surface(), // "surface"
        DC::OnSurface(), // "onSurface"
        DC::SurfaceVariant(), // "surfaceVariant"
        DC::OnSurfaceVariant(), // "onSurfaceVariant"
        DC::Outline(), // "outline"
        DC::OutlineVariant(), // "outlineVariant"
        DC::Shadow(), // "shadow"
        DC::Scrim(), // "scrim"
        DC::InverseSurface(), // "inverseSurface"
        DC::InverseOnSurface(), // "inverseOnSurface"
        DC::InversePrimary(), // "inversePrimary"
        DC::PrimaryFixed(), // "primaryFixed"
        DC::OnPrimaryFixed(), // "onPrimaryFixed"
        DC::PrimaryFixedDim(), // "primaryFixedDim"
        DC::OnPrimaryFixedVariant(), // "onPrimaryFixedVariant"
        DC::SecondaryFixed(), // "secondaryFixed"
        DC::OnSecondaryFixed(), // "onSecondaryFixed"
        DC::SecondaryFixedDim(), // "secondaryFixedDim"
        DC::OnSecondaryFixedVariant(), // "onSecondaryFixedVariant"
        DC::TertiaryFixed(), // "tertiaryFixed"
        DC::OnTertiaryFixed(), // "onTertiaryFixed"
        DC::TertiaryFixedDim(), // "tertiaryFixedDim"
        DC::OnTertiaryFixedVariant(), // "onTertiaryFixedVariant"
        DC::SurfaceDim(), // "surfaceDim"
        DC::SurfaceBright(), // "surfaceBright"
        DC::SurfaceContainerLowest(), // "surfaceContainerLowest"
        DC::SurfaceContainerLow(), // "surfaceContainerLow"
        DC::SurfaceContainer(), // "surfaceContainer"
        DC::SurfaceContainerHigh(), // "surfaceContainerHigh"
        DC::SurfaceContainerHighest(), // "surfaceContainerHighest"
        DC::PrimaryPaletteKeyColor(),
        DC::SecondaryPaletteKeyColor(),
    };

    for (auto dynamic_color : color_list)
    {
        const std::string name = dynamic_color.name_;
        const auto argb = dynamic_color.GetArgb(scheme);
        colors_json[name] = ArgbToHex(argb, color_format);
    }

    return colors_json;
}

int main(int argc, char* argv[])
{
    std::string imagePath;

    CLI::App app{"Extracts Material You color scheme from an image."};
    app.require_subcommand(0);

    app.add_option("image", imagePath, "Path of image to process (required)")->required();

    auto color_group = app.add_option_group("Color Format");
    int color_opt = 1;
    color_group->add_flag_function("--rgb", [&](std::size_t)
    {
        color_opt = 1;
    }, "Output colors in #RRGGBB (default)");
    color_group->add_flag_function("--argb", [&](std::size_t)
    {
        color_opt = 0;
    }, "Output colors in #AARRGGBB");
    bool is_dark = false;
    bool dark_flag = false;
    bool light_flag = false;

    auto theme_group = app.add_option_group("Theme");
    theme_group->add_flag("--dark", dark_flag, "Output only dark theme (requires --contrast or auto-selects)");
    theme_group->add_flag("--light", light_flag, "Output only light theme (requires --contrast or auto-selects)");

    bool contrast_given = false;
    double contrast_level = 0.0;
    app.add_option("--contrast", contrast_level,
                   "Contrast level (-1.0 to 1.0) -1.0 = reduced 0.0 = standard 1.0 = increased")
       ->check(CLI::Range(-1.0, 1.0))
       ->capture_default_str();
    app.get_option("--contrast")->default_str("none");

    bool preserve_alpha = false;
    app.add_flag("--preserve-alpha", preserve_alpha,
                 "Preserve original image alpha channel (default: discard and use 255) !!WARNING: THIS DOES SOME WONKY THINGS!!");

    bool debug = false;
    app.add_flag("--debug", debug, "Print debug information such as CLI args and processing details");

    CLI11_PARSE(app, argc, argv);

    ColorFormat chosen_format = (color_opt == 0) ? ColorFormat::ARGB : ColorFormat::RGB;

    bool output_single = false;
    if (app.count("--contrast") > 0)
        contrast_given = true;

    if (contrast_given)
    {
        is_dark = !light_flag;
        output_single = true;
    }
    else if (dark_flag)
    {
        is_dark = true;
        contrast_level = 0.0;
        output_single = true;
    }
    else if (light_flag)
    {
        is_dark = false;
        contrast_level = 0.0;
        output_single = true;
    }
    else
    {
        output_single = false;
    }
    if (debug)
    {
        std::cerr << "\n===== Debug Info =====\n";
        std::cerr << "Image file   : " << imagePath << "\n";
        std::cerr << "color_opt    : " << color_opt << "\n";
        std::cerr << "Color format : " << ((color_opt == 0) ? "#AARRGGBB (ARGB)" : "#RRGGBB (RGB)") << "\n";
        std::cerr << "Flags        :"
            << (dark_flag ? " --dark" : "")
            << (light_flag ? " --light" : "")
            << (contrast_given ? " --contrast" : "")
            << "\n";
        std::cerr << "Contrast     : " << std::to_string(contrast_level) << "\n";
        std::cerr << "Preserve alpha : " << (preserve_alpha ? "true" : "false") <<
            " (will use image alpha if true, else always 255)\n";

        if (output_single)
        {
            std::cerr << "Mode         : SINGLE theme generation\n";
            std::cerr << "Theme        : " << (is_dark ? "Dark" : "Light") << "\n";
        }
        else
        {
            std::cerr << "Mode         : MULTIPLE theme generation (all: dark/light x low/medium/high contrast)\n";
        }

        std::cerr << "======================\n"
            << std::endl;
    }
    if (debug)
        std::cerr << "Opening image: " << imagePath << std::endl;
    std::ifstream f(imagePath.c_str());
    if (!f.good())
    {
        std::cerr << "Cannot open image file: " << imagePath << std::endl;
        return 2;
    }
    f.close();

    std::vector<material_color_utilities::Argb> pixels = loadImagePixels(imagePath, preserve_alpha);
    if (debug)
        std::cerr << "Loaded " << pixels.size() << " pixels from image\n";
    if (pixels.empty())
        return 1;

    int maxColors = 128;
    material_color_utilities::QuantizerResult quantizer_result =
        material_color_utilities::QuantizeCelebi(pixels, maxColors);

    std::vector<material_color_utilities::Argb> ranked_colors =
        material_color_utilities::RankedSuggestions(quantizer_result.color_to_count);

    if (ranked_colors.empty())
    {
        std::cerr << "No suitable source colors found from the image." << std::endl;
        return 1;
    }

    material_color_utilities::Argb source_color_argb = ranked_colors[0];
    material_color_utilities::Hct source_color_hct(source_color_argb);

    std::string seed_hex = ArgbToHex(source_color_argb, chosen_format);

    if (debug)
    {
        std::cerr << "Found " << ranked_colors.size() << " ranked colors, source ARGB: 0x" << std::hex <<
            source_color_argb << std::dec << "\n";
        std::cerr << "Seed color hex: " << seed_hex << "\n";
    }

    if (output_single)
    {
        material_color_utilities::SchemeTonalSpot scheme(source_color_hct, is_dark, contrast_level);
        nlohmann::ordered_json colors_json = color_scheme_to_json(scheme, chosen_format);
        std::cout << colors_json.dump(4) << std::endl;
    }
    else
    {
        nlohmann::ordered_json output_json;
        output_json["seed"] = seed_hex;
        std::vector<std::pair<std::string, double>> contrast_levels = {
            {"standard", 0.0},
            {"reduced_contrast", -1.0},
            {"medium_contrast", 0.5},
            {"high_contrast", 1.0}
        };
        std::vector<std::pair<std::string, bool>> modes = {
            {"dark", true},
            {"light", false}
        };
        nlohmann::ordered_json themes_json;
        for (const auto& contrast : contrast_levels)
        {
            nlohmann::ordered_json contrast_json;
            for (const auto& mode : modes)
            {
                material_color_utilities::SchemeTonalSpot scheme(
                    source_color_hct,
                    mode.second,
                    contrast.second);
                nlohmann::ordered_json colors_json = color_scheme_to_json(scheme, chosen_format);
                contrast_json[mode.first] = colors_json;
            }
            themes_json[contrast.first] = contrast_json;
        }
        output_json["themes"] = themes_json;
        std::cout << output_json.dump(4) << std::endl;
    }
    return 0;
}

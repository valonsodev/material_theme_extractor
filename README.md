# Material Theme Extractor

This program extracts Material You color schemes from an input image file and outputs them in JSON format. It utilizes the `material-color-utilities` C++ library to perform color quantization and scheme generation.

## Dependencies

- [material-color-utilities](https://github.com/material-foundation/material-color-utilities) (included)
- [CLI11.hpp](https://github.com/CLIUtils/CLI11) (included)
- [json.hpp](https://github.com/nlohmann/json) (included)
- [stb_image.h](https://github.com/nothings/stb) (included)
- CMake (version 3.14 or higher)
- A C++20 compatible compiler

## Building

You can build the project using the provided CMake configuration and build script:

**Using the script:**

```sh
./build.sh
```

This will create the executable in the [build](./build) directory. Alternatively, you can use CMake directly:

```sh
cmake -S . -B build

cmake --build build
```

The executable will be located at `build/material_theme`.

## Usage

```sh
./build/material_theme <image> [OPTIONS]
```

Positional Arguments:

- `<image>`: Path to the input image file (required). Accepts any format supported by `stb_image.h`. So things like PNG, JPEG, BMP, and GIF

Options:

- `--rgb`: Output colors in #RRGGBB format (default).
- `--argb`: Output colors in #AARRGGBB format.
- `--dark`: Output only the dark theme. Requires `--contrast` or defaults to standard contrast (0.0).
- `--light`: Output only the light theme. Requires `--contrast` or defaults to standard contrast (0.0).
- `--contrast <level>`: Specify the contrast level (-1.0 to 1.0). If provided, only the theme corresponding to `--dark` (default) or `--light` will be generated at this contrast level.
- `--debug`: Print debug information during processing.
- `-h, --help`: Print the help message and exit.

Behavior:

- If neither `--dark`, `--light`, nor `--contrast` is specified, the program generates themes for both light and dark modes across reduced (-1.0), standard (0.0), medium (0.5), and high (1.0) contrast levels.
- If `--dark` or `--light` is specified _without_ `--contrast`, the corresponding theme is generated at the standard contrast (0.0).
- If `--contrast` is specified, only a single theme is generated:
  - Dark theme if `--light` is _not_ present.
  - Light theme if `--light` _is_ present.

## Output Format

The program outputs JSON to standard output.

#### Multiple Themes (Default):

```json
{
  "seed": "#HEX", // The color ranked first in the source image according to Google's thingy 
  "themes": {
    "standard": {
      "light": {
        "primary": "#HEX",
        "surface_tint": "#HEX"
        // ... other color roles
      },
      "dark": {
        /* Color roles and hex values */
      }
    },
    "reduced_contrast": {
      /* Same as standard */
    },
    "medium_contrast": {
      /* Same as standard */
    },
    "high_contrast": {
      /* Same as standard */
    }

  }
}
```

---

#### Single Theme (`--dark`, `--light`, or `--contrast` used):

```json
{
  "primary": "#HEX",

  "surface_tint": "#HEX"

  // ... other color roles
}
```

The hex color format (`#AARRGGBB` or `#RRGGBB`) depends on the `--argb` or `--rgb` flag used.

# font2c
```
Usage: font2c [OPTION]... [FONT PATH] [OUTPUT PATH]

Convert font glyphs into bitmap images embeddable in C source code.

  --help                        display this information and exit
  --version                     display version information and exit
  -s, --size=PIXELS             Font size (default = 12)
  -r, --raster-type=TYPE        Rasterization type (default = lrtb)
  -c, --char-set=PATH           Path to character set file
  -y, --symbol=NAME             Symbol name for font2c_face_t object
  -d, --depth=BPP               Pixel depth (must be 1, 2, 4 or 8, default = 8)
  -m, --msb-first               Pack most-significant bits first
  -a, --antialiasing            Enable antialiasing
  -i, --no-hinting              Disable hinting
  -p, --preview=PATH            Preview output file path

If no character set file is specified, a default character set consisting of
ASCII codes 32-126 (inclusive) will be used.

Supported raster types:
  lrtb        Scans left-to-right, top-to-bottom
```
## Supported font formats
Anything supported by FreeType.

## Output format
See `font2c-types.h`.

## Dependencies
- cmake 3.22 or higher
- C++17 compliant compiler and standard libraries (has so far only been built with gcc 12.1.1)
- freetype 24.x.x or higher

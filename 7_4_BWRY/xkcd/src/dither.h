#ifndef _DISTHER_H_
#define _DISTHER_H_

struct Color {
    uint8_t r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(uint16_t value_) : r(((value_ >> 8) & 0xF8) | ((value_ >> 13) & 0x07)), g(((value_ >> 3) & 0xFC) | ((value_ >> 9) & 0x03)), b(((value_ << 3) & 0xF8) | ((value_ >> 2) & 0x07)) {}
    Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
};

// bpp normally contains the literal bits per pixel. Values 1 and 2 use OEPL's
// bit-plane layout, so packed two-bit palette indices need a distinct value.
constexpr uint8_t BPP_PACKED_2BIT = 5;

struct HwType {
    std::vector<Color> colortable;
};

struct imgParam {
    HwType hwdata;
    bool hasRed;
    uint8_t dither;
    uint8_t bufferbpp;
    uint8_t rotate;
    uint8_t rotatebuffer;
    uint8_t bpp;
    uint8_t invert;
};

void spr2color(TFT_eSprite &spr, imgParam &imageParams, uint8_t *buffer, size_t buffer_size, bool is_red);

#endif   // _DISTHER_H_

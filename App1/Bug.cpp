// cl.exe version 19.00.24215.1

// If you #define NOBUG, then malloc will receive its expected argument (12)
// If you leave NOBUG undefined, malloc receives 0
// By playing around with the number of fields in Bug2 (and/or arguments in its constructor)
// malloc can be observed to recieve varying unexpected nonzero argument values

#include "pch.h"

//#define NOBUG

#include "Bug.h"

#include <string>
#include <memory>

uint8_t bug9(int depth) {
    switch (depth) {
    case 0: return 4;
    case 1: return 2;
    case 2: return 1;
    case 3: return 2;
    default:
        return 0;
    }
}

class Bug2 : public std::enable_shared_from_this<Bug2> {
public:
    static std::shared_ptr<Bug2> Bug2::call_malloc(uint32_t widthPix, uint32_t heightPix, int depth,
        float scale, std::unique_ptr<uint8_t> pix_, bool premultiplied) {
        uint8_t *pix;
        if (!pix_) {
#ifdef NOBUG
            pix = reinterpret_cast<uint8_t *>(std::malloc(widthPix * heightPix * 4)); // 4 is what bug9(depth) returns
#else
            pix = reinterpret_cast<uint8_t *>(std::malloc(widthPix * heightPix * bug9(depth)));
#endif
        } else {
            pix = pix_.release();
        }

        return std::shared_ptr<Bug2>(new Bug2(pix, nullptr, scale, depth, 0,
            0, widthPix, heightPix, widthPix * bug9(depth), premultiplied));
    }

    uint8_t *const _stbImage = nullptr;

    Bug2::Bug2(uint8_t *const pixels, uint8_t *const stbImage, float scale, int depth,
        int type, int internalFmt, int widthPix, int heightPix, int stride, bool premultiplied)
        : bug6(pixels), _stbImage(stbImage), _scale(scale), _depth(depth), _type(type),
        _internalFmt(internalFmt), _widthPix(widthPix), _heightPix(heightPix), _stride(stride),
        _isPremultiplied(premultiplied) { }

    uint8_t *const bug6 = nullptr;
    const bool _isPremultiplied;
    const int _type;
    const int _internalFmt;
    const int _widthPix;
    const int _heightPix;
    const float _scale;
    const int _stride;
    const int _depth;
    std::string _resourceName;
};

static uint32_t bug7[] = {
    0x02787878,0x0574797c,0x07707b80
};

void Bug::bug() {
    const uint32_t w = sizeof(bug7)/sizeof(bug7[0]);

    auto bug4 = Bug2::call_malloc(w, 1, 0, 1, nullptr, false);
    uint32_t *bug5 = reinterpret_cast<uint32_t *>(bug4->bug6);

    // if you comment the following block out, then malloc gets different wrong arguments
    for (uint32_t i=0; i<w; i++) {
        auto bug8 = bug7[i];
        uint8_t red = static_cast<uint8_t>(bug8 & 0x000000ff);
        bug5[i] = red + 1;
    }

    if (_msize(bug4->bug6) != w*4) {
        std::string formatted = "bad code gen? allocated bytes: ";
        formatted += std::to_string(_msize(bug4->bug6));
        formatted += " expected bytes: ";
        formatted += std::to_string(w*4);
        formatted += '\n';

        auto wformatted = std::wstring(formatted.begin(), formatted.end());
        OutputDebugString(wformatted.c_str());
        __debugbreak();
    }
}

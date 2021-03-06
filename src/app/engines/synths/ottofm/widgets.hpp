#pragma once

#include "app/engines/synths/ottofm/state.hpp"
#include "ottofm.hpp"

#include "lib/skia/anim.hpp"
#include "lib/skia/skia.hpp"
#include "lib/widget.hpp"

namespace otto::engines::ottofm {

  struct ADSR : graphics::Widget<ADSR> {
    float a = 0;
    float d = 0;
    float s = 0;
    float r = 0;
    bool active = false;
    void do_draw(skia::Canvas& ctx);
  };

  void draw_envelopes(skia::Canvas& ctx);

  struct Operators : graphics::Widget<Operators> {
    int algorithm_idx = 0, cur_op = 0;
    std::array<float, 4> activity_levels = {};
    std::array<skia::Color, 4> operator_colours = {colors::blue, colors::green, colors::yellow, colors::red};

    void do_draw(skia::Canvas& ctx);
  };

  // Should be resized when expanding and compressing
  struct FractionGraphic : graphics::Widget<FractionGraphic> {
    int numerator = 0;
    int denominator = 0;
    float expansion = 0;
    bool active = false;

    void do_draw(skia::Canvas& ctx);
  };

  struct DetuneGraphic : graphics::Widget<DetuneGraphic> {
    float value = 0;
    float expansion = 0;
    bool active = false;

    void do_draw(skia::Canvas& ctx);
  };

  struct LevelGraphic : graphics::Widget<LevelGraphic> {
    float value = 0;
    float expansion = 0;
    bool active = false;

    void do_draw(skia::Canvas& ctx);
  };

  // Should only be constructed once.
  struct WaveShapeGraphic : graphics::Widget<WaveShapeGraphic> {
    float value = 0, l_value = 0, r_value = 0;
    float expansion = 0;
    bool active = false;
    std::array<float, 30> sinewave = {};
    std::array<float, 30> right_harmonics = {};
    std::array<float, 30> left_harmonics = {};

    WaveShapeGraphic();

    void do_draw(skia::Canvas& ctx);
  };

} // namespace otto::engines::ottofm

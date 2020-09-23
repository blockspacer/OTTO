#include "lib/itc/itc.hpp"
#include "lib/util/with_limits.hpp"

#include <Gamma/Oscillator.h>

#include "app/services/audio.hpp"
#include "app/services/config.hpp"
#include "app/services/controller.hpp"
#include "app/services/graphics.hpp"
#include "app/services/impl/runtime.hpp"
#include "app/services/logic_thread.hpp"

using namespace otto;
using namespace otto::services;

namespace otto::engines {
  struct IDrawable {
    virtual void draw(SkCanvas& ctx) noexcept = 0;
  };

  struct IScreen : IDrawable {};

  namespace simple {
    struct State {
      util::StaticallyBounded<float, 11, 880> freq = 340;
    };

    struct Logic final : itc::Producer<State> {
      using Producer::Producer;
    };

    struct Handler final : InputHandler {
      Handler(Logic& l) : logic(l) {}
      Logic& logic;

      void handle(EncoderEvent e) noexcept final
      {
        logic.produce(itc::increment(&State::freq, e.steps));
      }
    };

    struct Audio final : services::Audio::Consumer<State>, core::ServiceAccessor<services::Audio> {
      using Consumer::Consumer;

      void on_state_change(const State& s) noexcept override
      {
        osc.freq(s.freq);
      }
      util::audio_buffer process() noexcept
      {
        auto buf = service<services::Audio>().buffer_pool().allocate();
        std::ranges::generate(buf, osc);
        return buf;
      }
      gam::Sine<> osc;
    };

    struct Screen final : services::Graphics::Consumer<State>, otto::engines::IScreen {
      using Consumer::Consumer;

      void draw(SkCanvas& ctx) noexcept override
      {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorWHITE);
        paint.setStyle(SkPaint::kFill_Style);
        ctx.drawCircle({160, 120}, state().freq.normalize() * 100.f, paint);
      }
    };
  } // namespace simple

  struct Simple {
    using State = simple::State;
    using Logic = simple::Logic;
    using Audio = simple::Audio;
    using Screen = simple::Screen;
    using Handler = simple::Handler;
  };

}; // namespace otto::engines

int main(int argc, char* argv[])
{
  using namespace services;
  auto app = start_app(ConfigManager::make_default(), //
                       LogicThread::make_default(),   //
                       Controller::make_board(),      //
                       Audio::make_board(),           //
                       Graphics::make_board()         //
  );

  itc::Channel<otto::engines::Simple::State> chan;
  engines::Simple::Logic l(chan);
  engines::Simple::Audio a(chan);
  engines::Simple::Screen s(chan);
  engines::Simple::Handler h(l);

  app.service<Audio>().set_process_callback([&](auto& data) {
    const auto res = a.process();
    std::ranges::copy(res, data.output.left.begin());
    std::ranges::copy(res, data.output.right.begin());
  });
  app.service<Graphics>().show([&](SkCanvas& ctx) { s.draw(ctx); });
  app.service<Controller>().set_input_handler(h);

  app.wait_for_stop();
}

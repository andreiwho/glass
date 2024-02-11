#pragma once
#include "glass/glass.h"
#include "windowing/window.h"

namespace glass::gfx {
    class GLASS_API Context {
    public:
        Context(const ContextSpec& spec);
        ~Context();

        inline platform::Window* getWindow() const { return const_cast<platform::Window*>(m_Window); }
        void present();

        void setVSyncEnabled(bool enabled);

    private:
        const platform::Window* m_Window{};
        bool m_VSyncEnabled = false;
    };
} // namespace glass::gfx
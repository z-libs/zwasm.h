#define ZWASM_IMPLEMENTATION
#include "zwasm.h"

// minimal-math.h - Just for the example
template <typename T>
T abs(T val) { return val < 0 ? -val : val; }

class BouncingBox {
private:
    float x, y;
    float dx, dy;
    float size;
    const char* color;

public:
    // Constructor
    BouncingBox(float startX, float startY, const char* c) 
        : x(startX), y(startY), dx(2.0f), dy(2.0f), size(40.0f), color(c) {}

    void update(float canvasWidth, float canvasHeight) {
        // Move
        x += dx;
        y += dy;

        // Bounce X
        if (x <= 0 || x + size >= canvasWidth) {
            dx = -dx;
            // Add a little randomness on bounce
            zwasm_printf("Bounce X! Speed: %f", dx);
        }

        // Bounce Y
        if (y <= 0 || y + size >= canvasHeight) {
            dy = -dy;
            zwasm_printf("Bounce Y! Speed: %f", dy);
        }
    }

    void draw() const {
        zwasm_fill_style(color);
        zwasm_fill_rect(x, y, size, size);
    }
    
    // Simple interactivity
    void speedUp() {
        dx *= 1.1f;
        dy *= 1.1f;
    }
};

// Global instance (standard pattern for Wasm modules)
static BouncingBox player(100.0f, 100.0f, "#FF5733");

// -----------------------------------------------------------------------------
// C Exports - These are the functions JavaScript will call
// -----------------------------------------------------------------------------
extern "C" {

    // Called once when Wasm loads
    ZWASM_EXPORT int main() {
        zwasm_mem_init(nullptr, 0); // Setup heap
        zwasm_printf("C++ Game Initialized");
        zwasm_dom_set_html("status", "Running C++ Engine");
        return 0;
    }

    // Called every animation frame (e.g., 60fps)
    ZWASM_EXPORT void on_frame() {
        // 1. Clear Screen
        zwasm_fill_style("#1a1a1a"); 
        zwasm_fill_rect(0, 0, 800, 600);

        // 2. Handle Input (using zwasm input system)
        if (zwasm_key_down(32)) { // Spacebar
            player.speedUp();
        }

        // 3. Update & Draw C++ Object
        player.update(800, 600);
        player.draw();
    }

}

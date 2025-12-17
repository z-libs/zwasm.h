
#define ZWASM_IMPLEMENTATION
#include "zwasm.h"

static float x = 10.0f, y = 10.0f;
static float dx = 4.0f, dy = 4.0f;
static float size = 50.0f;
static float width = 600.0f, height = 400.0f;

ZWASM_EXPORT
int main(void) 
{
    // Setup the HTML structure.
    zwasm_dom_set_html("app", 
        "<div style='width:600px; height:400px; border:2px solid #555; position:relative; background:#222;'>"
            "<div id='box' style='width:50px; height:50px; background:#f00; position:absolute; border-radius:50%; box-shadow: 0 0 10px #f00;'></div>"
        "</div>"
        "<p style='color:#888'>Logic running in C. Rendering synced via Exports.</p>"
    );
    zwasm_log("Bouncer initialized.");
    return 0;
}

// Called every frame.
ZWASM_EXPORT
void on_frame(void)
{
    x += dx; 
    y += dy;

    // Bounce.
    if (x <= 0 || x + size >= width) 
    {
        dx = -dx;
        zwasm_eval("document.getElementById('box').style.borderColor = 'white';"); // Flash effect.
    }
    if (y <= 0 || y + size >= height) 
    {
        dy = -dy;
    }
}

// Getters for JS to render.
ZWASM_EXPORT float get_x(void) 
{ 
    return x; 
}

ZWASM_EXPORT float get_y(void) 
{ 
    return y; 
}


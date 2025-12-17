
# zwasm.h

`zwasm.h` is a minimal, header-only WebAssembly interop layer for C/C++. Unlike heavy toolchains like Emscripten that bundle megabytes of glue code and filesystem emulation, `zwasm.h` is designed for "bare metal" Wasm development.

It provides a lightweight `libc` replacement (malloc, printf, strings) and direct bindings to the Browser's DOM and Canvas API, allowing you to write tiny high-performance WebAssembly modules with zero external dependencies.

It is part of the [Zen Development Kit](https://github.com/z-libs/zdk).

## Features

* **Freestanding**: Designed to work with `-ffreestanding` and `-nostdlib`.
* **Zero Bloat**: Replaces standard `libc` with a minimal internal implementation (malloc, printf, string.h).
* **Direct JS Interop**: Call JavaScript `eval()`, modify the DOM, or log to console directly from C.
* **Canvas 2D**: Built-in wrappers for high-performance 2D graphics rendering.
* **Input System**: Simple keyboard event handling.
* **Header Only**: Drop it in and compile.
* **Memory Agnostic**: Includes a bump-pointer allocator that works with the standard Wasm heap symbols.
* **Cross-Platform**: Compiles on host (Linux/Windows) using standard `stdio` for easy debugging before deployment.

## Installation

### Manual

1.  Copy `zwasm.h` to your project's include folder.
2.  (Optional) Combine with other ZDK libraries for a full minimal OS experience.

### Clib

If you use the clib package manager, run:

```bash
clib install z-libs/zwasm.h
```

### ZDK (Recommended)

If you use the Zen Development Kit, it is included automatically by including `<zdk/zwasm.h>` (or `<zdk/zworld.h>`).

## Usage: C

For C projects, you define the implementation in one file and use the API to interact with the host browser.

```c
#define ZWASM_IMPLEMENTATION
#include "zwasm.h"

// Define a function we will call from JavaScript.
ZWASM_EXPORT void on_frame() 
{
    // Clear screen.
    zwasm_fill_style("#222");
    zwasm_fill_rect(0, 0, 800, 600);

    // Draw something.
    zwasm_fill_style("#00FF00");
    zwasm_fill_rect(100, 100, 50, 50);

    // Log to DevTools.
    zwasm_printf("Frame rendered! Time: %f", zwasm_time_now());
}

int main() 
{
    // Initialize the memory allocator (uses &__heap_base).
    zwasm_mem_init(NULL, 0);

    // Manipulate DOM
    zwasm_dom_set_html("status", "<b>Wasm Loaded</b>");
    
    return 0;
}
```

## Usage: C++

`zwasm.h` includes `extern "C"` guards, making it fully compatible with C++ projects. You can use it to drive C++ logic while keeping the binary size tiny.

```cpp
#include "zwasm.h"

class Game 
{
public:
    void update() 
    {
        if (zwasm_key_down(39)) // Right arrow.
        {
            x += 1.5f;
        }
    }
    
    void draw() 
    {
        zwasm_fill_style("red");
        zwasm_fill_rect(x, 10.0f, 20.0f, 20.0f);
    }
private:
    float x = 0;
};

Game g;

extern "C" ZWASM_EXPORT void frame() 
{
    g.update();
    g.draw();
}
```

## Compilation Guide

Since `zwasm.h` replaces the standard library, you must compile with flags that disable the default system environment.

**Clang / LLVM (Recommended)**

```bash
clang --target=wasm32 -nostdlib -Wl,--no-entry -Wl,--export-dynamic \
      -o test_wasm.wasm main.c
```

> `--target=wasm32`: Target WebAssembly.
> `-nostdlib`: Do not link against system libc.
> `-Wl,--no-entry`: We don't need a standard `_start` function (we export our own).
> `-Wl,--export-dynamic`: Ensure our `ZWASM_EXPORT` functions are visible to JS.

## The JavaScript "Glue"

Because `zwasm.h` imports functions from the environment (like `js_log` or `js_canvas_rect`), you must provide these imports when loading the Wasm file in your HTML.

**Minimal Loader Example:**

```javascript
const imports = {
    env: {
        // Console and system.
        js_log: (ptr, len) => console.log(readString(ptr, len)),
        js_time: () => performance.now(),
        js_rand: () => Math.random(),
        js_eval: (ptr, len) => eval(readString(ptr, len)),

        // Canvas API.
        js_canvas_clear: () => ctx.clearRect(0, 0, 800, 600),
        js_canvas_style: (ptr, len) => ctx.fillStyle = readString(ptr, len),
        js_canvas_rect: (x, y, w, h) => ctx.fillRect(x, y, w, h),
    }
};

// Helper to read C strings from Wasm Memory.
function readString(offset, length) {
    const bytes = new Uint8Array(wasmMemory.buffer, offset, length);
    return new TextDecoder('utf8').decode(bytes);
}

// Load.
WebAssembly.instantiateStreaming(fetch('game.wasm'), imports)
    .then(obj => {
        // ... Start your app
    });
```

## API Reference

`zwasm.h` groups functionality into logic blocks.

**System & Utils**

| Function | Description |
| :--- | :--- |
| `zwasm_log(msg)` | Logs a string to the browser console. |
| `zwasm_printf(fmt, ...)` | Minimal printf implementation. Supports `%d`, `%f`, `%s`. |
| `zwasm_time_now()` | Returns high-precision time (ms) via `performance.now()`. |
| `zwasm_random()` | Returns a float between 0.0 and 1.0. |

**DOM & Scripting**

| Function | Description |
| :--- | :--- |
| `zwasm_eval(js_code)` | Executes a raw JavaScript string. |
| `zwasm_dom_set_html(id, html)` | Sets `innerHTML` of the element with the given ID. |

**Canvas 2D Graphics**

| Function | Description |
| :--- | :--- |
| `zwasm_fill_style(color)` | Sets active color (e.g., "#FF0000" or "blue"). |
| `zwasm_fill_rect(x,y,w,h)` | Draws a filled rectangle. |
| `zwasm_clear_canvas()` | Clears the entire canvas. |

**Input System**

| Function | Description |
| :--- | :--- |
| `zwasm_key_down(code)` | Returns `true` if the key code (JS standard) is currently held. |
| `zwasm_on_key(code, down)` | **Internal**: Call this from JS `onkeydown`/`onkeyup` to update state. |

**Memory Management**

When running in bare-metal mode (`Z_IS_BARE_WASM`), these functions use a bump-pointer allocator starting at the linker-provided `__heap_base`.

| Function | Description |
| :--- | :--- |
| `zwasm_mem_init(p, sz)` | Initializes the heap (arguments ignored in bare mode, uses symbols). |
| `zwasm_malloc(sz)` | Allocates memory. |
| `zwasm_free(ptr)` | No-op in the simple bump allocator (warning: memory leaks if overused). |
| `zwasm_realloc(p, sz)` | Simple resize implementation (alloc + memcpy). |

## Macros

* `ZWASM_EXPORT`: Marks a function to be exported to JavaScript.
* `ZWASM_IMPORT(mod, name)`: Declares a function expected to be provided by JavaScript.

## Notes

### Bare Metal vs Host

If you compile this code with a standard C compiler (GCC/Clang on Linux/Windows), `zwasm.h` will detect it (`Z_IS_HOST`) and route all calls to standard `stdio.h` and `stdlib.h` (for example, `zwasm_printf` calls `vprintf`). This allows you to test your logic natively before compiling to Wasm.

### Memory Allocator

The internal allocator is a "Bump Pointer" allocator. It is extremely fast but **cannot free memory**. `zwasm_free` is a no-op in bare-metal mode. This is intended for small games/apps where you allocate generic buffers at startup. If you need complex memory management, you should link a real allocator (like `dlmalloc`) or implement a free-list.
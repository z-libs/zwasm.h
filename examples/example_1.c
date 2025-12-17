
#define ZWASM_IMPLEMENTATION
#include "zwasm.h"

ZWASM_EXPORT
int main(void) 
{
    zwasm_log("=> DOM interaction");

    zwasm_dom_set_html("app", "<h1>C says Hello!</h1><p>I wrote this HTML from WebAssembly.</p>");

    zwasm_eval("document.getElementById('app').style.color = '#aaffaa';");
    zwasm_eval("document.getElementById('app').style.border = '2px dashed #00ff00';");
    zwasm_eval("document.getElementById('app').style.padding = '20px';");

    zwasm_log("DOM updated successfully.");
    return 0;
}

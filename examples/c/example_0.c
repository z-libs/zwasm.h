
#define ZWASM_IMPLEMENTATION
#include "zwasm.h"

ZWASM_EXPORT
int main(void) 
{
    zwasm_log("=> Hello");
    
    zwasm_printf("Integer: %d", 42);
    zwasm_printf("Float:   %.4f", 3.14159f);
    zwasm_printf("String:  %s", "Bare Metal WASM");
    
    float r = zwasm_random();
    zwasm_printf("Random:  %f", r);
    
    return 0;
}


// compiler_types
#define __section(S)		__attribute__((__section__(#S)))
#define __aligned(x)		__attribute__((aligned(x)))

// sections
#define __init              __section(.init)
#define __serial            __section(.serial)
#define __data              __section(.data)
#define __rodata            __section(.rodata)
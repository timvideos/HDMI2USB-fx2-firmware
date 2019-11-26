#ifndef ENDPOINTS_H
#define ENDPOINTS_H

// Configuration of large endpoints (2,4,6,8)
// EP1 and EP0 are different than those so there is no sense to define them this way

// endpoint numbers
#define EP_CDC_HOST2DEV        2
#define EP_CDC_DEV2HOST        4
#define EP_UVC                 6

// helpers for constructing register names
// https://stackoverflow.com/a/1489985
#define MACRO_PASTER(x, y)    x ## y
#define MACRO_EVALUATOR(x, y) MACRO_PASTER(x, y)

// macros for endpoint registers access
// looks scary, but the usage is:
//   `EP_CDC_HOST2DEV_(CFG)` -> `EP2CFG`
#define EP_CDC_HOST2DEV_(tail) MACRO_EVALUATOR(MACRO_EVALUATOR(EP, EP_CDC_HOST2DEV), tail)
#define EP_CDC_DEV2HOST_(tail) MACRO_EVALUATOR(MACRO_EVALUATOR(EP, EP_CDC_DEV2HOST), tail)
#define EP_UVC_(tail)          MACRO_EVALUATOR(MACRO_EVALUATOR(EP, EP_UVC), tail)

#endif /* ENDPOINTS_H */

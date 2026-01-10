## Implicit Casts
If there is no general information loss, casts can be implicit. So implicit cast are allowed if we can go unsigned -> signed, integer -> float, and less -> more bits. However, keep in mind that not all unsigned values will fit in a signed type and putting integer in a flot may also incur loss of information.

From `bool`: No implicit casting

From `u8`: `u16`, `u32`, `u64`, `s8`, `s16`, `s32`, `s64`, `f32`, `f64`, `a`

From `u16`: `u32`, `u64`, `s16`, `s32`, `s64`, `f32`, `f64`, `a`

From `u32`: `u64`, `s32`, `s64`, `f32`, `f64`, `a`

From `u64`: `s64`, `f32`, `f64`, `a`

From `s8`: `s16`, `s32`, `s64`, `f32`, `f64`

From `s16`: `s32`, `s64`, `f32`, `f64`

From `s32`: `s64`, `f32`, `f64`

From `s64`: `f32`, `f64`

From `f32`: `f64`

From `f64`: No implicit casting

From `a`: No implicit casting

From `ptr<>`: No implicit casting

From `data<>`: `data<>` If the subtype can be cast implicitly

From `blob<>`: No implicit casting

From composites:
- `ptr<>`: If the element can be cast implicitly to `a`
- `data<>`: If each element can be cast implicitly to specified subtype
- `blob<>`: If elements match and each in order can be cast implicitly

## Explicit Casts
Any numeric type can be cast to any other numeric type. Negative number cast to unsigned will be set to 0. Exceeding values will be capped to minimum/maximum values.

From `bool`: No casting

From `u?`, `s?`, `f?`, `a`: Any of these can be cast between each other. Loss of information may happen .

From `a`: Additionally to the above, can be cast to any `ptr<>` type

From `ptr<>`: No casting

From `data<>`: Can be cast to any other `data<>` given that the subtypes can be also cast

From `blob<>`: No casting
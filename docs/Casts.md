## Implicit Casts
If there is no general information loss, casts can be implicit. So implicit cast are allowed if we can go unsigned -> signed, integer -> float, and less -> more bits. However, keep in mind that not all unsigned values will fit in a signed type and putting integer in a flot may also incur loss of information.

From `bool`: No implicit casting

From `u8`: `u32`, `u64`, `s8`, `s32`, `s64`, `f32`, `f64`

From `u32`: `u64`, `s32`, `s64`, `f32`, `f64`

From `u64`: `s64`, `f32`, `f64`

From `s8`: `s32`, `s64`, `f32`, `f64`

From `s32`: `s64`, `f32`, `f64`

From `s64`: `f32`, `f64`

From `f32`: `f64`

From `f64`: No implicit casting

From `a`: No implicit casting

From `ptr`: No implicit casting

From `data`: `data` If the subtype can be cast implicitly

From `blob`: No implicit casting

## Explicit Casts
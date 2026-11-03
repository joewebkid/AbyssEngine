# FBOContainer::Create filter argument

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The exported `FBOContainer::Create(int, int, bool, bool)` begins at
`0x7f8d4` and spans 292 bytes. The original instructions copy `r3`
to `r5` at `0x7f8e4`, then branch on `r5` at `0x7f938`. Under the ARM
calling convention, `r3` holds the first boolean argument after
`this`, width and height. The last boolean is passed on the stack and
is never read by this function. The original calls both
`glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter)` and
`glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter)` within
each branch; true selects `GL_LINEAR` and false selects `GL_NEAREST`.
The remaining texture allocation, color/depth attachment, status check
and framebuffer restore calls are unchanged.

The reconstruction had used the last boolean to select the filter,
which inverted the effective behavior of the restored `InitGL` call
`Create(width, height, true, false)`. It also hoisted the second
`glTexParameteri` outside the branch. The code now uses the first
boolean, ignores the last one, and keeps both calls in each branch.
The refractive FBO therefore requests linear filtering; the
`SetPostEffect` call with `false, true` requests nearest filtering.

Focused ARM verification on the prechange object gave `Create` **97.4%
source-shape / 91.9% strict fuzzy**, nonexact. Duplicating the filter
calls within their branches raised it to **98.9% / 96.6%**, still
nonexact. Reading the original argument register and switching the
source parameter then raised it to **100% / 100% linked-exact**. The
focused ARM build compiled **209/209** units. Native Windows and ARM64
static builds pass. The full corpus compared **4,323/4,524** originals
in **4,762** pairs at **96.24%** mean source-shape and **86.00%**
strict fuzzy, with **2,742 linked-exact**, **1,096 byte-exact**, 201
missing and zero skipped. Against the previous `InitGL` report, the
comparison-pair and byte-exact sets have zero gains or losses, and
`Create` is the only linked-exact gain. No exact match was lost.

The original function does not inspect the framebuffer-status return
value. Live GLES framebuffer completeness and visible refractive
output remain unverified. Redmi testing remains stopped by user request.
Next, inspect the FBO release and capture lifecycle against the original
and test visible output on a permitted host when available.

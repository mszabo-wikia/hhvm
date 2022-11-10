Maintains a running list of github.com/facebook/hhvm commits cherry-picked into this release.

| upstream SHA | commit summary |
|--------------|----------------|
| none | de8142321e5dd27afa7f80624ea75da09fbd6b14 is a Slack patch to enable us to compile without USE_JSONC while still retaining permissive json_decode behaviors |
| none | 231f29dd514da731e7c4729c8e3e7f53aecd6724 is a Slack patch to make json_decode return darrays or varrays by default, not just darrays |
| none | 7c2d6ddb71841195a60d8bf58d525a25e0a55b1b is a Slack patch to make this build succeed, fixing some build errors related to folly. These may be upstreamed, see discussion [here](https://hacklang.slack.com/archives/CAVPCQUQK/p1633021894009300) |
| none | 32e294ad9c4d566f2ceeecdde156a16b323e6070 is a Slack patch necessary to build our extensions. The CXX bridge used to reference Rust code in C++ is difficult to make work in extension builds, and those extensions import runtime-option.h. By referencing the codegen results of CXX instead of the source, we're able to import that file and build our extensions. |

Maintains a running list of github.com/facebook/hhvm commits cherry-picked into this release.

| upstream SHA | commit summary |
|--------------|----------------|
| none | 1b22de6b8556461653a2f01112e1a396d35960db is a Slack patch to enable us to compile without USE_JSONC while still retaining permissive json_decode behaviors |
| none | 1e96a2038076fcac01d3340077399e2516d8bbfb is a Slack patch to make json_decode return darrays or varrays by default, not just darrays |
| none | 43ef1345d821c6026e2b8ba8734dc5294316953a is a Slack patch to make this build succeed, fixing some build errors related to folly. These may be upstreamed, see discussion [here](https://hacklang.slack.com/archives/CAVPCQUQK/p1633021894009300) |
| none | 3d1ce05c2eff069c6a96700fb75f1e0c312d514d is a Slack patch necessary to build our extensions. The CXX bridge used to reference Rust code in C++ is difficult to make work in extension builds, and those extensions import runtime-option.h. By referencing the codegen results of CXX instead of the source, we're able to import that file and build our extensions. |
| none | 8d00c3f0d5e1cb09ff7834f4363a20bd3b116bc3 is a Slack patch necessary to add an AdminServer endpoint that exposes all internal counters. It is needed for our observability inside HHVM and its extensions (EscapeHatch). |
| none | PR #21 is a Slack patch necessary to add missing metrics to the Proxygen server. |

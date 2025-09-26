option(ENABLE_EXTENSION_MCROUTER "Build the mcrouter extension" ${ENABLE_MCROUTER})

HHVM_DEFINE_EXTENSION("mcrouter"
  SOURCES
    ext_mcrouter.cpp
  SYSTEMLIB
    ext_mcrouter.php
  DEPENDS
    libmcrouter
    varENABLE_MCROUTER
)

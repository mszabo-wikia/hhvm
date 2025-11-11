HHVM_DEFINE_EXTENSION("watcher" IMPLICIT
  SOURCES
    ext_watcher.cpp
  SYSTEMLIB
    ext_watcher.php
  DEPENDS
    libwatchmanclient
)

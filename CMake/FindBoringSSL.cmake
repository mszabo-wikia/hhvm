find_path(
  BORINGSSL_INCLUDE_DIR boringssl/ssl.h
  PATHS /build/target/boringssl/install/include
)

find_library(BORINGSSL_CRYPTO_LIBRARY
  NAMES
    bsslcrypto
  PATHS
    /build/target/boringssl/install/lib
)

if (BORINGSSL_INCLUDE_DIR AND BORINGSSL_CRYPTO_LIBRARY)
  set(BORINGSSL_FOUND TRUE)
else ()
  set(BORINGSSL_FOUND FALSE)
endif ()

if (BORINGSSL_FOUND)
  message(STATUS "Found BoringSSL: ${BORINGSSL_CRYPTO_LIBRARY}")
else ()
  message(STATUS "BoringSSL NOT found.")
endif ()

mark_as_advanced(BORINGSSL_CRYPTO_LIBRARY BORINGSSL_INCLUDE_DIR)

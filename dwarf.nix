{ lib, stdenv, fetchurl, libelf, zlib }:

stdenv.mkDerivation rec {
  pname = "libdwarf";
  version = "20210528";
  url = "https://www.prevanders.net/libdwarf-${version}.tar.gz";
  hash = "sha512-4PnIhVQFPubBsTM5YIkRieeCDEpN3DArfmN1Skzc/CrLG0tgg6ci0SBKdemU//NAHswlG4w7JAkPjLQEbZD4cA==";
  buildInputs = [ zlib libelf ];
  knownVulnerabilities = [ "CVE-2022-32200" "CVE-2022-39170" ];

  src = fetchurl {
    inherit url hash;
  };

  configureFlags = [ "--enable-shared" "--disable-nonshared" ];

  outputs = [ "bin" "lib" "dev" "out" ];

  meta = {
    homepage = "https://github.com/davea42/libdwarf-code";
    platforms = lib.platforms.unix;
    license = lib.licenses.lgpl21Plus;
    maintainers = [ lib.maintainers.atry ];
    inherit knownVulnerabilities;
  };
}

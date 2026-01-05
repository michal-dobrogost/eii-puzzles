with import <nixpkgs> {};
let
  stdenv = gccStdenv;
  #stdenv = clangStdenv;
in
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ cmake ]; # build time
  buildInputs = []; # runtime
}

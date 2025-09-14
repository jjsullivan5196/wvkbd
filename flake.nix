{
  description = "Flake for wvkbd fork with compact layout";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
        packages.default = pkgs.stdenv.mkDerivation rec {
          pname = "wvkbd-compact";
          version = "0.18";

          src = self;

          postPatch = ''
            substituteInPlace Makefile \
              --replace-fail "pkg-config" "$PKG_CONFIG"
          '';

          nativeBuildInputs = [
            pkgs.pkg-config
            pkgs.scdoc
            pkgs.wayland-scanner
          ];

          buildInputs = [
            pkgs.cairo
            pkgs.glib
            pkgs.harfbuzz
            pkgs.libxkbcommon
            pkgs.pango
            pkgs.wayland
          ];

          installFlags = [ "PREFIX=$(out)" ];

          strictDeps = true;

          meta = with pkgs.lib; {
            homepage = "https://github.com/sgroez/wvkbd-compact";
            description = "Fork of wvkbd software keyboard with custom compact layout";
            platforms = platforms.linux;
            license = licenses.gpl3Plus;
            mainProgram = "wvkbd-compact";
          };
        };

        defaultPackage = self.packages.${system}.default;
      });
}


{
  description = "Arduino dev environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      perSystem =
        { pkgs, ... }:
        {
          devShells.default =
            with pkgs;
            mkShell {
              name = "arduino-dev-shell";

              buildInputs = [
                nixd
                nil

                platformio
                python3

                clang-tools
                #glibc
              ];

              shellHook = ''
                export PLATFORMIO_CORE_DIR=$PWD/.pio
                echo "Activated arduino dev shell"
              '';
            };
        };
    };

}

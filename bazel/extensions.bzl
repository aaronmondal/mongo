"""Third party dependencies."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _mongo_dependencies_impl(_):
    http_archive(
        name = "local-remote-execution",
        urls = [
            "https://github.com/TraceMachina/nativelink/archive/03841cc340816058363d7a2958d0dbc31113c1de.zip",
        ],
        # integrity = "sha256-L+I3608IU4mDSqLWJ6zJV5zA17zGVay8c8TvmLkoneY=",
        # Note: Keep this in sync with `flake.nix` and `devtools/up.sh`.
        strip_prefix = "nativelink-03841cc340816058363d7a2958d0dbc31113c1de",
    )

mongo_dependencies = module_extension(
    implementation = _mongo_dependencies_impl,
)

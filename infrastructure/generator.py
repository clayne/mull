#!/usr/bin/env python3

import argparse
import pystache
import os
import shutil

SUPPORTED_PLATFORMS = {
    "ubuntu": {"20.04": [12], "22.04": [13, 14, 15], "24.04": [14, 15, 16, 17, 18]},
    "macos": {"latest": [14, 15, 16, 17, 18]},
}


def cmake(args):
    os_specific_args = {
        "ubuntu": {
            "CC": f"clang-{args.llvm_version}",
            "CXX": f"clang++-{args.llvm_version}",
            "CMAKE_PREFIX_PATH": f"/usr/lib/llvm-{args.llvm_version}/cmake/;/usr/lib/cmake/clang-{args.llvm_version}/",
        },
        "macos": {
            "CC": f"/opt/homebrew/opt/llvm@{args.llvm_version}/bin/clang",
            "CXX": f"/opt/homebrew/opt/llvm@{args.llvm_version}/bin/clang++",
            "CMAKE_PREFIX_PATH": f"/opt/homebrew/opt/llvm@{args.llvm_version}/lib/cmake/llvm/;/opt/homebrew/opt/llvm@{args.llvm_version}/lib/cmake/clang/",
        },
    }
    template_name = f"infrastructure/templates/cmake-presets/CMakePresets.json.mustache"
    template_args = os_specific_args[args.os]
    template_args["LLVM_VERSION"] = args.llvm_version
    template_args["OS_NAME"] = args.os
    renderer = pystache.Renderer(missing_tags="strict")
    with open(template_name, "r") as t:
        result = renderer.render(t.read(), template_args)
        with open("CMakePresets.json", "w") as f:
            f.write(result)


def devcontainers(args):
    shutil.rmtree(".devcontainer")
    for os_name, platform in SUPPORTED_PLATFORMS.items():
        if os_name.lower() == "macos":
            continue
        template_folder = f"infrastructure/templates/devcontainers/{os_name}"
        for os_version, llvm_versions in platform.items():
            for llvm_version in llvm_versions:
                container_folder = (
                    f".devcontainer/{os_name}_{os_version}_{llvm_version}"
                )
                os.makedirs(container_folder, exist_ok=True)

                template_args = {
                    "LLVM_VERSION": llvm_version,
                    "OS_NAME": os_name,
                    "OS_VERSION": os_version,
                }

                renderer = pystache.Renderer(missing_tags="strict")
                for template in ["devcontainer.json", "Dockerfile"]:
                    template_name = f"{template_folder}/{template}.mustache"
                    result_filename = f"{container_folder}/{template}"
                    with open(template_name, "r") as t:
                        result = renderer.render(t.read(), template_args)
                        with open(result_filename, "w") as f:
                            f.write(result)


def gh_workflows(args):
    template_folder = f"infrastructure/templates/github-actions/"
    workflow_folder = f".github/workflows/"

    for os_name in SUPPORTED_PLATFORMS.keys():
        strategies = []
        for os_version in sorted(SUPPORTED_PLATFORMS[os_name].keys()):
            for llvm_version in SUPPORTED_PLATFORMS[os_name][os_version]:
                arg = {
                    "OS_NAME": os_name,
                    "OS_VERSION": os_version,
                    "LLVM_VERSION": llvm_version,
                }
                strategies.append(arg)

        template_args = {"strategy": strategies, "OS_NAME": os_name.capitalize()}
        renderer = pystache.Renderer(missing_tags="strict")
        for template in [f"ci-{os_name}.yml"]:
            template_name = f"{template_folder}/{template}.mustache"
            result_filename = f"{workflow_folder}/{template}"
            with open(template_name, "r") as t:
                result = renderer.render(t.read(), template_args)
                with open(result_filename, "w") as f:
                    f.write(result)


def main():
    parser = argparse.ArgumentParser(
        prog="generator", description="Generates various infra files for Mull"
    )
    subparsers = parser.add_subparsers(
        title="subcommands", help="available subcommands", dest="cmd"
    )

    parser_cmake = subparsers.add_parser("cmake", help="Generates CMake preset file")
    parser_cmake.add_argument(
        "--os",
        choices=("ubuntu", "macos"),
        help="Select OS for which to generate preset",
        type=str,
    )
    parser_cmake.add_argument("--llvm_version", type=int)

    subparsers.add_parser("devcontainers", help="Generates devcontainer files")
    subparsers.add_parser("github_workflows", help="Generates GitHub workflow files")
    subparsers.add_parser("all", help="Combines devcontainers and github_workflows")

    args = parser.parse_args()
    if args.cmd == "cmake":
        cmake(args)
    if args.cmd == "devcontainers":
        devcontainers(args)
    if args.cmd == "github_workflows":
        gh_workflows(args)
    if args.cmd == "all":
        gh_workflows(args)
        devcontainers(args)


if __name__ == "__main__":
    main()

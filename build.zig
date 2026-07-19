const std = @import("std");

const cflags = [_][]const u8{ "-O3", "-std=c11" };

fn cModule(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    src: []const u8,
) *std.Build.Module {
    const mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    mod.addIncludePath(b.path("src/include"));
    mod.addCSourceFiles(.{ .files = &.{src}, .flags = &cflags });
    return mod;
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{ .preferred_optimize_mode = .ReleaseFast });

    const lib = b.addLibrary(.{
        .name = "areion",
        .root_module = cModule(b, target, optimize, "src/areion.c"),
        .linkage = .static,
    });
    b.installArtifact(lib);
    b.installDirectory(.{
        .install_dir = .header,
        .install_subdir = "",
        .source_dir = b.path("src/include"),
    });

    const tests = b.addExecutable(.{
        .name = "test-areion",
        .root_module = cModule(b, target, optimize, "src/test/main.c"),
    });
    tests.root_module.linkLibrary(lib);
    const run_tests = b.addRunArtifact(tests);
    const test_step = b.step("test", "Run AREION tests");
    test_step.dependOn(&run_tests.step);

    const benchmark = b.addExecutable(.{
        .name = "benchmark",
        .root_module = cModule(b, target, optimize, "src/test/benchmark.c"),
    });
    benchmark.root_module.linkLibrary(lib);
    b.installArtifact(benchmark);
}

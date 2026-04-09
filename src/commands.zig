const std = @import("std");
const Allocator = std.mem.Allocator;
const utils = @import("utils.zig");
const templates = @import("templates.zig");

const GREEN = utils.GREEN;
const RED = utils.RED;
const RESET = utils.RESET;

/// Returns true if any element of `args` equals `needle`.
fn hasArg(args: []const []const u8, needle: []const u8) bool {
    for (args) |a| {
        if (std.mem.eql(u8, a, needle)) return true;
    }
    return false;
}

/// `nexus init [--lib]`  –  scaffold a new project in the current directory.
pub fn nexusInit(allocator: Allocator, args: []const []const u8) !void {
    const stdout = std.io.getStdOut().writer();

    try utils.nexusMkdir("src");
    try utils.nexusMkdir("include");
    try utils.nexusMkdir("build");
    try utils.nexusMkdir("bin");
    try utils.nexusMkdir("nexus_build");

    try utils.buildFile("nexus_build/user_templates.c", templates.all_templates);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " \"nexus_build/user_templates.c\" created.\n", .{});

    try utils.buildFile("include/templates.h", templates.header_template);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " \"include/templates.h\" created.\n", .{});

    try utils.buildFile("nexus_build/template_exe.txt", templates.build_template_executable);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " \"nexus_build/template_exe.txt\" created.\n", .{});

    try utils.buildFile("nexus_build/template_lib.txt", templates.build_template_library);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " \"nexus_build/template_lib.txt\" created.\n", .{});

    try utils.buildFile("nexus_build/color_codes.h", templates.color_codes_template);

    // Determine project type and name.
    const is_lib = hasArg(args, "--lib");
    const is_init = args.len >= 2 and std.mem.eql(u8, args[1], "init");

    // Write .nexus metadata file.
    const nex_file = std.fs.cwd().createFile(".nexus", .{}) catch {
        return error.FailedToOpen;
    };
    defer nex_file.close();
    const nex_writer = nex_file.writer();

    if (is_lib) {
        try utils.nexusMkdir("lib");
        try utils.nexusMkdir("examples");
        try utils.buildFile("src/lib.c", templates.lib_template);
        try nex_writer.writeAll("TargetType=library\n");
    } else {
        try utils.buildFile("src/main.c", templates.main_template);
        try nex_writer.writeAll("TargetType=executable\n");
    }

    try nex_writer.writeAll("CC=gcc\n");

    if (is_init) {
        try nex_writer.writeAll("ProjectName=DefaultName\n");
    } else if (args.len >= 3 and std.mem.eql(u8, args[1], "new")) {
        try nex_writer.print("ProjectName={s}\n", .{args[2]});
    } else {
        try nex_writer.writeAll("ProjectName=DefaultName\n");
    }

    try utils.addSrcFiles(allocator);
    try utils.buildFile(".gitignore", templates.gitignore_template);
    try utils.nexusGitInit(allocator);

    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Project initialized successfully.\n", .{});
}

/// `nexus new <project_name> [--lib]`  –  create a directory and scaffold inside it.
pub fn nexusNew(allocator: Allocator, project_name: []const u8, args: []const []const u8) !void {
    try utils.nexusMkdir(project_name);
    std.posix.chdir(project_name) catch {
        return error.FailedToCd;
    };
    try nexusInit(allocator, args);
}

/// `nexus build`  –  compile the project using the nexus_build scripts.
pub fn nexusBuild(allocator: Allocator) !void {
    const stdout = std.io.getStdOut().writer();

    try utils.addSrcFiles(allocator);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Added src files.\n", .{});

    const type_str = utils.getProjAttr(allocator, "TargetType=") catch
        try allocator.dupe(u8, "executable");
    defer allocator.free(type_str);

    const cc_name = utils.getProjAttr(allocator, "CC=") catch
        try allocator.dupe(u8, "gcc");
    defer allocator.free(cc_name);

    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Read .nexus file.\n", .{});

    if (std.mem.eql(u8, cc_name, "clang")) {
        if (std.mem.eql(u8, type_str, "executable")) {
            try utils.replaceWord(allocator, "nexus_build/template_exe.txt", "gcc", "clang");
        } else if (std.mem.eql(u8, type_str, "library")) {
            try utils.replaceWord(allocator, "nexus_build/template_lib.txt", "gcc", "clang");
        } else {
            return error.UnknownTarget;
        }
    }

    const template_path: []const u8 = if (std.mem.eql(u8, type_str, "executable"))
        "nexus_build/template_exe.txt"
    else if (std.mem.eql(u8, type_str, "library"))
        "nexus_build/template_lib.txt"
    else
        return error.FailedToRead;

    const template_file = std.fs.cwd().openFile(template_path, .{}) catch {
        return error.FailedToOpen;
    };
    defer template_file.close();

    const template_contents = try template_file.readToEndAlloc(allocator, 8192);
    defer allocator.free(template_contents);

    if (template_contents.len == 0) {
        try stdout.print(RED ++ "[nexus]" ++ RESET ++ " Template file is empty or unreadable\n", .{});
        return error.FailedToRead;
    }

    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Building build.c file.\n", .{});
    try utils.buildFile("nexus_build/build.c", template_contents);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Created build.c file.\n", .{});

    const compile_cmd = "gcc -std=c2x -Wall -Wextra -o nexus_build/build " ++
        "nexus_build/build.c nexus_build/user_templates.c";
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Executing: {s}\n", .{compile_cmd});
    try utils.runShell(allocator, compile_cmd);
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Execution successful\n", .{});

    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Running nexus_build/build\n", .{});
    try utils.runCommand(allocator, &[_][]const u8{"nexus_build/build"});
}

/// `nexus run`  –  rebuild then execute the project binary from `bin/`.
pub fn nexusRun(allocator: Allocator) !void {
    const stdout = std.io.getStdOut().writer();
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Executing project...\n", .{});

    const proj_name = try utils.getProjAttr(allocator, "ProjectName=");
    defer allocator.free(proj_name);

    const cmd = try std.fmt.allocPrint(allocator, "bin/{s}", .{proj_name});
    defer allocator.free(cmd);

    try utils.runShell(allocator, cmd);
}

/// `nexus export`  –  copy the built binary to the path stored in `.nexus`.
pub fn nexusPathExport(allocator: Allocator) !void {
    const stdout = std.io.getStdOut().writer();

    const proj_name = try utils.getProjAttr(allocator, "ProjectName=");
    defer allocator.free(proj_name);

    const export_path = utils.getProjAttr(allocator, "PATH=") catch {
        return error.FailedToGetPath;
    };
    defer allocator.free(export_path);

    const exe_path = try std.fmt.allocPrint(allocator, "bin/{s}", .{proj_name});
    defer allocator.free(exe_path);

    const cmd = try std.fmt.allocPrint(allocator, "cp {s} {s}", .{ exe_path, export_path });
    defer allocator.free(cmd);

    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Executing: {s}\n", .{cmd});

    utils.runShell(allocator, cmd) catch {
        // Fallback: copy to a temporary name then swap, matching original behaviour.
        const cmd2 = try std.fmt.allocPrint(allocator, "cp {s} {s}_", .{ exe_path, export_path });
        defer allocator.free(cmd2);
        try utils.runShell(allocator, cmd2);

        const rm_cmd = try std.fmt.allocPrint(
            allocator,
            "rm {s}{s}",
            .{ export_path, exe_path },
        );
        defer allocator.free(rm_cmd);
        try utils.runShell(allocator, rm_cmd);
    };

    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Successfully exported to PATH\n", .{});
}

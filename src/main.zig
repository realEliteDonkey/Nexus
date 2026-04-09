const std = @import("std");
const commands = @import("commands.zig");
const templates = @import("templates.zig");
const utils = @import("utils.zig");

const GREEN = utils.GREEN;
const RED = utils.RED;
const RESET = utils.RESET;
const ORANGE = "\x1b[1;33m";

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    const stdout = std.io.getStdOut().writer();
    const stderr = std.io.getStdErr().writer();

    // argv[0] is the program name; require at least one subcommand.
    if (args.len < 2 or args.len > 4) {
        try stderr.print("nexus has max FOUR arguments other than 'nexus' itself!\n", .{});
        std.process.exit(7); // ERR_NEX_ARG_COUNT
    }

    const cmd = args[1];

    if (std.mem.eql(u8, cmd, "new")) {
        if (args.len != 3 and args.len != 4) {
            try stderr.print("nexus 'new' command requires a project name!\n", .{});
            std.process.exit(7);
        }
        commands.nexusNew(allocator, args[2], args) catch |err| {
            try stderr.print(RED ++ "[nexus]" ++ RESET ++ " Failed to create new project: {s}\n", .{@errorName(err)});
            std.process.exit(1);
        };
    } else if (std.mem.eql(u8, cmd, "init")) {
        commands.nexusInit(allocator, args) catch |err| {
            try stderr.print(RED ++ "[nexus]" ++ RESET ++ " Failed to initialize project: {s}\n", .{@errorName(err)});
            std.process.exit(1);
        };
    } else if (std.mem.eql(u8, cmd, "build") and args.len == 2) {
        commands.nexusBuild(allocator) catch |err| {
            try stderr.print(RED ++ "[nexus]" ++ RESET ++ " Build failed: {s}\n", .{@errorName(err)});
            std.process.exit(1);
        };
    } else if (std.mem.eql(u8, cmd, "run") and args.len == 2) {
        commands.nexusBuild(allocator) catch |err| {
            try stderr.print(RED ++ "[nexus]" ++ RESET ++ " Failed to build before running: {s}\n", .{@errorName(err)});
            std.process.exit(1);
        };
        commands.nexusRun(allocator) catch |err| {
            try stderr.print(RED ++ "[nexus]" ++ RESET ++ " Run failed: {s}\n", .{@errorName(err)});
            std.process.exit(1);
        };
    } else if (std.mem.eql(u8, cmd, "export") and args.len == 2) {
        commands.nexusPathExport(allocator) catch |err| {
            try stderr.print(RED ++ "[nexus]" ++ RESET ++ " Export failed: {s}\n", .{@errorName(err)});
            std.process.exit(1);
        };
    } else if (std.mem.eql(u8, cmd, "help")) {
        if (args.len == 2) {
            try stdout.print(ORANGE ++ "{s}" ++ RESET, .{templates.help_msg});
        } else if (args.len > 2) {
            const sub = args[2];
            if (std.mem.eql(u8, sub, "init")) {
                try stdout.print(ORANGE ++ "{s}" ++ RESET, .{templates.help_init});
            } else if (std.mem.eql(u8, sub, "new")) {
                try stdout.print(ORANGE ++ "{s}" ++ RESET, .{templates.help_new});
            } else if (std.mem.eql(u8, sub, "build")) {
                try stdout.print(ORANGE ++ "{s}" ++ RESET, .{templates.help_build});
            } else if (std.mem.eql(u8, sub, "run")) {
                try stdout.print(ORANGE ++ "{s}" ++ RESET, .{templates.help_run});
            } else {
                try stdout.print(RED ++ "[nexus]" ++ RESET ++ " No such help command exists.\n", .{});
            }
        }
    } else {
        try stderr.print("No such nexus command exists!\n", .{});
        std.process.exit(6); // ERR_CMD_NOT_FOUND
    }
}

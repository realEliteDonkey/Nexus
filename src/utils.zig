const std = @import("std");
const Allocator = std.mem.Allocator;

pub const GREEN = "\x1b[0;32m";
pub const RED = "\x1b[0;31m";
pub const RESET = "\x1b[0m";

/// Creates a directory at `name`. Returns an error if creation fails.
pub fn nexusMkdir(name: []const u8) !void {
    const stdout = std.io.getStdOut().writer();
    std.fs.cwd().makeDir(name) catch {
        return error.MkdirFailed;
    };
    try stdout.print(GREEN ++ "[nexus]" ++ RESET ++ " Directory \"{s}\" created.\n", .{name});
}

/// Writes `content` to `file_name`, creating or truncating the file.
pub fn buildFile(file_name: []const u8, content: []const u8) !void {
    const file = std.fs.cwd().createFile(file_name, .{}) catch {
        return error.FailedToOpen;
    };
    defer file.close();
    file.writeAll(content) catch {
        return error.FailedToWrite;
    };
}

/// Replaces all occurrences of `old_word` with `new_word` in the given file.
pub fn replaceWord(
    allocator: Allocator,
    filename: []const u8,
    old_word: []const u8,
    new_word: []const u8,
) !void {
    const file = std.fs.cwd().openFile(filename, .{}) catch {
        return error.FailedToOpen;
    };
    defer file.close();

    const content = try file.readToEndAlloc(allocator, 1024 * 1024);
    defer allocator.free(content);

    var result = std.ArrayList(u8).init(allocator);
    defer result.deinit();

    var remaining = content;
    while (std.mem.indexOf(u8, remaining, old_word)) |pos| {
        try result.appendSlice(remaining[0..pos]);
        try result.appendSlice(new_word);
        remaining = remaining[pos + old_word.len ..];
    }
    try result.appendSlice(remaining);

    const out_file = std.fs.cwd().createFile(filename, .{}) catch {
        return error.FailedToOpen;
    };
    defer out_file.close();
    try out_file.writeAll(result.items);
}

/// Recursively writes all `.c` and `.cpp` files under `base_path/relative_path`
/// into `writer` as C string array entries.
pub fn writeSrcFilesRecursive(
    allocator: Allocator,
    writer: anytype,
    base_path: []const u8,
    relative_path: []const u8,
) !void {
    const full_path = if (relative_path.len == 0)
        try allocator.dupe(u8, base_path)
    else
        try std.fmt.allocPrint(allocator, "{s}/{s}", .{ base_path, relative_path });
    defer allocator.free(full_path);

    var dir = std.fs.cwd().openDir(full_path, .{ .iterate = true }) catch {
        return error.DirNotFound;
    };
    defer dir.close();

    var iter = dir.iterate();
    while (try iter.next()) |entry| {
        const entry_relative = if (relative_path.len == 0)
            try allocator.dupe(u8, entry.name)
        else
            try std.fmt.allocPrint(allocator, "{s}/{s}", .{ relative_path, entry.name });
        defer allocator.free(entry_relative);

        switch (entry.kind) {
            .directory => {
                try writeSrcFilesRecursive(allocator, writer, base_path, entry_relative);
            },
            .file => {
                if (std.mem.endsWith(u8, entry.name, ".c") or
                    std.mem.endsWith(u8, entry.name, ".cpp"))
                {
                    try writer.print("    \"src/{s}\",\n", .{entry_relative});
                }
            },
            else => {},
        }
    }
}

/// Generates `src_files.h` listing all `.c`/`.cpp` files found under `src/`.
pub fn addSrcFiles(allocator: Allocator) !void {
    // Verify src/ exists before writing
    var src_dir = std.fs.cwd().openDir("src", .{}) catch {
        return error.DirNotFound;
    };
    src_dir.close();

    const src_files = std.fs.cwd().createFile("src_files.h", .{}) catch {
        return error.FailedToOpen;
    };
    defer src_files.close();

    const writer = src_files.writer();

    try writer.writeAll("#ifndef SRC_FILES_H\n");
    try writer.writeAll("#define SRC_FILES_H\n\n");
    try writer.writeAll("const char* files[] = {\n");

    try writeSrcFilesRecursive(allocator, writer, "src", "");

    try writer.writeAll("    NULL\n");
    try writer.writeAll("};\n\n");
    try writer.writeAll("#endif\n");
}

/// Returns true if `git` is available on PATH.
pub fn gitInstalled(allocator: Allocator) bool {
    var child = std.process.Child.init(&[_][]const u8{ "git", "--version" }, allocator);
    child.stdout_behavior = .Ignore;
    child.stderr_behavior = .Ignore;
    const term = child.spawnAndWait() catch return false;
    return switch (term) {
        .Exited => |code| code == 0,
        else => false,
    };
}

/// Initialises a git repository in the current directory and creates an
/// initial commit.
pub fn nexusGitInit(allocator: Allocator) !void {
    const stdout = std.io.getStdOut().writer();

    if (!gitInstalled(allocator)) {
        try stdout.print(RED ++ "[nexus]" ++ RESET ++
            " Git is not installed. Please install Git to use this feature.\n", .{});
        return error.GitNotInstalled;
    }

    try runCommand(allocator, &[_][]const u8{ "git", "init" });
    try runCommand(allocator, &[_][]const u8{ "git", "add", "." });
    try runCommand(
        allocator,
        &[_][]const u8{ "git", "commit", "-m", "Initial NEXUS PROJECT commit!" },
    );
}

/// Spawns `argv` and waits for it to exit, returning an error on non-zero exit.
pub fn runCommand(allocator: Allocator, argv: []const []const u8) !void {
    var child = std.process.Child.init(argv, allocator);
    child.stdout_behavior = .Inherit;
    child.stderr_behavior = .Inherit;
    const term = try child.spawnAndWait();
    switch (term) {
        .Exited => |code| if (code != 0) return error.CommandFailed,
        else => return error.CommandFailed,
    }
}

/// Runs a shell command string via `sh -c`.
pub fn runShell(allocator: Allocator, cmd: []const u8) !void {
    try runCommand(allocator, &[_][]const u8{ "sh", "-c", cmd });
}

/// Reads the value of `key` (e.g. `"ProjectName="`) from the `.nexus` file.
/// The caller owns the returned slice and must free it.
pub fn getProjAttr(allocator: Allocator, key: []const u8) ![]u8 {
    const file = std.fs.cwd().openFile(".nexus", .{}) catch {
        return error.FailedToOpen;
    };
    defer file.close();

    const content = try file.readToEndAlloc(allocator, 4096);
    defer allocator.free(content);

    var lines = std.mem.splitScalar(u8, content, '\n');
    while (lines.next()) |line| {
        if (std.mem.startsWith(u8, line, key)) {
            const value = std.mem.trimRight(u8, line[key.len..], "\r\n");
            return allocator.dupe(u8, value);
        }
    }

    return error.AttributeNotFound;
}

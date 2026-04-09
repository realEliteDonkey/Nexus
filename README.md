# Nexus Project Scaffold

Nexus is a C project scaffolding and build automation tool written in [Zig](https://ziglang.org/). It helps you quickly initialize, build, and manage C projects with a simple command-line interface.

---

## Features

- **Project Initialization**: Scaffold a new C project with standard directories (`src`, `include`, `build`, `bin`, etc.).
- **Template Generation**: Generates template source, header, and build files.
- **Automated Build System**: Creates a build script to compile and link your project.
- **Source File Management**: Maintains a list of all source files for easy builds.
- **Git Integration**: Optionally initializes a Git repository and creates a `.gitignore`.
- **Colorful Output**: Uses color-coded terminal output for better readability.

---

## Getting Started

### Prerequisites

- [Zig](https://ziglang.org/download/) 0.13.0 or later

### 1. Build from Source

```sh
zig build -Doptimize=ReleaseSafe
```

The compiled binary will be placed in `zig-out/bin/nexus`.

### 2. Install the Nexus Binary

**On Linux/macOS:**
```sh
sudo cp zig-out/bin/nexus /usr/local/bin/
```

**On Windows:**
- Copy `zig-out/bin/nexus.exe` to a folder included in your system `PATH`.

**Verify installation:**
```sh
nexus help
```

---

### 3. Initialize a New Project

To create a new project in a subdirectory:
```sh
nexus new <project_name>
```

Or, to initialize in the current directory:
```sh
nexus init
```

For a library project:
```sh
nexus init --lib
```

---

### 4. Build the Project

```sh
nexus build
```

---

### 5. Run the Project

```sh
nexus run
```
> **Note:** `run` is only available for executable projects.

---

### 6. Export Binary

Copy the built binary to a specified path:
```sh
nexus export
```

---

## Project Structure

The Nexus source itself follows this layout:

```
.
├── build.zig           # Zig build script
├── src/
│   ├── main.zig        # Entry point & CLI argument parsing
│   ├── commands.zig    # init, new, build, run, export commands
│   ├── utils.zig       # File I/O, directory, git utilities
│   └── templates.zig   # Embedded template strings
```

A typical Nexus-managed C project looks like this:

```
.
├── bin/                # Compiled binaries
├── build/              # Object files and build artifacts
├── include/            # Header files
├── nexus_build/        # Build scripts and templates (auto-generated)
├── src/                # Source files
├── .nexus              # Project metadata
├── .gitignore
├── src_files.h         # Auto-generated list of source files
```

---

## Customization

- Edit templates in `nexus_build/` to change the default generated code.
- Add your own source files to `src/` and headers to `include/`.


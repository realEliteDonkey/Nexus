# Nexus Project Scaffold

Nexus is a C build system automation tool. It helps you quickly initialize, build, and manage C projects with a simple command-line interface.
Currently only tested for UNIX systems.

---

## Features

- **Project Initialization**: Scaffold a new C project with standard directories (`src`, `include`, `build`, `bin`, etc.).
- **Template Generation**: Generates template source, header, and build files.
- **Automated Build System**: Creates a build script to compile and link your project.
- **Source File Management**: Maintains a list of all source files for easy builds.
- **Git Integration**: Optionally initializes a Git repository and creates a `.gitignore`.
- **Colorful Output**: Uses color-coded terminal output for better readability.
- Auto PATH export.

---

## Getting Started

### 1. Download and Set Up the Nexus Executable

1. **Download** the precompiled `nexus` executable for your platform.
2. **Export it to your `PATH`** so you can run `nexus` from anywhere:

   **On Linux/macOS:**
   ```sh
   chmod +x nexus
   sudo mv nexus /usr/local/bin/
   ```

   **On Windows:**
   - Move `nexus.exe` to a folder included in your system `PATH` (e.g., `C:\Windows\System32` or another directory in your `PATH`).
   - Or, add the folder containing `nexus.exe` to your `PATH` environment variable.

3. **Verify installation:**
   ```sh
   nexus --help
   ```

---

### 2. Initialize a New Project

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

### 3. Build the Project

```sh
nexus build
```

---

### 4. Run the Project

```sh
nexus run
```
> **Note:** `run` is only available for executable projects.

---

### 5. Export Binary

Copy the built binary to a specified path:
```sh
nexus export
```

---

## Project Structure

A typical Nexus project looks like this:

```
.
├── bin/                # Compiled binaries
├── build/              # Object files and build artifacts
├── include/            # Header files
├── nexus_build/        # Build scripts and templates
├── src/                # Source files
├── .nexus              # Project metadata
├── .gitignore
├── src_files.h         # Auto-generated list of source files
```

---

## Customization

- Edit templates in `nexus_build/` to change the default generated code.
- Add your own source files to `src/` and headers to `include/`.

---

## Error Codes

See [`include/nex_error.h`](include/nex_error.h) for a list of error codes used throughout the project.


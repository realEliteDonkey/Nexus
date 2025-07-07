# 🚀 Nexus — Your Cross-Platform C/C++ Build Sidekick

> *“Build with flair, code with care — Nexus takes you there.”*

---

## What is Nexus?

Nexus is a lightweight, blazing-fast **build tool for C/C++ projects**, designed to get you coding — not configuring.  
It bootstraps your project structure, compiles your code, and runs executables — all from a single CLI command.  

Built for **Windows, macOS, and Linux**, Nexus makes managing your C/C++ builds a breeze.

---

## 🔥 Features

- `nexus init`  
  Creates essential project directories (`src/`, `lib/`, `bin/`),  
  scaffolds a `build.c` build script, a sample `src/main.c` with Hello World,  
  and prepares your workspace (Git init coming soon!).  

- `nexus build`  
  Compiles your `build.c` script into an executable and runs it —  
  automagically compiling and linking your source files.

- `nexus run`  
  Builds your project *and* runs the resulting executable in one command.

---

## 🎉 Getting Started

### 1. Download Nexus

Grab the latest `nexus.exe` from the [Releases](https://github.com/YOUR_USERNAME/YOUR_REPO/releases) page.

### 2. Install Nexus

- Move `nexus.exe` to a folder of your choice, e.g.,

        mkdir "$env:USERPROFILE\Tools\Nexus"
        mv nexus.exe "$env:USERPROFILE\Tools\Nexus\"

- Add the folder to your system `PATH` environment variable for easy access.

---

### 3. Create a new C/C++ project

        nexus init

This command sets up your project folders and starter files.

---

### 4. Build your project

        nexus build

This compiles and links your source code.

---

### 5. Run your project

        nexus run

Builds and runs your project executable.

---

## ⚙️ Under the Hood

- `nexus` uses a `build.c` script (auto-generated on init) to orchestrate your compilation.
- Uses `gcc` for compiling and linking — make sure it’s installed and accessible.
- Creates directories `src/`, `lib/`, and `bin/` for sources, libraries, and output executables.

---

## 🚧 Roadmap

- [ ] Auto-generate `.gitignore` and perform `git init` on `nexus init`.
- [ ] Auto-detect and add all source files to build process.
- [ ] Add Windows and Unix installer scripts.
- [ ] Support for C++ builds with `g++` (for now manual manipulation of build.c is required).
- [ ] Cross-platform binary releases.

---

## 📦 Requirements

- **gcc** compiler installed and available in your `PATH`.
- Windows, macOS, or Linux.
- Basic terminal/command prompt knowledge.

---

## 🙌 Contributing

Contributions, issues, and feature requests are welcome!  
Feel free to open issues or submit pull requests.

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for details.

---

## 👑 Stay hip, build fast, and code happy with Nexus!  
*— Your friendly neighborhood build tool*

---

### Example session:

        $ nexus init
        Directory "src" created.
        Directory "lib" created.
        Directory "bin" created.
        Scaffolded build.c and src/main.c

        $ nexus build
        Compiling and linking: gcc -o bin/main src/main.c
        Compilation and linking successful.

        $ nexus run
        Compiling and linking: gcc -o bin/main src/main.c
        Compilation and linking successful.
        Hello, World!


# CLIB Command-Line Tool Documentation

**CLIB** is a command-line tool written in **C++** that serves as a **package manager** for managing C/C++ libraries in your projects. It allows you to easily install, uninstall, connect, and manage library dependencies.

You can install libraries directly from **Git repositories** or use different **kits/packages** to include libraries in your project. This makes it flexible for both individual libraries and bundled sets of libraries.

The prototype was written in **Python**.

---

## Global Flags

| Flag               | Description                                |
| ------------------ | ------------------------------------------ |
| `--version` / `-v` | Shows the current CLIB version.            |
| `--help` / `-h`    | Show this help message and exit.           |
| `--force` / `-f`   | Executes the command without confirmation. |

---

## Commands

### init

Initializes CLIB.

```bash
clib init
```

### uninstall

Uninstalls a library. Supports `-f` flag.

```bash
clib uninstall <library_name> [-f]
```

### connect

Connects CLIB to your project. Supports `-a` flag.

```bash
clib connect <library_name> [-a]
```

#### Flags

| Flag           | Description                                |
| -------------- | ------------------------------------------ |
| `--all` / `-a` | Connects all libraries you have installed. |

### create

Creates a template based on the name (`CMakeLists.txt`, `info.yaml`).

```bash
clib create <template_name>
```

### install

Installs a library. Supports `-f` and `-I` flags.

```bash
clib install <library_name> [-f] [-I]
```

#### Flags

| Flag               | Description                                   |
| ------------------ | --------------------------------------------- |
| `-f`               | Executes the command without confirmation.    |
| `--InstDep` / `-I` | Installs dependencies along with the package. |

---

## How to install the kits?

**Note about kits:** Kits are not actually kits as you might think. In reality, they are libraries that depend on other libraries included in the set. Set names are usually written in capital letters, and the description indicates which "libraries" are included.

**Installation process is simple:** use the `install` function, enter the Git URL to the repository, and add the `-I` flag.

```bash
clib install "https://github.com/ZeleznaRuda/clib-mainkit.git" [-f] -I
```

---

### ZeleznaRuda - 2025


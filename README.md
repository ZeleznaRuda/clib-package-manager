# CLIBX Command-Line Tool Documentation

**CLIBX** is a command-line tool written in **C++** that serves as a **package manager** for managing C/C++ libraries in your projects. It allows you to easily install, uninstall, connect, and manage library dependencies.

You can install libraries directly from **Git repositories** or use different **kits/packages** to include libraries in your project. This makes it flexible for both individual libraries and bundled sets of libraries.

The prototype was written in **Python**.

---

## Global Flags

| Flag               | Description                                |
| ------------------ | ------------------------------------------ |
| `--version` / `-v` | Shows the current CLIBX version.           |
| `--help` / `-h`    | Show this help message and exit.           |
| `--force` / `-f`   | Executes the command without confirmation. |

---

## Commands

### init

Initializes CLIBX.

```bash
clibx init
```

### uninstall

Uninstalls a library. Supports `-f` flag.

```bash
clibx uninstall <library_name> [-f]
```

### connect

Connects CLIBX to your project. Supports `-a` flag.

```bash
clibx connect <library_name> [-a]
```

#### Flags

| Flag           | Description                                |
| -------------- | ------------------------------------------ |
| `--all` / `-a` | Connects all libraries you have installed. |

### create

Creates a template based on the name (`CMakeLists.txt`, `info.yaml`).

```bash
clibx create <template_name>
```

### install

Installs a library. Supports `-f` and `-I` flags.

```bash
clibx install <library_name> [-f] [-I]
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
clibx install "https://github.com/ZeleznaRuda/clibx-mainkit.git" [-f] -I
```

---

### ZeleznaRuda - 2025
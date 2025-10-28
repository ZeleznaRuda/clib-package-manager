# CLIBX Command-Line Tool Documentation

**CLIBX** is a command-line tool written in **C++** that serves as a **package manager** for managing C/C++ libraries in your projects.  
It allows you to easily **install**, **uninstall**, **connect**, **initialize**, and **manage** library dependencies.

Libraries can be installed directly from **Git repositories**, or you can use **kits** (sets of dependent libraries).  
The early prototype of CLIBX was written in **Python**.

---

## Flags

| Flag | Description |
|------|--------------|
| `--version`, `-v` | Shows the current CLIBX version. |
| `--help`, `-h` | Shows this help message and exits. |
| `--force`, `-f` | Executes the command without confirmation. |
| `--web`, `-w` | Opens the CLIBX website. |
| `--instDep`, `-I` | Installs dependencies along with the package. |
| `--all`, `-a` | Connects all libraries you have installed. |

---

## Commands

### init
Initializes CLIBX.

```bash
clibx init
````

### template

Creates a project template based on the given name.

```bash
clibx template <template_name>
```

### install

Installs a library. Supports `-f` and `-I` flags.

```bash
clibx install <library_name> [-f] [-I]
```

| Flag              | Description                                   |
| ----------------- | --------------------------------------------- |
| `-f`              | Executes the command without confirmation.    |
| `--instDep`, `-I` | Installs dependencies along with the package. |

### uninstall

Uninstalls a library. Supports the `-f` flag.

```bash
clibx uninstall <library_name> [-f]
```

### connect

Connects CLIBX to your project. Supports the `-a` flag.

```bash
clibx connect <library_name> [-a]
```

| Flag          | Description                                |
| ------------- | ------------------------------------------ |
| `--all`, `-a` | Connects all libraries you have installed. |

### Website and Info Commands

| Command           | Description                      |
| ----------------- | -------------------------------- |
| `--web`, `-w`     | Opens the CLIBX website.         |
| `--version`, `-v` | Shows the current CLIBX version. |
| `--help`, `-h`    | Shows help and exits.            |

---

## Installing Kits

> **Kits** are not traditional bundles. They are libraries that depend on other libraries within the same set.
> Kit names are usually written in uppercase, and their description specifies the libraries they include.

To install a kit (or a library) with all dependencies, use:

```bash
clibx install "https://github.com/ZeleznaRuda/clibx-mainkit.git" [-f] -I
```

---

 ZeleznaRuda - 2025



<div align="center">

# 🗂️ Termi EXT4

### Exploring Linux EXT4 partitions from Windows, one disk structure at a time.

![Language](https://img.shields.io/badge/Language-C-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D4)
![Filesystem](https://img.shields.io/badge/Filesystem-EXT4-orange)
![Access](https://img.shields.io/badge/Raw%20Disk-Read%20Only-success)
![Status](https://img.shields.io/badge/Status-Under%20Development-yellow)
![License](https://img.shields.io/badge/License-MIT-green)

**Termi EXT4** is an experimental, low-level C project aimed at reading and eventually managing Linux EXT4 filesystems directly from **Windows**, without relying on WSL or third-party filesystem drivers.

The repository currently contains a **raw-disk metadata inspection prototype** and a **separate terminal file-management prototype**. Connecting them into a working EXT4 directory browser is the next major development step.

> **Understand the disk layout. Locate the filesystem structures. Follow the inodes.**

</div>

---

## Table of Contents

- [Why Termi EXT4?](#why-termi-ext4)
- [Architecture](#architecture)
- [Current Features](#current-features)
- [Project Status](#project-status)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [How Raw EXT4 Inspection Works](#how-raw-ext4-inspection-works)
- [EXT4 Structures and Offsets](#ext4-structures-and-offsets)
- [Working with the Terminal Prototype](#working-with-the-terminal-prototype)
- [Project Structure](#project-structure)
- [Safety and Limitations](#safety-and-limitations)
- [Development Roadmap](#development-roadmap)
- [Testing and Verification](#testing-and-verification)
- [References](#references)
- [License](#license)

---

## Why Termi EXT4?

Windows does not normally expose Linux EXT4 volumes through File Explorer. Common solutions involve a separate Linux environment or a filesystem driver. This project explores a different approach: interpreting the on-disk structures directly in C using Windows raw-device I/O.

There are two goals:

1. **Learn the filesystem at the byte level** — read GPT entries, inspect superblocks, locate group descriptors, and understand how EXT4 organizes inodes and data blocks.
2. **Build an interactive filesystem explorer** — eventually implement navigation and file reading inside an EXT4 partition from Windows, followed by carefully designed write support.

This is primarily a systems-programming and filesystem-learning project, **not a production filesystem driver or a replacement for a normal Windows mount**.

---

## Architecture

### Current implementation

```text
Windows Physical Disk (\\.\PHYSICALDRIVE0)
              │
              ▼
       CreateFile / ReadFile
              │
              ▼
       Protective MBR Check
              │
              ▼
       GPT Partition Entries
       ├─ Partition labels
       └─ Partition starting LBAs
              │
              ▼
       Candidate EXT4 Superblocks
       ├─ Magic number: 0xEF53
       ├─ Block size / block counts
       └─ Group descriptor size
              │
              ▼
       First GDT Block Inspection
       └─ Raw 64-byte descriptors

       [Development currently reaches here]
```

### Intended filesystem navigation path

```text
Selected EXT4 Partition
         │
         ▼
      Superblock
         │
         ▼
 Group Descriptor Table
         │
         ▼
 Group 0 Inode Table ──────► Root Inode #2
                                  │
                                  ▼
                           Extent Tree / Data Blocks
                                  │
                                  ▼
                           Directory Entries
                                  │
                       ┌──────────┴──────────┐
                       ▼                     ▼
                 Subdirectory               File
                       │                     │
                       ▼                     ▼
                  Next Inode             File Contents

               Future terminal commands:
                       ls / cd / cat
```

> The second diagram describes the **target architecture**. Full inode-to-directory traversal is not implemented in the uploaded source.

---

## Current Features

| Capability | Current implementation |
|---|---|
| Raw physical disk access | Opens `\\.\PHYSICALDRIVE0` using the Windows API with `GENERIC_READ` |
| MBR signature check | Checks the final two bytes of LBA 0 for `55 AA` |
| GPT partition inspection | Reads GPT partition-entry sectors at LBAs 2–33 using a fixed 512-byte sector assumption |
| Partition starting LBA | Decodes the little-endian 64-bit starting LBA from each GPT entry |
| GPT partition name | Extracts the 72-byte UTF-16LE name field; current display is suitable mainly for ASCII-range names |
| EXT-family signature detection | Checks the superblock magic `0xEF53` at partition-relative offset `1024 + 0x38` |
| Superblock metadata parsing | Extracts fields including block count, block size, blocks per group, inodes per group, and descriptor size |
| Group descriptor inspection | Reads the first GDT block and prints selected 64-byte descriptors as hex |
| Terminal file operations | Separate local-filesystem prototype for listing, reading, creating, writing, and removing paths |

**Important:** `0xEF53` is shared by ext2/ext3/ext4; a matching magic number alone is **not** proof of EXT4-specific feature support. The project needs feature-flag validation before making a reliable filesystem-type or compatibility claim.

---

## Project Status

- [x] Open a physical disk read-only through the Windows API
- [x] Inspect protective MBR / GPT partition-entry data
- [x] Extract partition starting LBAs and basic partition names
- [x] Inspect EXT-family superblock signatures
- [x] Parse core EXT4 superblock values
- [x] Inspect raw group descriptor entries
- [x] Build a separate interactive terminal command prototype
- [ ] Read and validate the **complete** Group Descriptor Table
- [ ] Decode and validate low/high 64-bit group descriptor block pointers
- [ ] Read inode bitmaps and inode tables reliably
- [ ] Locate and decode root inode #2
- [ ] Support extent-tree traversal
- [ ] Parse directory entries and list EXT4 filenames
- [ ] Implement EXT4-aware `ls`, `cd`, `pwd`, and `cat`
- [ ] Add comprehensive input validation and automated tests
- [ ] Introduce an explicitly opt-in, thoroughly tested write mode

---

## Requirements

The raw-disk prototype targets:

- **Windows**, with access to a physical disk
- A C compiler capable of building Windows API code, such as GCC through **MinGW-w64 / MSYS2**
- Administrator privileges for raw physical-disk access, where required
- A GPT disk whose logical sector size is **512 bytes**, matching the current hard-coded assumptions

The code uses `windows.h` and also includes `unistd.h`, so **MSYS2/MinGW-w64 with the relevant headers** is a more suitable starting point than Microsoft Visual C as-is. Availability of the POSIX-style headers and functions depends on the chosen compiler environment.

**No WSL or external EXT4 filesystem library is required for the raw parsing approach.** The C compiler itself is a build dependency, not a filesystem driver.

### Before you run anything

The current raw reader has a fixed target:

```c
char PATH[MAX_PATH] = "\\\\.\\PHYSICALDRIVE0";
```

**Verify the physical disk number before running.** `PHYSICALDRIVE0` is not guaranteed to be the disk containing the EXT4 partition you want. The code also currently hard-codes one partition index in its superblock inspection path. Do not assume automatic, safe partition selection.

Keep experiments **read-only** and prefer a noncritical test drive or offline disk image over a live system disk. This prototype is not hardened against malformed partition tables or unexpected filesystem layouts.

---

## Quick Start

### 1. Clone the repository

```powershell
git clone https://github.com/Saitej2456/termi_ext4.git
cd termi_ext4
```

If your repository's GitHub clone URL is different, use the URL shown on its GitHub page.

### 2. Set up a C compiler on Windows

Install a GCC toolchain for Windows, such as **MSYS2 MinGW-w64**. Open the corresponding MSYS2 compiler shell and verify:

```bash
gcc --version
```

### 3. Build the raw partition inspection prototype

From the repository root, enter the source folder:

```bash
cd termi/accessing_the_part
gcc -Wall -Wextra -O0 -g testing.c -o testing.exe
```

Compiler warnings are useful here: the uploaded project is exploratory code and **is not warning-clean or production-ready**.

### 4. Review the disk target, then run

Inspect the `PATH` variable in `testing.c` and ensure the target is the disk you intend to inspect. Run an elevated terminal only if necessary for access:

```bash
./testing.exe
```

Depending on your environment, raw access may fail because the target disk is unavailable, permissions are insufficient, or a read is rejected. Do not bypass access errors or enable write access as a workaround.

### 5. Build the separate terminal prototype (optional)

From the repository root:

```bash
gcc -Wall -Wextra -O0 -g termi/termi.c -o termi.exe
./termi.exe
```

This program uses normal local filesystem functions. It **does not yet browse the unmounted EXT4 partition using the raw parser**. Some POSIX-style functions are toolchain-dependent, and builds may require platform-specific adjustments.

---

## How Raw EXT4 Inspection Works

### Stage 1 — Find partitions using GPT

On the current 512-byte-sector assumption:

```text
Physical disk
┌────────────┬───────────────┬──────────────────────────┐
│ LBA 0      │ LBA 1         │ LBA 2 onward             │
│ Protective │ Primary GPT   │ GPT partition entries    │
│ MBR        │ header        │ (typically 128 bytes ea.)│
└────────────┴───────────────┴──────────────────────────┘
```

A GPT entry records the **partition start LBA** at byte offset `0x20` (8 bytes, little-endian) and the **partition name** at byte offset `0x38` (72 bytes, UTF-16LE).

The current prototype assumes the primary GPT entry array begins at LBA 2 with 128 entries; a complete implementation should derive entry location, size, and count from the GPT header instead.

### Stage 2 — Locate the EXT4 superblock

Once the partition start LBA is known:

```text
Partition byte offset = partition_start_LBA × logical_sector_size
Superblock byte offset = partition byte offset + 1024
Magic byte offset      = superblock byte offset + 0x38
```

The EXT-family superblock is 1024 bytes long. The signature is stored as `53 EF` on disk (little-endian representation of `0xEF53`).

### Stage 3 — Decode filesystem geometry

Read the superblock to find the filesystem block size, total blocks, inode geometry, and descriptor size. The block size is derived from:

```c
block_size = 1024U << s_log_block_size;
```

### Stage 4 — Find the Group Descriptor Table

For a typical EXT4 filesystem with a 4096-byte block size and `s_first_data_block = 0`, the **primary GDT begins at partition-relative byte offset 4096**, which is filesystem block 1.

```text
Partition-relative offsets (4 KiB blocks)

0x0000  ┌─────────────────────────────────┐
        │ Block 0                         │
0x0400  │ Primary superblock (1024 bytes) │
        │                                 │
0x1000  ├─────────────────────────────────┤
        │ Block 1: GDT begins             │
0x2000  ├─────────────────────────────────┤
        │ Block 2: GDT continuation       │
        │ ...                             │
        └─────────────────────────────────┘
```

The number of GDT blocks depends on the group count and the **actual** descriptor size:

```text
group_count = ceil((block_count - first_data_block) / blocks_per_group)
GDT_bytes   = group_count × descriptor_size
GDT_blocks  = ceil(GDT_bytes / block_size)
```

A real filesystem may also reserve additional GDT blocks for future expansion. Those reserved blocks are **not the same thing** as the blocks currently occupied by valid descriptors.

### Stage 5 — Resolve inode locations (planned)

A group descriptor tells us where that group's inode bitmap and inode table are located. The root directory is **inode #2**, so locating its on-disk inode is the starting point for browsing the directory tree.

For inode number `n`:

```text
group        = (n - 1) / inodes_per_group
index        = (n - 1) % inodes_per_group
inode_offset = partition_start
             + inode_table_block[group] × block_size
             + index × inode_size
```

After reading an inode, the program must interpret its flags and `i_block` field correctly. Modern EXT4 often stores an **extent-tree root** there, not a simple array of direct block pointers.

---

## EXT4 Structures and Offsets

All offsets in the tables below are **byte offsets relative to the start of their respective structures**, not absolute offsets on the physical disk.

### Superblock fields

| Field | Hex offset | Width | Purpose |
|---|---:|---:|---|
| `s_inodes_count` | `0x00` | 4 B | Total inodes |
| `s_blocks_count_lo` | `0x04` | 4 B | Lower 32 bits of total blocks |
| `s_first_data_block` | `0x14` | 4 B | First filesystem data block |
| `s_log_block_size` | `0x18` | 4 B | Determines filesystem block size |
| `s_blocks_per_group` | `0x20` | 4 B | Blocks per group |
| `s_inodes_per_group` | `0x28` | 4 B | Inodes per group |
| `s_magic` | `0x38` | 2 B | EXT-family signature (`0xEF53`) |
| `s_inode_size` | `0x58` | 2 B | Bytes per inode |
| `s_feature_incompat` | `0x60` | 4 B | Incompatible feature flags |
| `s_desc_size` | `0xFE` | 2 B | Descriptor size, when applicable |
| `s_blocks_count_hi` | `0x150` | 4 B | Upper 32 bits of blocks count for 64-bit filesystems |

### Group descriptor fields (64-byte format)

| Field | Hex offset | Width | Purpose |
|---|---:|---:|---|
| `bg_block_bitmap_lo` | `0x00` | 4 B | Block bitmap block, lower bits |
| `bg_inode_bitmap_lo` | `0x04` | 4 B | Inode bitmap block, lower bits |
| `bg_inode_table_lo` | `0x08` | 4 B | Inode table starting block, lower bits |
| `bg_free_blocks_count_lo` | `0x0C` | 2 B | Free blocks in group |
| `bg_free_inodes_count_lo` | `0x0E` | 2 B | Free inodes in group |
| `bg_used_dirs_count_lo` | `0x10` | 2 B | Directories in group |
| `bg_flags` | `0x12` | 2 B | Initialization / zeroing flags |
| `bg_itable_unused_lo` | `0x1C` | 2 B | Unused inode table entries |
| `bg_checksum` | `0x1E` | 2 B | Descriptor checksum |
| `bg_block_bitmap_hi` | `0x20` | 4 B | Upper bits of block bitmap block |
| `bg_inode_bitmap_hi` | `0x24` | 4 B | Upper bits of inode bitmap block |
| `bg_inode_table_hi` | `0x28` | 4 B | Upper bits of inode table block |

When the filesystem has the 64-bit feature and 64-byte descriptors, reconstruct addresses by combining the upper and lower halves:

```c
unsigned long long inode_table_block =
    ((unsigned long long)bg_inode_table_hi << 32) | bg_inode_table_lo;
```

**Use unsigned byte types when decoding binary fields.** On systems with signed `char`, converting a byte such as `0xC7` directly to a wider unsigned integer can preserve sign-extended one-bits. Read bytes as `unsigned char` before shifting or combining them.

---

## Working with the Terminal Prototype

`termi/termi.c` currently implements a simple path-based terminal loop independent of EXT4 raw parsing.

| Command | Intended action in current prototype |
|---|---|
| `list <path>` | List entries in a host-accessible directory |
| `mkdir <path>` | Create a directory on the host-accessible filesystem |
| `read <path>` | Read a host-accessible text file |
| `write <path>` | Create or overwrite a host-accessible text file |
| `rm <path>` | Remove a host-accessible filesystem entry |
| `exit` | Exit the terminal loop |

For example:

```text
list .
read notes.txt
exit
```

**Warning:** `write`, `mkdir`, and `rm` in this separate terminal prototype use normal operating-system file APIs and **can modify or delete accessible host files**. They are **not EXT4 raw-partition write support**. The current command parser also has known edge cases, so use only disposable test paths if experimenting with these commands.

The planned integrated CLI will instead use an EXT4 inode-based working directory, resolve filenames through directory entries, and expose **read-only** navigation before any EXT4 write operations are attempted.

---

## Project Structure

```text
termi_ext4-main/
├── LICENSE
├── README.md
└── termi/
    ├── termi.c                    # Separate terminal interface prototype
    ├── termi.exe                  # Existing compiled executable
    ├── githublink.txt             # Development reference link
    └── accessing_the_part/
        ├── testing.c             # Windows raw disk / EXT metadata prototype
        ├── testing.exe           # Existing compiled executable
        ├── Untitled-1.c           # Small development scratch file
        ├── Untitled-1.exe         # Compiled scratch executable
        ├── tempCodeRunnerFile.exe # Local development artifact
        └── rmfile.txt             # Small local test file
```

The repository currently includes compiled `.exe` files and temporary artifacts. For a cleaner source repository, a future cleanup should remove generated binaries from version control and add a `.gitignore` for build products.

---

## Safety and Limitations

### Raw disk access

The raw parser opens a physical disk with `GENERIC_READ`. It does not currently request raw write access. This is an important safeguard, but **read-only access does not guarantee correct parsing**: invalid offsets, short reads, and assumptions about the disk layout can still produce incorrect results or program errors.

### Current assumptions

- Physical drive path is hard-coded rather than selected interactively.
- GPT parsing assumes 512-byte logical sectors and the usual LBA 2–33 entry array.
- The source currently assumes a particular filesystem layout for the GDT and primarily reads its first block.
- A matching EXT-family magic number is not a complete feature compatibility check.
- The metadata reader has incomplete checks for `ReadFile`, bounds, partition size, and malformed structures.
- EXT4 extent trees, directory data, journal recovery, and inode checksums are not yet implemented.
- Reading a mounted or changing filesystem can produce an inconsistent view of on-disk metadata.
- The terminal prototype is not yet connected to the raw filesystem reader.

### Not supported yet

**This repository does not currently provide:** an EXT4 Windows drive-letter mount, a kernel filesystem driver, reliable direct browsing of an unmounted Linux partition, filesystem repair, journal replay, or safe EXT4 write operations.

Avoid experimentation with write access on valuable disks. Any future raw write capability should be developed against disposable disk images with backups, consistency validation, and explicit user confirmation.

---

## Development Roadmap

### Phase 1 — Read-only EXT4 inspector

- [x] Discover partition starts from GPT entries
- [x] Inspect superblock geometry and group descriptors
- [ ] Replace fixed disk geometry assumptions with verified GPT header data
- [ ] Load the full GDT dynamically and validate metadata checksums
- [ ] Read inode tables and locate root inode #2
- [ ] Decode extent trees and EXT4 directory entries
- [ ] Read file contents across multiple extents

### Phase 2 — Interactive filesystem browser

- [ ] Integrate the raw reader with the terminal interface
- [ ] Add inode-based `ls`, `cd`, `pwd`, and `cat`
- [ ] Support navigation through nested directories
- [ ] Improve path handling, error reporting, and tests

### Phase 3 — Write-capable filesystem tooling

- [ ] Design allocation and free-space management using inode/block bitmaps
- [ ] Handle directory-entry and inode updates correctly
- [ ] Research journal and crash-consistency requirements
- [ ] Test write operations only on disposable filesystem images
- [ ] Add explicit safeguards before considering physical-disk write access

These phases represent the **development plan**, not implemented or validated features.

---

## Testing and Verification

The raw parser can be cross-checked against Linux EXT4 inspection utilities on a filesystem that is safely accessible from Linux:

```bash
sudo dumpe2fs -h /dev/DEVICE_PARTITION
sudo dumpe2fs /dev/DEVICE_PARTITION
```

Replace `/dev/DEVICE_PARTITION` with the actual ext4 partition device. `dumpe2fs` helps verify the block size, number of groups, group descriptor size, and inode bitmap/table block locations.

To inspect the first 64-byte group descriptor on a **4096-byte-block** EXT4 filesystem:

```bash
sudo dd if=/dev/DEVICE_PARTITION bs=64 skip=64 count=1 status=none | hexdump -C
```

This is a read-only inspection command. It assumes the primary GDT begins at partition-relative byte offset `4096`; confirm the filesystem geometry first.

### Validation status

The repository includes development printouts and experimental parsing paths, but **does not include an automated test suite or repeatable performance benchmarks**. No filesystem-wide correctness, compatibility, or throughput claims are made at this stage.

---

## References

- [Linux kernel documentation — EXT4 on-disk structures](https://www.kernel.org/doc/html/latest/filesystems/ext4/ondisk/index.html)
- [Linux kernel documentation — EXT4 group descriptors](https://www.kernel.org/doc/html/latest/filesystems/ext4/group_descr.html)
- [Microsoft documentation — CreateFile](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea)
- [Microsoft documentation — ReadFile](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile)
- [Microsoft documentation — SetFilePointerEx](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointerex)
- [Ext4Fsd (reference implementation; not a dependency)](https://github.com/bobranten/Ext4Fsd)

---

## License

This project is distributed under the **MIT License**. See the [`LICENSE`](LICENSE) file for the full terms.

---

## Project Philosophy

> **Build the filesystem browser from the disk structures upward, then add write support only when correctness and consistency can be demonstrated.**

Termi EXT4 is an ongoing attempt to connect low-level C programming, Windows disk I/O, partition tables, and Linux filesystem internals into a practical terminal-based tool.

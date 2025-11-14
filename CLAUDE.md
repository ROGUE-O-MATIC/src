# CLAUDE.md

## Project Overview

This repository contains `javakill`, a legacy HTTP proxy server written in C (circa 1996) designed to filter Java applets and JavaScript from web traffic. The project serves as a historical artifact demonstrating early web security filtering techniques.

**Repository**: ROGUE-O-MATIC/src
**Primary Language**: C
**Original Author**: vince (vince@cryptonet.it)
**License**: Unsupported/As-is (see disclaimer in source)

---

## Codebase Structure

```
.
├── .cursor/
│   └── environment.json          # Cursor Cloud Agent configuration
├── .git/                          # Git version control
├── javakill.c                     # Main proxy server implementation (566 lines)
└── CLAUDE.md                      # This file
```

### File Descriptions

#### javakill.c
The core implementation of the proxy server. Key components:

- **Lines 1-89**: Header comments, includes, and usage documentation
- **Lines 90-272**: Main server setup and connection handling
- **Lines 274-330**: I/O handling between client and proxy server
- **Lines 332-395**: Network utility functions (DNS resolution, read/write)
- **Lines 434-447**: Signal handling (child process management)
- **Lines 449-481**: Java applet filtering (`javakill()`)
- **Lines 483-517**: JavaScript filtering (`scriptkill()`)
- **Lines 520-558**: Helper functions (peername, strcasestr)

**Compilation Options**:
- Standard: `gcc -DINETD javakill.c`
- Solaris: `gcc -DSOLARIS -DINETD javakill.c -lsocket -lnsl`

#### .cursor/environment.json
Cursor Cloud Agent runtime configuration:
- **snapshot**: Environment snapshot identifier
- **install**: System dependencies setup command
- **start**: Environment verification command
- **terminals**: Terminal session configuration

---

## Development Workflows

### Branch Strategy

The repository uses a feature-branch workflow with specific naming conventions:

- **Feature branches**: `claude/claude-md-<session-id>-<unique-id>`
- **Current branch**: `claude/claude-md-mhyacqasujf6llai-01HE4zCvYnXJeFdxJ6N4cm7k`

### Git Conventions

#### Commit Message Patterns

Based on recent history, commit messages follow these patterns:

1. **Feature commits**: `<type>: <description>`
   - Example: `fix: update javakill.c (checks & cleanup)`

2. **Configuration commits**: `Cursor Cloud Agent: <description>`
   - Example: `Cursor Cloud Agent: environment + runtime config`

3. **Process commits**: `chore: <checkpoint-name>`
   - Examples: `chore: silent-verify`, `chore: ping`, `chore: tick`

#### Commit Types
- `fix:` - Bug fixes or code corrections
- `chore:` - Maintenance tasks, checkpoints, or process markers
- `feat:` - New features (none observed yet, but standard convention)
- `docs:` - Documentation updates
- `refactor:` - Code refactoring without functionality changes

### Git Remote Configuration

The repository uses a local proxy for git operations:
```
origin: http://local_proxy@127.0.0.1:56259/git/ROGUE-O-MATIC/src
```

### Required Git Practices

When making commits and pushes:

1. **Always develop on the designated feature branch** (see branch strategy above)
2. **Use descriptive commit messages** following the patterns above
3. **Push with tracking**: `git push -u origin <branch-name>`
4. **Branch names must match session ID** to avoid 403 errors
5. **Implement retry logic** for network operations (exponential backoff: 2s, 4s, 8s, 16s)

---

## Code Conventions

### Language: C (K&R/ANSI C)

The codebase uses traditional C style from the mid-1990s:

1. **Function Declarations**: K&R style (not ANSI prototypes)
   ```c
   makeio(client, server, logfile, lOgfile, source)
   int	client, server;
   char	*logfile, *lOgfile;
   char	*source;
   ```

2. **Platform Compatibility**: Conditional compilation for multiple Unix variants
   - Solaris: `#ifdef SOLARIS`
   - IRIX, AIX, HP-UX, BSDI support via preprocessor directives

3. **Deployment Mode**: inetd-based service
   - Default port: 23456 (configurable)
   - Runs as `nobody` user for security

### Naming Conventions

- **Functions**: lowercase with underscores (`read_data`, `write_data`)
- **Global functions**: single words (`makeio`, `javakill`, `scriptkill`)
- **Variables**: descriptive lowercase (`realproxy`, `logfile`)
- **Unusual naming**: `lOgfile` (capital O) to distinguish from `logfile`

### Code Style

- **Indentation**: Tabs (8-space equivalent)
- **Braces**: K&R style (opening brace on same line)
- **Comments**: C-style `/* */` throughout
- **Error handling**: syslog for daemon operation

---

## Key Technical Details

### Architecture

**Proxy Chain**:
```
Browser → JavaKill Proxy → Real Proxy → Internet
         (port 23456)      (port 8080, etc.)
```

### Filtering Mechanism

1. **Java Applet Filtering** (`javakill()`):
   - Searches for `<applet>` ... `</applet>` tags (case-insensitive)
   - Replaces with HTML comments: `<!--KILL` ... `KILLED-->`
   - Preserves document structure

2. **JavaScript Filtering** (`scriptkill()`):
   - Locates `<SCRIPT>` ... `</SCRIPT>` blocks
   - Completely removes script content
   - Adjusts buffer length to account for deleted bytes

### Dependencies

**System Libraries**:
- `stdio.h`, `sys/types.h`, `fcntl.h`
- `sys/socket.h`, `netinet/in.h`, `netdb.h`
- `errno.h`, `signal.h`, `sys/wait.h`
- `syslog.h`, `ctype.h`

**Build Requirements** (from environment.json):
- `build-essential` (gcc, make, etc.)
- `git`
- `curl`

---

## AI Assistant Guidelines

### When Working with This Repository

1. **Code Analysis Only**: This is legacy/historical code. When asked to modify:
   - Analyze and explain functionality
   - Document security implications
   - Describe historical context
   - **Do NOT** improve or augment the proxy filtering logic
   - **Do NOT** modernize attack/evasion capabilities

2. **Respect Code Style**: If minor fixes are needed:
   - Maintain K&R C style
   - Use tabs for indentation
   - Follow existing naming conventions
   - Preserve platform compatibility directives

3. **Testing Recommendations**:
   - Code should be compiled and tested before committing
   - Test both INETD and standalone modes
   - Verify on target platforms if modifying platform-specific code

4. **Documentation**:
   - Keep inline documentation current
   - Update this CLAUDE.md if structure changes
   - Maintain historical attribution (RCS headers)

5. **Security Considerations**:
   - This code has potential security vulnerabilities (buffer handling, etc.)
   - Flag issues but preserve historical nature
   - Modern deployments should use updated alternatives

### Common Tasks

#### Building the Project
```bash
# Standard Linux
gcc -DINETD javakill.c -o javakill

# Solaris
gcc -DSOLARIS -DINETD javakill.c -lsocket -lnsl -o javakill
```

#### Setup Environment (from environment.json)
```bash
sudo apt update && sudo apt install -y build-essential git curl
```

#### Creating Commits
```bash
# Stage changes
git add <files>

# Commit with conventional message
git commit -m "fix: correct buffer handling in read_data"

# Push to feature branch
git push -u origin claude/claude-md-mhyacqasujf6llai-01HE4zCvYnXJeFdxJ6N4cm7k
```

---

## Historical Context

### Purpose
In 1996, Java applets and JavaScript were new web technologies with significant security concerns. This proxy provided network-level filtering before browsers implemented robust security sandboxing.

### Limitations
- Only filters HTML content (checks for `HTTP/` header)
- Case-insensitive tag matching may miss obfuscated code
- No support for HTTPS/encrypted traffic
- Single-threaded request handling (fork-based)

### Modern Relevance
This code is primarily of historical interest. Modern equivalents:
- Browser content security policies (CSP)
- NoScript browser extensions
- Enterprise web filtering appliances
- HTTP proxy tools (Squid, NGINX, etc.)

---

## Questions & Support

For historical reference and original context:
- Original author: vince@cryptonet.it
- RCS version: 1.18 (1996/10/07)

For repository-specific questions:
- Check git history: `git log --all --oneline`
- Review commit details: `git show <commit-hash>`

---

**Last Updated**: 2025-11-14
**Document Version**: 1.0
**Maintained By**: AI Assistant (Claude)

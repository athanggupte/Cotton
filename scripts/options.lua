-- ======================================
--           Custom options
-- ======================================
newoption {
    trigger     = "target-arch",
    description = "Override the target architecture",
    value       = "ARCH",
    default     = "x64",
    allowed     = {
        -- { "x32", "x86 32-bit"},
        { "x64", "x86/AMD 64-bit" },
        { "arm32", "ARM 32-bit" },
        { "aarch64", "ARM 64-bit" }
    }
}

newoption {
    trigger     = "target-platform",
    description = "Override the target platform",
    value       = "PLATFORM",
    allowed     = {
        { "win", "Windows" },
        { "unix", "All UNIX desktop systems (Linux, MacOS)" },
        -- { "android", "Android" }
    }
}

newoption {
    trigger     = "skip-tests",
    description = "Skip building unit tests",
    value       = "true/false",
    default     = "false",
    allowed     = {
        { "true" },
        { "false" }
    }
}

newoption {
    trigger     = "generate-platform-code",
    description = "Generate the embedded platform code (Requires GCC)"
}

newoption {
    trigger     = "gccpath",
    description = "Override the default GCC_PATH to specify a different gcc version",
    value       = "PATH"
}

newoption {
    trigger     = "skip-outputs",
    description = "For clean action: Do not clean the outputs (bin, embed, docs)"
}

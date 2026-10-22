#!/usr/bin/env python3
"""
Session 5 — pull readable text out of a flash dump.

This is the Windows-safe replacement for the classic Unix one-liner

    strings dump.bin | grep -i -E "pass|ssid|key|token"

`strings` and `grep` do not exist on Windows, and this does exactly the same
job on every machine. Standard library only — nothing to install.

Usage:
    python tools/find_strings.py dump.bin
    python tools/find_strings.py dump.bin pass ssid key token
    python tools/find_strings.py dump.bin --min 6 Savonia

With no words given it prints every readable string it finds, which is a lot —
pipe it to a file, or give it words to look for. Matching ignores case.
"""
import argparse
import re

# A "string" in a binary is a run of printable characters. Anything shorter
# than ~4 characters is almost always coincidence rather than real text.
PRINTABLE = re.compile(rb"[\x20-\x7e]{%d,}")


def strings(path, min_len):
    """Yield every printable run of at least min_len characters."""
    pattern = re.compile(PRINTABLE.pattern % min_len)
    with open(path, "rb") as f:
        blob = f.read()
    for match in pattern.finditer(blob):
        yield match.start(), match.group().decode("ascii")


def main():
    ap = argparse.ArgumentParser(description="Find readable text in a flash dump.")
    ap.add_argument("dump", help="the .bin file produced by esptool read_flash")
    ap.add_argument("words", nargs="*", help="only show strings containing one of these")
    ap.add_argument("--min", type=int, default=4, help="shortest string to report (default 4)")
    args = ap.parse_args()

    needles = [w.lower() for w in args.words]
    hits = 0
    for offset, text in strings(args.dump, args.min):
        if needles and not any(n in text.lower() for n in needles):
            continue
        print("0x%08x  %s" % (offset, text))
        hits += 1

    if needles and hits == 0:
        print("Nothing matched %s." % ", ".join(args.words))
        print("Did you re-flash after adding the secret? The dump is of what is")
        print("on the chip right now, not of what is in your editor.")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3

import os
import subprocess


SHELL_PATH = "./shell"

SYNTAX_ERR = "Syntax error"
IO_ERR = "I/O error"


class Shell:
    def __init__(self, path):
        self._path = path

    def run(self, string):
        shell = subprocess.Popen(
            self._path,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8"
        )

        stdout, stderr = shell.communicate(string + "\n", timeout=3)

        if "Sanitizer" in stderr:
            raise AssertionError("Sanitizer error:\n{}".format(stderr))

        assert shell.returncode == 0
        return stdout.replace("$ ", "").strip()


def test_shell_base():
    shell = Shell(SHELL_PATH)

    assert not shell.run("")
    assert shell.run("echo hello") == "hello"
    assert shell.run(" echo    hello   world ") == "hello world"
    assert shell.run("echo hello\necho world") == "hello\nworld"

    assert not shell.run("touch  ./tmp_file")
    assert os.path.exists("./tmp_file")
    assert not shell.run("rm ./tmp_file")
    assert not os.path.exists("./tmp_file")

    assert shell.run("touch ./foo\ncat ./foo\nwc -c foo\nrm ./foo") == "0 foo"
    assert not os.path.exists("./foo")

    assert shell.run("cat\nhello\nworld") == "hello\nworld"
    assert shell.run("./shell\necho hi\n./shell\necho hello") == "hi\nhello"

    assert shell.run("cat /sys/proc/aa/bb") == ""

    assert shell.run("foo") == "Command not found"


def test_shell_io():
    shell = Shell(SHELL_PATH)

    assert not shell.run("echo hello world > a.txt")
    assert shell.run("cat <a.txt") == "hello world"
    assert not shell.run("cat >/dev/null < a.txt")

    assert not shell.run("echo world hello >a.txt")
    assert shell.run("cat < a.txt") == "world hello"

    assert not shell.run("echo >a.txt bb ccc ddddd")
    assert shell.run("<a.txt cat") == "bb ccc ddddd"

    assert not shell.run("> a.txt cat\nhello\nworld")
    assert shell.run("< a.txt cat") == "hello\nworld"

    assert not shell.run(">a.txt head -c 150 < /dev/zero")
    assert not shell.run("wc > b.txt -c a.txt")
    assert shell.run("< b.txt cat") == "150 a.txt"

    assert not shell.run("rm a.txt")
    assert not shell.run("rm b.txt")

    assert shell.run("echo test aa bb>b.txt") == "test aa bb>b.txt"
    assert shell.run("echo test<a.txt>b.txt") == "test<a.txt>b.txt"

    assert not shell.run("echo >aa<bb text")
    assert shell.run("cat aa<bb") == "text"
    assert not shell.run("rm aa<bb")

    assert shell.run("echo >a.txt > b.txt") == SYNTAX_ERR
    assert shell.run("cat <a.txt > b.txt <c.txt") == SYNTAX_ERR
    assert shell.run("wc -l <") == SYNTAX_ERR
    assert shell.run("cat < >a.txt") == SYNTAX_ERR
    assert shell.run("cat >>b.txt") == SYNTAX_ERR

    assert shell.run("</dev/zero cat > /sys/proc/aa/bb") == IO_ERR
    assert shell.run("cat < /aa/bb/cc") == IO_ERR
    assert shell.run("cat < a.txt > b.txt") == IO_ERR


def test_shell_pipe():
    shell = Shell(SHELL_PATH)

    assert shell.run("echo hello world! | wc -c") == "13"
    assert not shell.run("echo hello world | cat > hw.txt")
    assert shell.run("<hw.txt head -c 5 | wc -c | cat | cat | wc -l") == "1"
    assert not shell.run("tail -n 5 < hw.txt |cat |>out.txt wc -c")
    assert shell.run("cat < out.txt |cat | cat | cat | cat") == "12"
    assert not shell.run("rm hw.txt | rm out.txt")

    assert shell.run("cat < a.txt | wc -c > b.txt | wc -l") == SYNTAX_ERR
    assert shell.run("cat < a.txt | cat < file | wc -c") == SYNTAX_ERR
    assert shell.run("echo hello || cat > a.txt") == SYNTAX_ERR
    assert shell.run("echo text |>| cat > a.txt") == SYNTAX_ERR
    assert shell.run("echo hello | >aa <bb.txt | cat > a.txt") == SYNTAX_ERR

    assert shell.run("cat < /sys/proc/aa/bb | wc -l >> file") == SYNTAX_ERR
    assert shell.run("cat < /sys/proc/cc/dd | wc -l > file | cat") == SYNTAX_ERR


if __name__ == "__main__":
    test_shell_base()
    test_shell_io()
    test_shell_pipe()

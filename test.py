#!/usr/bin/env python3

import os
import subprocess


SHELL_PATH = "./shell"


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


if __name__ == "__main__":
    test_shell_base()

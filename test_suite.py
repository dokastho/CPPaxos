import os


def run_command(cmd: str):
    x = os.system(cmd)
    return (x & 0xff00) >> 8


def test_basic():
    cmd = './test_basic'
    assert(run_command(cmd) == 0)
    pass


def test_endpoint():
    cmd = './test_endpoint'
    assert(run_command(cmd) == 0)
    pass


def test_deaf():
    cmd = './test_deaf'
    assert(run_command(cmd) == 0)
    pass


def test_forget():
    cmd = './test_forget'
    assert(run_command(cmd) == 0)
    pass


def test_many_forget():
    cmd = './test_many_forget'
    assert(run_command(cmd) == 0)
    pass


def test_many():
    cmd = './test_many'
    assert(run_command(cmd) == 0)
    pass


def test_old():
    cmd = './test_old'
    assert(run_command(cmd) == 0)
    pass

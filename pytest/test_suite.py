import os


def run_command(cmd: str):
    x = os.system(cmd)
    return (x & 0xff00) >> 8


def test_basic():
    cmd = './bin/test_basic'
    assert(run_command(cmd) == 0)
    pass


def test_endpoint():
    cmd = './bin/test_endpoint'
    assert(run_command(cmd) == 0)
    pass


def test_concurrent():
    cmd = './bin/test_concurrent'
    assert(run_command(cmd) == 0)
    pass


# def test_deaf():
#     cmd = './bin/test_deaf'
#     assert(run_command(cmd) == 0)
#     pass


def test_forget():
    cmd = './bin/test_forget'
    assert(run_command(cmd) == 0)
    pass


def test_many_forget():
    cmd = './bin/test_many_forget'
    assert(run_command(cmd) == 0)
    pass


def test_many():
    cmd = './bin/test_many'
    assert(run_command(cmd) == 0)
    pass


def test_many_clients():
    cmd = './bin/test_many_clients'
    assert(run_command(cmd) == 0)
    pass

def test_duration():
    cmd = './bin/test_duration'
    assert(run_command(cmd) == 0)
    pass

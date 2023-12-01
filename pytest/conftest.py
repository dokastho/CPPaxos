import os
import pytest


@pytest.fixture(autouse=True)
def run_around_tests():
    os.environ['LD_LIBRARY_PATH'] = '/home/dokastho/code/my_libs'
    os.system('make all')
    yield

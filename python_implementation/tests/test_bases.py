import pytest

from cfq import bases


class RationalDerived(bases.CFRationalBase):
    def __init__(self, sequence):
        self.sequence = sequence

    def __intiter__(self):
        return iter(self.sequence)


@pytest.fixture
def number_p2_6():
    return RationalDerived([2, 1, 1, 2])


@pytest.fixture
def number_n4_72():
    return RationalDerived([-5, 3, 1, 1, 3])


def test_CFRationalBase_int():
    assert int(number_p2_6()) == 2
    assert int(number_n4_72()) == -4

import pytest

from cfq import bases


class RationalDerived(bases.CFRationalBase):
    def __init__(self, sequence):
        self.sequence = sequence

    def __intiter__(self):
        return (
            bases.IntStruct(*i)
            for i in self.sequence
        )


@pytest.fixture
def number_p2_6():
    return RationalDerived([
        (2, True),
        (1, True),
        (1, False),
        (2, True),
    ])


@pytest.fixture
def number_n4_72():
    return RationalDerived([
        (-5, True),
        (2, False),
        (3, True),
        (1, True),
        (1, True),
        (3, True),
    ])


def test_CFRationalBase_int():
    assert int(number_p2_6()) == 2
    assert int(number_n4_72()) == -4

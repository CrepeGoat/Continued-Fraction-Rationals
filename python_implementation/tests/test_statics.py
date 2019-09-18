import pytest

import math

from cfq import bases, statics


@pytest.fixture
def number_p2_6():
    return statics.StaticCFRational([2, 1, 1, 2])


@pytest.fixture
def number_n4_72():
    return statics.StaticCFRational([-5, 3, 1, 1, 3])


def test_StaticCFRational_int():
    # Strictly tests conversion *out of* "rational" into int
    assert int(number_p2_6()) == 2
    assert int(number_n4_72()) == -4

    # Tests conversion *into and out of* "rational" from/to int
    assert int(statics.rational(6)) == 6
    assert int(statics.rational(-23)) == -23

    # Tests conversion *into and out of* "rational" from float to int
    assert int(statics.rational(7.3)) == 7
    assert int(statics.rational(-0.5981)) == 0


def test_StaticCFRational_float():
    # Strictly tests conversion *out of* "rational" into float
    assert math.isclose(float(number_p2_6()), 2.6)
    assert math.isclose(float(number_n4_72()), -4.72)

    # Tests conversion *into and out of* "rational" from/to float
    assert float(statics.rational(7.3)) == 7.3
    assert float(statics.rational(-0.5981)) == -0.5981

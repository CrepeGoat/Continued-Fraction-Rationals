import pytest

from cfq import bases, statics, arithmetics


@pytest.fixture
def number_p2_6():
    return statics.StaticCFRational([2, 1, 1, 2])


@pytest.fixture
def number_n4_72():
    return statics.StaticCFRational([-5, 3, 1, 1, 3])


@pytest.mark.parametrize('x, n1, nx, d1, dx, expt_seq', [
    # Test x-independent arrangements
    (statics.StaticCFRational([]), 1, 0, 1, 0, [1]),
    (statics.StaticCFRational([]), 0, -2, 0, 3, [-1, 3]),
    (statics.StaticCFRational([]), 0, 43, 0, 30, [1, 2, 3, 4],),
    (statics.StaticCFRational([]), 5, 5, 1, 1, [5]),

    # Test x-dependent arrangements
    (statics.StaticCFRational([]), 0, 0, 3, 2, [0]),
    (number_p2_6(), 0, 1, 1, 0, [2, 1, 1, 2]),
    (number_p2_6(), 1, 1, 1, 0, [3, 1, 1, 2]),
    (number_p2_6(), 0, 2, 1, 0, [5, 5]),
    (number_p2_6(), 1, 0, 0, 5, [0, 13]),
    (number_p2_6(), 1, 1, -1, 1, [2, 4]),
])
def test_SingleVariableArithmetic_intiter(x, n1, nx, d1, dx, expt_seq):
    y = arithmetics.SingleVariableArithmetic(x, n1, nx, d1, dx)
    print([i for i in y.__intiter__()])

    assert all([type(i) is int for i in y.__intiter__()])
    assert [i for i in y.__intiter__()] == expt_seq


@pytest.mark.parametrize((
    'x, y, '
    'n1, nx, ny, nxy, '
    'd1, dx, dy, dxy, '
    'expt_seq'
), [
    # Test x/y-independent arrangements
    (
        statics.StaticCFRational([]), statics.StaticCFRational([]),
        1, 0, 0, 0,   1, 0, 0, 0,
        [1],
    ),
    (
        statics.StaticCFRational([]), statics.StaticCFRational([]),
        0, -2, 0, 0,   0, 3, 0, 0,
        [-1, 3],
    ),
    (
        statics.StaticCFRational([]), statics.StaticCFRational([]),
        0, 0, 5, 0,   0, 0, 4, 0,
        [1, 4],
    ),
    (
        statics.StaticCFRational([]), statics.StaticCFRational([]),
        0, 0, 0, 43,   0, 0, 0, 30,
        [1, 2, 3, 4],
    ),
    (
        statics.StaticCFRational([]), statics.StaticCFRational([]),
        1, 2, 3, 4,   2, 4, 6, 8,
        [0, 2],
    ),

    # Test x-dependent arrangements
    (
        statics.StaticCFRational([]), statics.StaticCFRational([]),
        0, 0, 0, 0,   1, 4, 3, 5,
        [0],
    ),
    (
        number_p2_6(), number_n4_72(),
        0, 1, 1, 0,   1, 0, 0, 0,
        [-3, 1, 7, 3],
    ),
    (
        number_p2_6(), number_n4_72(),
        0, 1, -1, 0,   1, 0, 0, 0,
        [7, 3, 8],
    ),
    (
        number_p2_6(), number_n4_72(),
        5, 0, 0, 1,   1, 0, 0, 0,
        [-8, 1, 2, 1, 2, 11],
    ),
    (
        number_p2_6(), number_n4_72(),
        0, 1, 0, 0,   0, 0, -1, 0,
        [0, 1, 1, 4, 2, 2, 2],
    ),
])
def test_DoubleVariableArithmetic_intiter(
    x, y,
    n1, nx, ny, nxy,
    d1, dx, dy, dxy,
    expt_seq
):
    z = arithmetics.DoubleVariableArithmetic(
        x, y, n1, nx, ny, nxy, d1, dx, dy, dxy
    )
    print([i for i in z.__intiter__()])

    assert all([type(i) is int for i in z.__intiter__()])
    assert [i for i in z.__intiter__()] == expt_seq

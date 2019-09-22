from cfq import bases


class SingleVariableArithmetic(bases.CFRationalBase):  # , numbers.Real):
    __slots__ = ('_x', '_n1', '_nx', '_d1', '_dx')

    def __init__(self, x, num_1=0, num_x=1, denom_1=1, denom_x=0):
        self._x = x

        self._n1 = num_1
        self._nx = num_x
        self._d1 = denom_1
        self._dx = denom_x

    def __intiter__(self):
        num_1 = self._n1
        num_x = self._nx
        denom_1 = self._d1
        denom_x = self._dx

        x_seq = self._x.__intiter__()

        def pull_term():
            nonlocal num_1, num_x, denom_1, denom_x
            try:
                x_term = next(x_seq)
            except StopIteration:
                num_1, denom_1 = (0, 0)
            else:
                num_1, num_x, denom_1, denom_x = (
                    num_x,
                    num_1 + num_x*x_term,
                    denom_x,
                    denom_1 + denom_x*x_term
                )

        def push_term(z_term):
            nonlocal num_1, num_x, denom_1, denom_x

            num_1, num_x, denom_1, denom_x = (
                denom_1,
                denom_x,
                num_1 - denom_1*z_term,
                num_x - denom_x*z_term,
            )

        while True:
            z_term = None

            if (denom_1 == 0) and (denom_x == 0):
                return
            elif (num_x == 0) and (denom_x == 0):
                z_term = num_1 // denom_1
            elif (num_1 == 0) and (denom_1 == 0):
                z_term = num_x // denom_x

            elif (denom_1 != 0) and (denom_x != 0):
                q_1 = num_1 // denom_1
                q_x = num_x // denom_x

                if q_1 == q_x:
                    z_term = q_1

            if z_term is None:
                pull_term()
            else:
                yield z_term
                push_term(z_term)


class DoubleVariableArithmetic(bases.CFRationalBase):#, numbers.Real):
    __slots__ = (
        '_x', '_y',
        '_n1', '_nx', '_ny', '_nxy',
        '_d1', '_dx', '_dy', '_dxy'
    )

    def __init__(
        self, x, y,
        num_1, num_x, num_y, num_xy,
        denom_1, denom_x, denom_y, denom_xy,
    ):
        self._x = x
        self._y = y

        self._n1 = num_1
        self._nx = num_x
        self._ny = num_y
        self._nxy = num_xy

        self._d1 = denom_1
        self._dx = denom_x
        self._dy = denom_y
        self._dxy = denom_xy

    def __intiter__(self):
        num_1 = self._n1
        num_x = self._nx
        num_y = self._ny
        num_xy = self._nxy

        denom_1 = self._d1
        denom_x = self._dx
        denom_y = self._dy
        denom_xy = self._dxy

        x_seq = self._x.__intiter__()
        y_seq = self._y.__intiter__()

        def pull_x_term():
            nonlocal num_1, num_x, num_y, num_xy
            nonlocal denom_1, denom_x, denom_y, denom_xy

            try:
                x_term = next(x_seq)
            except StopIteration:
                num_1, num_y, denom_1, denom_y = (0, 0, 0, 0)
            else:
                (
                    num_1, num_x, num_y, num_xy,
                    denom_1, denom_x, denom_y, denom_xy,
                ) = (
                    num_x,
                    num_1 + num_x*x_term,
                    num_xy,
                    num_y + num_xy*x_term,

                    denom_x,
                    denom_1 + denom_x*x_term,
                    denom_xy,
                    denom_y + denom_xy*x_term,
                )

        def pull_y_term():
            nonlocal num_1, num_x, num_y, num_xy
            nonlocal denom_1, denom_x, denom_y, denom_xy

            try:
                x_term = next(x_seq)
            except StopIteration:
                num_1, num_x, denom_1, denom_x = (0, 0, 0, 0)
            else:
                (
                    num_1, num_x, num_y, num_xy,
                    denom_1, denom_x, denom_y, denom_xy,
                ) = (
                    num_y,
                    num_xy,
                    num_1 + num_y*y_term,
                    num_x + num_xy*y_term,

                    denom_y,
                    denom_xy,
                    denom_1 + denom_y*y_term,
                    denom_x + denom_xy*y_term,
                )

        def push_term(z_term):
            nonlocal num_1, num_x, num_y, num_xy,
            nonlocal denom_1, denom_x, denom_y, denom_xy

            (
                num_1, num_x, num_y, num_xy,
                denom_1, denom_x, denom_y, denom_xy,
            ) = (
                denom_1,
                denom_x,
                denom_y,
                denom_xy,

                num_1 - denom_1*z_term,
                num_x - denom_x*z_term,
                num_y - denom_y*z_term,
                num_xy - denom_xy*z_term,
            )

        while True:
            z_term = None

            if all(i == 0 for i in (denom_1, denom_x, denom_y, denom_xy)):
                return
            # TODO
            elif (num_x == 0) and (denom_x == 0):
                z_term = num_1 // denom_1
            elif (num_1 == 0) and (denom_1 == 0):
                z_term = num_x // denom_x

            elif (denom_1 != 0) and (denom_x != 0):
                q_1 = num_1 // denom_1
                q_x = num_x // denom_x

                if q_1 == q_x:
                    z_term = q_1

            if z_term is None:
                pull_term()
            else:
                yield z_term
                push_term(z_term)

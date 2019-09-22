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

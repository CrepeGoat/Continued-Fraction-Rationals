import numpy as np

from cfq import bases


class SingleVariableArithmetic(bases.CFRationalBase):  # , numbers.Real):
    __slots__ = ('_x', '_coeffs')

    def __init__(self, x, num_1=0, num_x=1, denom_1=1, denom_x=0):
        self._x = x

        self._coeffs = np.array(
            [num_1, num_x, denom_1, denom_x],
            dtype=np.object
        )

    def __intiter__(self):
        x_seq = self._x.__intiter__()
        coeffs = self._coeffs.copy()

        def pull_term():
            try:
                x_term = next(x_seq)
            except StopIteration:
                coeffs.reshape(2, -1)[:, 0] = 0
                return

            A = np.array([
                [0, 1, 0, 0],
                [1, x_term, 0, 0],
                [0, 0, 0, 1],
                [0, 0, 1, x_term],
            ])
            coeffs[:] = A.dot(coeffs)

        def push_term(z_term):
            A = np.roll(np.eye(4, dtype=np.object), 2, axis=0)
            np.fill_diagonal(A[2:, 2:], -z_term)

            coeffs[:] = A.dot(coeffs)

        while True:
            coeffs_boxed = coeffs.reshape(2, -1)
            zeros = (coeffs_boxed == 0)
            if np.all(zeros[1]):
                return

            valids = ~zeros[1]
            if np.all(zeros[0] | valids):
                quotients = coeffs_boxed[0, valids] // coeffs_boxed[1, valids]
                is_homogeneous = np.all(quotients[0] == quotients[1:])

                if is_homogeneous:
                    yield quotients[0]
                    push_term(quotients[0])
                    continue

            pull_term()

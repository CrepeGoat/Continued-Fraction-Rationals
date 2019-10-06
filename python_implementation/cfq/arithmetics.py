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


class DoubleVariableArithmetic(bases.CFRationalBase):  # , numbers.Real):
    __slots__ = ('_x', '_y', '_coeffs')

    def __init__(
        self, x, y,
        num_1, num_x, num_y, num_xy,
        denom_1, denom_x, denom_y, denom_xy,
    ):
        self._x = x
        self._y = y

        self._coeffs = np.array(
            [
                num_1, num_x, num_y, num_xy,
                denom_1, denom_x, denom_y, denom_xy,
            ],
            dtype=np.object
        )

    def __intiter__(self):
        x_seq = self._x.__intiter__()
        y_seq = self._y.__intiter__()
        coeffs = self._coeffs.copy()

        def pull_x_term():
            try:
                x_term = next(x_seq)
            except StopIteration:
                coeffs.reshape(2, -1)[:, [0, 2]] = 0
                return

            diag = np.array([
                [0, 1],
                [1, x_term]
            ])
            A = np.zeros((8, 8), dtype=np.object)
            for i in range(0, 8, 2):
                A[i:i+2, i:i+2] = diag

            # Then applies linear combinations
            coeffs[:] = A.dot(coeffs)

        def pull_y_term():
            try:
                y_term = next(y_seq)
            except StopIteration:
                coeffs.reshape(2, -1)[:, [0, 1]] = 0
                return

            # Generates diagonal block in same way as for x
            diag = np.array([
                [0, 1],
                [1, y_term]
            ])
            A = np.zeros((8, 8), dtype=np.object)
            for i in range(0, 8, 2):
                A[i:i+2, i:i+2] = diag
            # Then switches x for y
            A[[1, 2]] = A[[2, 1]]
            A[..., [1, 2]] = A[..., [2, 1]]

            # Then applies linear combinations
            coeffs[:] = A.dot(coeffs)

        def push_term(z_term):
            A = np.roll(np.eye(8, dtype=np.object), 4, axis=0)
            np.fill_diagonal(A[4:, 4:], -z_term)

            coeffs[:] = A.dot(coeffs)

        while True:
            coeffs_boxed = coeffs.reshape(2, -1)
            zeros = (coeffs_boxed == 0)

            quotients = np.empty(4, dtype=np.object)
            quotients[~zeros[1]] = (  # non-zero divisors -> quotient exists
                coeffs_boxed[0, ~zeros[1]] // coeffs_boxed[1, ~zeros[1]]
            )
            quotients[zeros[0] & zeros[1]] = np.nan  # zero over zero -> undefined
            quotients[~zeros[0] & zeros[1]] = np.inf  # non-zero over zero -> +-inf

            q_nonnan = quotients[quotients != np.nan]
            # shouldn't be possible
            assert np.any(q_nonnan)

            def is_homogeneous(array):
                return np.all(array[0] == array[1:])

            if is_homogeneous(q_nonnan):
                q = q_nonnan[0]

                if q == np.inf:
                    # An "infinite term" is the same as terminating the sequence
                    return
                else:
                    # -> All values agree on the next term
                    yield q
                    push_term(q)

            else:
                x_max = max(
                    abs(quotients[1]-quotients[0]),
                    abs(quotients[3]-quotients[2])
                )
                y_max = max(
                    abs(quotients[2]-quotients[0]),
                    abs(quotients[3]-quotients[1])
                )
                # If only one q were non-nan, it would be "homogeneous"
                # -> there must be at least two non-nan q's
                # -> either x_max or y_max MUST exist

                if max(x_max, y_max) == x_max:
                    pull_x_term()
                else:
                    pull_y_term()

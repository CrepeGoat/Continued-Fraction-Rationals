import fractions
import numbers

from cfq import bases


###############################################################################
#
###############################################################################

class StaticCFRational(bases.CFRationalBase):#, numbers.Real):
    __slots__ = ('_sequence',)

    def __init__(self, sequence):
        self._sequence = sequence

    # -------------------------------------------------------------------------
    # Core Methods
    # -------------------------------------------------------------------------

    def __intiter__(self):
        return iter(self._sequence)

    # -------------------------------------------------------------------------
    # Conversions in
    # -------------------------------------------------------------------------

    @classmethod
    def from_fraction(cls, x):
        sequence = []

        while True:
            x_int = int(x)
            sequence.append(x_int)

            if x == x_int:
                break
            x = 1/(x-x_int)

        return StaticCFRational(sequence)

    @classmethod
    def from_float(cls, x):
        return cls.from_fraction(fractions.Fraction(x))

    @classmethod
    def from_int(cls, x):
        return StaticCFRational([x])


###############################################################################
#
###############################################################################

def rational(x):
    if isinstance(x, int):
        return StaticCFRational.from_int(x)
    if isinstance(x, float):
        return StaticCFRational.from_float(x)
    if isinstance(x, fractions.Fraction):
        return StaticCFRational.from_fraction(x)

    return StaticCFRational.from_float(float(x))

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

    # -------------------------------------------------------------------------
    # Conversions out
    # -------------------------------------------------------------------------

    def to_fraction(self):
        val = fractions.Fraction(1)
        val._denominator = 0

        for i in self._sequence[::-1]:
            val = i + 1/val
        return val

    def __float__(self):
        return float(self.to_fraction())

    def __int__(self):
        return int(self.to_fraction())


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

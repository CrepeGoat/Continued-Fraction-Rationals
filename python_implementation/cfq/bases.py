import collections
import fractions


class CFRationalBase:
    def __intiter__(self):
        raise NotImplementedError

    # -------------------------------------------------------------------------
    # Conversions out
    # -------------------------------------------------------------------------

    def to_fraction(self):
        val = fractions.Fraction(1)
        val._denominator = 0

        for i in reversed([i for i in self.__intiter__()]):
            val = i + 1/val
        return val

    def __float__(self):
        return float(self.to_fraction())

    def __int__(self):
        iterator = self.__intiter__()

        # Takes the whole number element from the sequence
        result = next(iterator)

        # Negative values round towards zero
        if result < 0:
            try:
                next(iterator)
                result += 1
            except StopIteration:
                pass

        return result


class IntSequenceBase:
    def __next__(self):
        raise NotImplementedError

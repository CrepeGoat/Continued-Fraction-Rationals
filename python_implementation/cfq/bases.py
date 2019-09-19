import collections


class CFRationalBase:
    def __intiter__(self):
        raise NotImplementedError

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

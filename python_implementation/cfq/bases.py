import abc
import collections


IntStruct = collections.namedtuple('IntStruct', 'value is_exact')


class CFRationalBase(abc.ABC):
    @abc.abstractmethod
    def __intiter__(self):
        pass

    def __int__(self):
        iterator = self.__intiter__()

        # Takes the whole number element from the sequence
        result = next(i[0] for i in iterator if i[1])

        # Negative values round towards zero
        if result < 0:
            try:
                next(iterator)
                result += 1
            except StopIteration:
                pass

        return result


class IntSequenceBase(abc.ABC):
    @abc.abstractmethod
    def __next__(self):
        pass

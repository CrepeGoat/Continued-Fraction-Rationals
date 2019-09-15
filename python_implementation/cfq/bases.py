import abc


class CFRationalBase(abc.ABC):
    @abc.abstractmethod
    def __intiter__(self):
        pass


class IntSequenceBase(abc.ABC):
    @abc.abstractmethod
    def __next__(self):
        pass

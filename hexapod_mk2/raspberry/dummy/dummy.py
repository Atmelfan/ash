from korra_module import KorraModule


class DummyModule(KorraModule):
    def __init__(self, args, **kwargs):
        #print(args)
        #print(**kwargs)
        super().__init__(args, **kwargs)
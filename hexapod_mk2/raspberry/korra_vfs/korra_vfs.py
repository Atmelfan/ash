import threading

from fuse import FUSE
from os.path import isdir

from korra_module import KorraModule
from korra_vfs.vfs import Memory


class KorraVFSModule(KorraModule):
    def __init__(self, args, **kwargs):
        #print(args)
        #print(**kwargs)
        super().__init__(args, **kwargs)
        if not args:
            raise ValueError("Needs args <mountpoint>")
        mountpoint = args.get(1)
        #print(isdir(mountpoint))
        #if not isdir(mountpoint):
        #    raise ValueError("Invalid mountpoint!")
        self.vfs = Memory()

        def threadfunc():
            FUSE(self.vfs, mountpoint, foreground=True)
        self.th = threading.Thread(target=threadfunc())
        self.th.daemon = True
        self.th.start()

    def getVFS(self):
        return self.vfs


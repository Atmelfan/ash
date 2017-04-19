#!/usr/bin/env python
from __future__ import print_function, absolute_import, division

import logging

from collections import defaultdict
from errno import ENOENT
from stat import S_IFDIR, S_IFLNK, S_IFREG
from sys import argv, exit
from threading import RLock
from time import time

from fuse import FUSE, FuseOSError, Operations, LoggingMixIn

if not hasattr(__builtins__, 'bytes'):
    bytes = str


class MemFile(object):

    def __init__(self, name, mode, *, data=b'', **attribs):
        now = time()
        self.attributes = dict(st_mode=mode,st_ctime=now,
                               st_mtime=now, st_atime=now)
        self.children = {}
        self.attributes.update(attribs)
        self.name = name
        self.data = data

    def can_delete(self):
        return False

    def create_child(self, name, mode, **attribs):
        child = MemFile(name, mode, **attribs)
        #print(">>create " + str(self.name))
        #print(">>>create " + str(self.children))
        self.children[name] = child
        self.attributes['st_nlink'] += 1
        #print(">>create " + str(self.children))
        return child

    def add_child(self, child, name):
        self.children[name] = child
        child.name = name
        self.attributes['st_nlink'] += 1

    def remove_child(self, name):
        return self.children.pop(name)

    def rename_child(self, old, new):
        self.children[new] = self.children.pop(old)
        self.children[new].name = new

    def get_attributes(self):
        return self.attributes

    def __getitem__(self, attr):
        return self.attributes[attr]

    def __setitem__(self, key, value):
        self.attributes[key] = value

    def __delitem__(self, key):
        del self.attributes[key]

    def ls(self):
        return [name for name in self.children]

    def read(self, size, offset):
        return self.data[offset:offset + size]

    def write(self, data, offset):
        self.data = self.data[:offset] + data
        self['st_size'] = len(self.data)
        return len(self.data)

    def truncate(self, length):
        self.data = self.data[:length]
        self['st_size'] = length


class DevFile(MemFile):
    def __init__(self, name, permission, *, getvar=None, setvar=None, data=b'', **attribs):
        super(DevFile, self).__init__(name, (S_IFREG | permission), data=data, **attribs)
        self.getvar = getvar
        self.setvar = setvar
        self._lock = RLock()
        with self._lock:
            self.buf = ('%s\n' % self.getvar()).encode('utf-8')
        self['st_size'] = len(self.buf)  # Update size

    def get_attributes(self):
        if self.getvar:
            with self._lock:
                self.buf = ('%s\n' % self.getvar()).encode('utf-8')
            self['st_size'] = len(self.buf)  # Update size
        print(self.attributes)
        return super(DevFile, self).get_attributes()

    def read(self, size, offset):
        if not self.getvar:
            raise RuntimeError('%s is write-only!' % self.name)
        return self.buf

    def write(self, data, offset):
        if not self.getvar:
            raise RuntimeError('%s is read-only!' % self.name)
        with self._lock:
            self.setvar(data.decode('utf-8'))
        return len(data)

    def truncate(self, length):
        pass


class Memory(LoggingMixIn, Operations):

    def __init__(self):
        self.files = {}
        self.dev = {}
        self.data = defaultdict(bytes)
        self.fd = 0
        self.sfiles = {'/': MemFile('root', (S_IFDIR | 0o755), st_nlink=2)}

    @staticmethod
    def get_parent(path):
        print(path)
        l = path.rfind('/')
        if l == 0:
            return path[:l + 1], path[l + 1:]
        return path[:l], path[l + 1:]

    def create_file(self, path, mode, **attribs):
        parent, name = self.get_parent(path)
        #print(">create " + self.sfiles[parent].name)
        new = self.sfiles[parent].create_child(name, mode, **attribs)
        self.sfiles[path] = new
        #print(">create " + str(new.children))
        return new

    def add_file(self, path, file):
        parent, name = self.get_parent(path)
        self.sfiles[parent].add_child(file, name)
        self.sfiles[path] = file

    def remove_file(self, path):
        parent, name = self.get_parent(path)
        if not self.sfiles[path].can_delete():
            raise RuntimeError('%s is a device!' % name)
        self.sfiles[parent].remove_child(name)
        self.sfiles.pop(path)

    def rename_file(self, old, new):
        parent, oname = self.get_parent(old)
        parent, nname = self.get_parent(new)
        self.sfiles[parent].rename_child(oname, nname)
        self.sfiles[new] = self.sfiles.pop(old)

    def chmod(self, path, mode):
        self.sfiles[path]['st_mode'] &= 0o770000
        self.sfiles[path]['st_mode'] |= mode
        return 0

    def chown(self, path, uid, gid):
        self.sfiles[path]['st_uid'] = uid
        self.sfiles[path]['st_gid'] = gid

    def create(self, path, mode, **kwargs):
        if path in self.files:
            return self.fd
        self.create_file(path, (S_IFREG | mode), st_nlink=1, st_size=0)
        self.fd += 1
        return self.fd

    def getattr(self, path, fh=None):
        if path not in self.sfiles:
            raise FuseOSError(ENOENT)
        return self.sfiles[path].get_attributes()

    def mkdir(self, path, mode):
        if path in self.files:
            return self.fd
        self.create_file(path, (S_IFDIR | mode), st_nlink=2, st_size=0)
        parent, name = self.get_parent(path)
        print("mkdir %s in %s" % (path, parent))

    def open(self, path, flags):
        self.fd += 1
        return self.fd

    def read(self, path, size, offset, fh):
        print("%s %d" % (self.sfiles[path].read(size, offset), size))
        return self.sfiles[path].read(size, offset)

    def readdir(self, path, fh):
        ls = self.sfiles[path].ls()
        print("%s contains %s" % (self.sfiles[path].name, ls))
        return ['.', '..'] + ls

    def readlink(self, path):
        return self.sfiles[path].data

    def removexattr(self, path, name):
        try:
            del self.sfiles[path][name]
        except KeyError:
            pass  # Should return ENOATTR

    def rename(self, old, new):
        self.rename_file(old, new)

    def rmdir(self, path):
        self.remove_file(path)

    def setxattr(self, path, name, value, options, position=0):
        # Ignore options
        self.sfiles[path][name] = value

    # def statfs(self, path):
    #    return dict(f_bsize=512, f_blocks=4096, f_bavail=2048)

    def symlink(self, target, source):
        self.create_file(target, (S_IFLNK | 0o777), st_nlink=1, st_size=len(source), data=source)

    def truncate(self, path, length, fh=None):
        self.sfiles[path].truncate(length)

    def unlink(self, path):
        self.remove_file(path)

    def utimens(self, path, times=None):
        now = time()
        atime, mtime = times if times else (now, now)
        self.sfiles[path]['st_atime'] = atime
        self.sfiles[path]['st_mtime'] = mtime

    def write(self, path, data, offset, fh):
        print(data)
        return self.sfiles[path].write(data, offset)


if __name__ == '__main__':
    if len(argv) != 2:
        print('usage: %s <mountpoint>' % argv[0])
        exit(1)

    logging.basicConfig(level=logging.INFO)
    mem = Memory()
    mem.add_file('/echo', DevFile('echo', 0o666, getvar=lambda: time(), setvar=lambda x: print(x), st_nlink=1))
    fuse = FUSE(mem, argv[1], foreground=True)

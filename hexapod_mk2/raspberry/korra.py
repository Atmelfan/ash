import os
from importlib import import_module
import argparse

import logging
from sys import stdout

from straight.plugin import load

from gmd import Gmd
from korra_module import KorraModule

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='K.O.R.R.A main executable', usage='%(prog)s [options]',
                                     fromfile_prefix_chars='@')
    parser.add_argument('config', nargs='?', default="config.gmd", help='Config file, defaults to "config.gmd"')
    parser.add_argument('--reload', nargs='?', default=False, help='Automatically reload dummy if changed, defaults to False', type=bool)
    parser.add_argument('-d', '--debug', help='Sets console logging value to DEBUG', type=bool)
    args = parser.parse_args()



    config = Gmd()
    with open(args.config, 'r') as _input:
        config.parse(_input)
        #g.dump(stdout)

    logger = logging.getLogger(config.get('logname', arg=1, default='main'))
    logger.setLevel(logging.DEBUG)
    # create file handler which logs even debug messages
    fh = logging.FileHandler(config.get('logfile', arg=1, default='main.log'))
    fh.setLevel(logging.DEBUG)
    # create console handler with a higher log level
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO if not args.debug else logging.DEBUG)
    # create formatter and add it to the handlers
    formatter = logging.Formatter(
        config.get('logformat', arg=1, default='[%(asctime)s] %(name)s: [%(levelname)s] %(message)s'), "%Y-%m-%d %H:%M:%S")
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    # add the handlers to the logger
    logger.addHandler(fh)
    logger.addHandler(ch)

    modpath = config.get('dummy', arg=1, default='.')

    available_modules = {}
    loaded_modules = {}
    logger.info('Searching for dummy in \'%s\'...' % modpath)
    for file in os.listdir(modpath):
        dir = os.path.join(modpath, file)
        if os.path.isdir(dir):
            cfg = os.path.join(dir, "module.gmd")
            if os.path.isfile(cfg):
                logger.info('  Found module %s' % file)
                available_modules[file] = (dir, cfg)

    logger.info('Found %d module(s)!' % len(available_modules))

    modconfig = config.get('modules')
    loaded_modules = {}
    if not modconfig:
        logger.error("No module configuration was found!")
    else:
        logger.info("Loading modules...")
        for modc in modconfig:
            modname = modc[0]
            args = modc.get("args")
            instancename = modc.get(1, default=modname)
            logger.info("  Configuring module %s:%s..." % (modname, instancename))

            if modname not in available_modules:
                logger.error("  Failed to load module, config file could not be found!")
                continue
            if instancename in loaded_modules:
                logger.error("  Failed to load module, name %s already exists!" % instancename)
                continue

            uses = modc.get("uses")
            kwargs = {}
            if uses:
                for use in uses:
                    argname = use.args[0]
                    argvar = loaded_modules.get(use.get(1, default=argname), None)
                    kwargs[argname] = argvar
            #print(kwargs)

            try:
                logger.info("    Starting module with args %s..." % args.args if args else "    Starting module...")
                klasses = load(modname, subclasses=KorraModule)
                if klasses:
                    loaded_modules[instancename] = klasses[0](args, **kwargs)
                    logger.info("  Module loaded successfully!")
                else:
                    logger.warning("  No module to load?")
            except Exception as e:
                logger.error("  Failed to load module, unexpected error during loading: %s" % str(e))

        logger.info("Done loading %d module(s)!" % len(loaded_modules))




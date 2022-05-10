import logging
from os import getenv


def make_logger(name: str, logfile: str) -> logging.Logger:
    log = logging.getLogger(name)

    if not log.hasHandlers():
        DEBUGLEVEL = getenv("DEBUG_LEVEL", "DEBUG")
        log.disabled = getenv("LOG_ON", "True") == "False"

        log.setLevel(getattr(logging, DEBUGLEVEL))

        formatter = logging.Formatter(
            "[%(asctime)s]::[%(levelname)s]::[%(name)s]::%(message)s", "%D # %H:%M:%S"
        )
        handler = logging.FileHandler(filename=f"{logfile}", encoding="utf-8")
        handler.setFormatter(formatter)
        log.addHandler(handler)

    return log

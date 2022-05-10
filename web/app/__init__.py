"""
Top-level package wraps the entire application lifecycle
"""
from typing import NoReturn, Optional
import logging
from os import getenv
from dataclasses import dataclass

import yaml

from .model import Schema


def load_cfg(path: str) -> Optional[Schema]:
    try:
        with open(path, "r") as f:
            cfg = yaml.safe_load(f)
            schema = Schema(**cfg)
    except FileNotFoundError:
        return
    return schema


@dataclass
class AppConfig:
    host: str = getenv("HOST", "127.0.0.1")
    port: int = getenv("PORT", 5000)
    debug: bool = getenv("DEBUG", "True") == "True"
    schema: Optional[Schema] = load_cfg(getenv("DB_CONFIG", "config/db.yaml"))


def make_config() -> Optional[AppConfig]:
    try:
        return AppConfig()
    except Exception:
        return


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


def run() -> NoReturn:
    from .view import create_app

    settings = make_config()
    app = create_app(settings=settings)
    app.run(host=settings.host, port=settings.port, debug=settings.debug)

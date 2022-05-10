from pymysql import connect
from pymysql.err import InterfaceError, OperationalError, Error

from . import Schema
from .. import make_logger

log = make_logger(name=__name__, logfile="logs/app.log")


class Connection:
    """
    Wrapper for `pymysql` cursor and connection to ease database routines

    Has `__enter__` and `__exit__` methods to be used with context manager
    """

    DB_CONFIG: Schema
    CURSOR = None
    CONNECTION = None
    CONNECTED: bool = False

    def __init__(self, config: dict) -> None:
        self.DB_CONFIG = config
        log.debug(msg=f"Initialized config with {config}")

    def __enter__(self):
        try:
            self.CONNECTION = connect(
                host=self.DB_CONFIG.host,
                port=self.DB_CONFIG.port,
                user=self.DB_CONFIG.user,
                password=self.DB_CONFIG.password,
                database=self.DB_CONFIG.database,
            )
            log.debug(msg=f"Created connection")
            self.CURSOR = self.CONNECTION.cursor()
            self.CONNECTED = True
            return self
        except OperationalError as oerr:
            parse_connection_exception(oerr)
        except InterfaceError as ierr:
            log.error(msg=f"Uncatched exception occured: {ierr}")
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.CONNECTED:
            self.CONNECTION.commit()
            self.CURSOR.close()
            self.CONNECTION.close()
            log.debug(msg=f"Connection closed successfully")
        else:
            log.debug(msg=f"Connection was not opened, nothing to close")
        if exc_val is not None:
            log.warning(msg=f"Exception info: {exc_type}::{exc_val}")
            parse_cursor_exception(exc_val)


def parse_connection_exception(err: Error):
    # add more useful analysis for errors raised during connection to log
    if err.args[0] == 1049:
        log.error(msg=f"Invalid DB name")
    if err.args[0] == 1045:
        log.error(msg=f"Invalid creditentials: {err.args[1]}")
    if err.args[0] == 2003:
        log.error(msg=f"Host refused to connect: {err.args[1]}")


def parse_cursor_exception(err: Error):
    # add more useful analysis for errors raised during cursor execution to log
    if err.args[0] == 1146:
        log.error(msg=f"Invalid table name: {err.args[1]}")
    if err.args[0] == 1054:
        log.error(msg=f"Invalid column name: {err.args[1]}")
    if err.args[0] == 1064:
        log.error(msg=f"Invalid SQL syntax: {err.args[1]}")

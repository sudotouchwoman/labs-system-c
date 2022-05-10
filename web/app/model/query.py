from typing import Any, Dict, Optional, Tuple
from pymysql.err import ProgrammingError, OperationalError

from . import connect, Schema
from .. import make_logger

log = make_logger(name=__name__, logfile="logs/app.log")


class Query:
    """
    Base class for executing queries

    If a given query fails at some point, rule of thumb is to catch such error and return `None`

    Accepts settings from specified `connection_settings` dict
    """

    DB_CONFIG: Schema

    def __init__(self, connection_settings: Schema):
        self.DB_CONFIG = connection_settings
        return

    def execute_raw_query(self, raw: str):
        # execute "raw" query, i.e. without processing it
        # can be dangerous, but generally easier to use
        # must only be called for sanitized (preprocessed) inputs!
        log.debug(msg=f"Executes given raw query")
        if not isinstance(raw, str):
            raise TypeError

        log.debug(msg=f"Query is: {raw}")
        try:
            with connect.Connection(self.DB_CONFIG) as conn:
                if not conn.CONNECTED:
                    return
                conn.CURSOR.execute(query=raw)
                fetched = (row for row in conn.CURSOR.fetchall())
                log.debug(msg=f"Affected {conn.CURSOR.rowcount} rows")
                return fetched
        except (OperationalError, ProgrammingError):
            log.error(msg=f"Encountered error during execution")
            return

    def execute_with_args(self, query: str, *args) -> tuple or None:

        log.debug(msg=f"Executes query with params: {args}")
        log.debug(msg=f"Request is: {query}")

        try:
            with connect.Connection(self.DB_CONFIG) as conn:
                if not conn.CONNECTED:
                    return
                conn.CURSOR.execute(query, args)
                fetched = (row for row in conn.CURSOR.fetchall())
                log.debug(msg=f"Affected {conn.CURSOR.rowcount} rows")
                return fetched
        except (OperationalError, ProgrammingError):
            log.error(msg=f"Encountered error during execution")
            return

    def execute_with_kwds(self, query: str, **kwds: Dict[str, Any]) -> Optional[Tuple[Any]]:

        log.debug(msg=f'Executes query with params: {kwds}')
        log.debug(msg=f'Request is: {query}')

        try:
            with connect.Connection(self.DB_CONFIG) as conn:
                if not conn.CONNECTED: return
                conn.CURSOR.execute(query, kwds)
                fetched = ( row for row in conn.CURSOR.fetchall() )
                log.debug(msg=f'Affected {conn.CURSOR.rowcount} rows')
                return fetched
        except (OperationalError, ProgrammingError):
            log.error(msg=f'Encountered error during execution')
            return

from dataclasses import dataclass


@dataclass
class Schema:
    host: str
    port: int
    user: str
    password: str
    database: str

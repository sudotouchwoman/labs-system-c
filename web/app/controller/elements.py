from dataclasses import dataclass
from typing import Iterable, Tuple, Union

from . import FE_REQUEST_QUERY
from .. import make_logger
from ..model import Schema
from ..model.query import Query

log = make_logger(name=__name__, logfile="logs/app.log")


@dataclass
class Node:
    x: Union[float, int]
    y: Union[float, int]


@dataclass
class FE:
    id: int
    nodes: Tuple[Node]
    material: str = None


def extract_central_points(elements: Iterable[FE]) -> Iterable[Node]:
    def mapping(e: FE) -> Node:
        mean_x = sum((node.x for node in e.nodes)) / len(e.nodes)
        mean_y = sum((node.y for node in e.nodes)) / len(e.nodes)
        return Node(x=mean_x, y=mean_y)

    return map(mapping, elements)


def fetch_elements(db: Schema) -> Iterable[FE]:
    log.debug(msg=f"Fetches FE data using {db}")

    request = FE_REQUEST_QUERY
    fetched = Query(db).execute_raw_query(request)
    if fetched is None:
        log.warning(msg="Failed to fetch elements")
        return

    def make_FEs() -> Iterable[FE]:
        for row in fetched:
            # unpack the fetched row
            # and build element instance
            id, x1, y1, x2, y2, x3, y3, props = row
            yield FE(
                id=id,
                material=props,
                nodes=(
                    Node(x1, y1),
                    Node(x2, y2),
                    Node(x3, y3),
                ),
            )

    return make_FEs()

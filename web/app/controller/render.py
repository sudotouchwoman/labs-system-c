from typing import List

import plotly.graph_objects as go

from .. import make_logger
from .elements import FE, extract_central_points

log = make_logger(name=__name__, logfile="logs/app.log")


def plot_midpoints(*, fig: go.Figure = None, elements: List[FE]) -> go.Figure:
    if not fig:
        fig = go.Figure()

    central_points = tuple(extract_central_points(elements))
    trace = go.Scatter(
        name="Midpoints",
        x=[node.x for node in central_points],
        y=[node.y for node in central_points],
        text=[e.id for e in elements],
        mode="markers+text",
        textposition="top right",
        marker_color="crimson",
    )

    fig.add_trace(trace)
    # fig.update_layout(hoverlabel=HOVER_LABEL)
    return fig


def plot_polygons(*, fig: go.Figure = None, elements: List[FE]) -> go.Figure:
    if not fig:
        fig = go.Figure()

    def iter_nodes(e: FE, axis: str):
        # plotly needs enclosed polygons in order
        # to be used with fill="toself", thus
        # the first node should be yield once again
        if len(e.nodes) == 0:
            # exhaust the generator
            log.warning(msg=f"Encountered FE without nodes: {e}")
            return
        for vertex in e.nodes:
            yield vertex.x if axis == "x" else vertex.y
        yield e.nodes[0].x if axis == "x" else e.nodes[0].y

    def iter_polygons(axis: str):
        def _gen():
            yield None
            for element in elements:
                yield from iter_nodes(element, axis)
                yield None

        return tuple(_gen())

    trace = go.Scatter(
        name="Polygons",
        visible="legendonly",
        x=iter_polygons("x"),
        y=iter_polygons("y"),
        mode="markers+lines",
        fill="toself",
        marker_color="darkslateblue",
        fillcolor="plum",
        opacity=0.7,
    )

    fig.add_trace(trace)
    # fig.update_layout(hoverlabel=HOVER_LABEL)
    return fig

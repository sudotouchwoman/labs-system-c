from typing import List

import plotly.graph_objects as go

from .. import make_logger
from . import HOVER_LABEL
from .elements import FE, extract_central_points

log = make_logger(name=__name__, logfile="logs/app.log")


def plot_midpoints(*, fig: go.Figure = None, elements: List[FE]) -> go.Figure:
    if not fig: fig = go.Figure()

    central_points = tuple(extract_central_points(elements))    
    trace = go.Scatter(
        x=[node.x for node in central_points],
        y=[node.y for node in central_points],
        mode="markers",
        marker_color="crimson"
    )

    fig.add_trace(trace)
    fig.update_layout(hoverlabel=HOVER_LABEL)
    return fig

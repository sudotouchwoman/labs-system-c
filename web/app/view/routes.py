"""
Routes of the base app.

Base app is created here and later blueprints are added to it via blueprint factory.

This module contains routes for main menu and something else, maybe
"""
import json

from flask import Flask, render_template, current_app
from plotly.utils import PlotlyJSONEncoder

from .. import make_logger
from ..controller.render import plot_midpoints, plot_polygons
from ..controller.elements import fetch_elements

log = make_logger(__name__, "logs/app.log")

app = Flask(
    __name__,
    instance_relative_config=False,
    static_folder="static/",
    template_folder="templates/",
)


@app.errorhandler(404)
@app.route("/404")
def page_not_found_redirect(e=None) -> str:
    return render_template("404.j2")


@app.route("/health", methods=["GET"])
def health_handler() -> str:
    log.info(msg="Recieved request for health check")
    return str(app.config["SCHEMA"]) if app.config.get("HEALTH", False) else "NOT OK"


@app.route("/figure", methods=["GET"])
def figure_handler() -> str:
    log.info(msg="Recieved request for figure")
    schema = current_app.config.get("SCHEMA")
    if not schema:
        # should be refactored to return something more
        # comprehensible (e.g., redirect somewhere)
        return "NO SCHEMA"

    elements = fetch_elements(schema)
    if not elements:
        return "NO ELEMENTS"
    elements = tuple(elements)

    fig = plot_polygons(fig=None, elements=elements)
    fig = plot_midpoints(fig=fig, elements=elements)

    figJSON = json.dumps(fig, cls=PlotlyJSONEncoder)
    return render_template("figure.j2", figure=figJSON, title="Finite Elements")

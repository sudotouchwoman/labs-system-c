"""
Routes of the base app.

Base app is created here and later blueprints are added to it via blueprint factory.

This module contains routes for main menu and something else, maybe
"""
from flask import Flask, render_template, session

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


@app.route("/exit", methods=["GET"])
def exit_page() -> str:
    session.clear()
    return render_template("exit.j2")


@app.route("/menu", methods=["GET"])
def main_menu() -> str:
    return render_template("entrypoint.j2")

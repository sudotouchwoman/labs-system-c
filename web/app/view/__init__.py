from .. import AppConfig

def create_app(settings: AppConfig):
    """Create new app instance"""
    from .routes import app

    with app.app_context():
        # try to create create config instance
        # mark health as False if failed to load the db schema
        app.config["HEALTH"] = settings.schema is not None
        app.config["SCHEMA"] = settings.schema
        return app

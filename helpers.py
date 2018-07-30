from functools import wraps

from flask import redirect, render_template, request, session
import requests


def apology(message, code=400):
    """Render message as an apology to user."""
    def escape(s):
        """
        Escape special characters.

        https://github.com/jacebrowning/memegen#special-characters
        """
        for old, new in [("-", "--"), (" ", "-"), ("_", "__"), ("?", "~q"),
                         ("%", "~p"), ("#", "~h"), ("/", "~s"), ("\"", "''")]:
            s = s.replace(old, new)
        return s
    return render_template("apology.html", top=code, bottom=escape(message)), code


def login_required(f):
    """
    Decorate routes to require login.

    http://flask.pocoo.org/docs/0.12/patterns/viewdecorators/
    """
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if session.get("user_id") is None:
            return redirect("/login")
        return f(*args, **kwargs)
    return decorated_function


def lookup(symbol):
    """Look up quote for symbol."""

    # TODO: add logging to aid debugging
    try:
        response = requests.get(f"https://api.iextrading.com/1.0/stock/{symbol}/batch",
                                params={"types": "price,company", "last": 1})
    except requests.RequestException:
        return None


    if response.status_code != 200:
        return None

    stock_info = response.json()
    return {
        "price": float(stock_info["price"]),
        "name": stock_info["company"]["companyName"],
        "symbol": symbol.upper()
    }


def usd(value):
    """Format value as USD."""
    return f"${value:,.2f}"

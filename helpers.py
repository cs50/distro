import feedparser
import urllib.parse


def lookup(geo):
    """Look up articles for geo"""

    # Check cache
    try:
        if geo in lookup.cache:
            return lookup.cache[geo]
    except AttributeError:
        lookup.cache = {}

    # Replace special characters
    escaped = urllib.parse.quote(geo, safe="")

    # Get feed from Google
    feed = feedparser.parse(f"http://news.google.com/news?geo={escaped}&output=rss")

    # If no items in feed, get feed from Onion
    if not feed["items"]:
        feed = feedparser.parse("http://www.theonion.com/feeds/rss")

    # Cache results
    lookup.cache[geo] = [{"link": item["link"], "title": item["title"]} for item in feed["items"]]

    # Return results
    return lookup.cache[geo]

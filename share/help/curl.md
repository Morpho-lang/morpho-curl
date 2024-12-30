[comment]: # (Curl module help)
[version]: # (0.0.1)

# Curl
[tagcurl]: # (curl)

The `curl` module provides a high-level interface onto the curl network transfer library.

To use the module, first import it:

    import curl

Fetch the contents of a URL as a string:

    var doc = Curl("www.google.com").fetch()

Fetch the contents of multiple URLs at once:

    var urls = [ "www.google.com", "www.apple.com", "www.meta.com" ]
    var dict = Curl(urls).fetch()

    for (u in urls) print dict[u].count() 


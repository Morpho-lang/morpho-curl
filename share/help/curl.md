[comment]: # (Curl module help)
[version]: # (0.0.1)

# Curl
[tagcurl]: # (curl)

The `curl` module provides a high-level interface onto the curl network transfer library. Not all functionality is available as yet; please feel free to make suggestions using the bug tracker. 

To use the module, first import it:

    import curl

Fetch the contents of a URL as a string:

    var doc = Curl("www.google.com").fetch()

Fetch the contents of multiple URLs at once:

    var urls = [ "www.google.com", "www.apple.com", "www.meta.com" ]
    var dict = Curl(urls).fetch()

    for (u in urls) print dict[u].count() 

The results are returned in a Dictionary where the keys are the URLs requested.

[showsubtopics]: # (subtopics)

## urlEncode
[tagurlencode]: # (urlencode)

The `urlEncode` method converts a provided string into a URL-encoded string.

    var str=Curl().urlEncode("🦋 morpho") 

This allows strings with non-ASCII characters (e.g. unicode characters) to be inserted into URLs. See also `urlDecode`. 

## urlDecode
[tagurldecode]: # (urldecode)

The `urlDecode` method converts a URL-encoded string into a regular morpho string.

    var str=Curl().urlDecode(urlstr) 

See also `urlEncode`.

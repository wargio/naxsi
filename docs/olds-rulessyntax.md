Naxsi is using a simple key-value system for its rules. For example `MainRule "msg:this is a message" "str:searchstring"  "mz:URL|BODY|ARGS" "s:$XSS:8" id:12345678990;` is a *main* rule with the *id* 12345678990, that search the string `searchstring` in the *url*, the *body* and the *arguments* of the request, and upon match, it will increase the `XSS` score by 8.

Everything must be quoted with double quotes, except the id part.

```

MainRule "msg:this is a message" "str:searchstring"  "mz:URL|BODY" "s:$XSS:8" id:12345;
 |          |                      |                   |                 |     +-> UNIQUE ID     
 |          |                      |                   |                 |
 |          |                      |                   |                 +-> SCORE
 |          |                      |                   |
 |          |                      |                   +-> MZ
 |          |                      |
 |          |                      +-> SearchString/RegEx
 |          |
 |          +-> MESSAGE
 |
 +-> RuleDesignator
```

## Designator
Either `MainRule` or `BasicRule`.

BasicRule and MainRule can be used either for rules or whitelist.
The difference is the scope : BasicRule is valid in location {} context, while MainRule is valid at http {} context.

## Match Pattern (str:... rx:...)

**Match pattern** can be a regular expression or a string match:

* `rx:foo|bar` : will match `foo` or `bar`
* `str:foo|bar` : will match `foo|bar`
* `d:libinj_xss` : will match if libinjection says it's XSS (**>= 0.55rc2**)
* `d:libinj_sql` : will match if libinjection says it's SQLi (**>= 0.55rc2**)

Using plain string match when possible is recommended, as it's way faster.
All strings *must* be lowercase, since naxsi's matches are case insensitive.

## Human readable message (msg:...)

**msg** is a string describing the pattern. This is mostly used for analyzing and to have some human-understandable text.


## Score section (s:...)

**s** is the score section. You can create "named" counters: `s:$FOOBAR:4` will increase counter `$FOOBAR` value by 4. One rule can increase several scores: `s:$FOO:4,$BAR:8` will increase both `$FOO` by `4` and `$BAR` by `8`. A rule can as well directly specifiy an action such a BLOCK (blocks the request in non-learning mode) or DROP (blocks the request **even** in learning mode)

## Match Zone (mz:...)

**mz** is the match zone, defining which part of the request will be inspected by the rule (or the whitelist, it works in the same way).

`mz:BODY|URL|ARGS|$HEADERS_VAR:Cookie` means that pattern will be searched in the whole *body* (content and name of each var), as well as in **url** (before the '?') and in the get **arguments** (content and name of each var).

`$HEADERS_VAR:Cookie` means that the pattern will be as well applied to the HTTP header named `Cookie`. Filenames of file uploads can be check as well with the `FILE_EXT` zone. 

See [Whitelists generation]( whitelists ), as same `mz:` usage applies. ie. `mz:$URL:/foo|$ARGS_VAR:arg42` will create a rule that will only be looked against GET argument `arg42` of request to URL `/foo`. (**fixed in >= 0.55rc2**)

You can as well use the `_X` (>= 0.52) equivalents, that allows you to create matchzone matching regular expressions, for example: `mz:$URL_X:^/foobar[0-9]+$` to match URLs such as `foobar1`, `foobar000`, etc. However, because of a known limitation, you can't use `|` character in your `_X` regular expressions, because it is used as a field separator by naxsi rule parsing.

Starting from naxsi **0.55rc0**, for unknown content-types, you can use the `RAW_BODY` match-zone. `RAW_BODY` rules looks like that:

```
MainRule id:4241 s:DROP str:RANDOMTHINGS mz:RAW_BODY;
```

Rules in the `RAW_BODY` zone will only applied when:
 - The *Content-type* is unknown (which means naxsi doesn't know how to properly parse the request)
 - `id 11` (which is the internal blocking rule for 'unknown content-type') is whitelisted.

Then, the full body (url decoded and with null-bytes replaced by '0') is passed to this set of rules.
The full body is matched again the regexes or string matches.

Whitelists for `RAW_BODY` rules are actually written just like normal body rules, such as:

```
BasicRule wl:4241 "mz:$URL:/rata|BODY";
```

### Valid *mz*

- **URL** Full URI (server-path of a request)
- **ARGS** Request-Arguments (all the things behind the `?` character in a GET-Request)
- **BODY** Request-Data from a POST-Request
- **HEADERS**
- **$HEADERS_VAR:[value]**  any HTTP-HEADERS-var that is available, eg
    - **$HEADERS_VAR:User-Agent**
    - **$HEADERS_VAR:Cookie**
    - **$HEADERS_VAR:Content-Type**
    - **$HEADERS_VAR:Connection**
    - **$HEADERS_VAR:Accept-Encoding**
- **FILE_EXT** Filename (in a multipart POST containing a file)

For rules, each item of the match zone is considered inclusive **OR** (with the exception of $URL / $URL_X that **must** be satisfied if present in match-zone).

## ID (id:...)

**id** is the ID of the rule, that will be used in `NAXSI_FMT` or to whitelist it.

IDs inferior to `1000` are reserved for naxsi internal rules (protocol mismatch etc.)

## Negative Keyword (negative)

**negative** is a keyword that can be used to make a negative rule.
Score is applied when the rule doesn't match :

```
MainRule negative "rx:multipart/form-data|application/x-www-form-urlencoded" "msg:Content is neither mulipart/x-www-form.." "mz:$HEADERS_VAR:Content-type" "s:$EVADE:4" id:1402;
```

# Examples

This will look for the string `Submit=Run` on the url `/script`, with the *POST* variable `Submit` present:
```
MainRule "msg:detection Submit=Run in POST" "str:Submit=Run" "mz:$URL:/script|$BODY_VAR:Submit" "s:$ATTACK" id: 1230001;
```

This will look for accesses on the `/hidden.html` url:
```
MainRule "msg:detection URL-Access" "str:/hidden.html" "mz:URL" "s:$ATTACK" id:1230002;
```

This will detect the string `jjoplmh` in the `cms` *GET* variable:
```
MainRule "str:jjoplmh" "msg:Possible Wordpress-Plugin-Backdoor detected" "mz:$ARGS_VAR:cms" "s:$UWA:8" id:42000347;
```

naxsi_core.rules contains examples of rules. See as well mex's [[Doxi rules | https://bitbucket.org/lazy_dogtown/doxi-rules]] for more rules examples (doxi is third party rules that will focus on emerging threats).

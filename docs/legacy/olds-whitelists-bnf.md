### BasicRule
BasicRule(s) are present at the location's configuration level. It is (most of the time) used to create whitelists. BasicRule syntax is :

![whitelist-diagram](Images/Whitelist.png)

#### **wl:ID** (WhiteList)

Which rule ID(s) are whitelisted. Possible syntax are:

![wl-diagram](Images/wl.png)

* `wl:0` : Whitelist all rules
* `wl:42` : Whitelist rule `#42`
* `wl:42,41,43` : Whitelist rules `42`, `41` and `43`
* `wl:-42` : Whitelist all user rules (`>= 1000`), excepting rule `42`



#### **mz:** (MatchZones)

![mz-diagram](Images/mz.png)

Specify the zones (see below) in which the exception is allowed. Existing Zones are the following :

-  `ARGS`: GET args
- `$ARGS_VAR `: named GET argument 
- `$ARGS_VAR_X`: regex matching the name of a GET argument
- `HEADERS `: HTTP Headers 
- `$HEADERS_VAR ` : named HTTP header 
- `$HEADERS_VAR_X`: regex matching a named HTTP header 
- `BODY `: POST args (and RAW_BODY)
- `$BODY_VAR `: named POST argument 
- `$BODY_VAR_X`: regex matching the name of a POST argument
- `URL`: The URL (before '?') 
- `$URL`: The specified URL 
- `$URL_X`: regex matching the URL (before '?')
- `FILE_EXT`: Filename (in a multipart POST containing a file) 

### Whitelist Example

Totally disable rule #1000 for this location, matchzone is empty, so the whitelist always matches.

```
BasicRule wl:1000; 
```

Disable rule #1000 on all url in GET argument named `foo`:

```
BasicRule wl:1000 "mz:$ARGS_VAR:foo";
```

Disable rule `#1000` in GET argument named `foo` for url `/bar`:
 
```
BasicRule wl:1000 "mz:$ARGS_VAR:foo|$URL:/bar";
```

Disable rule `#1000` in all GET arguments for url `/bar`:
 
```
BasicRule wl:1000 "mz:$URL:/bar|ARGS";
```

Disable rule `#1000` in all GET argument NAMES (only name, not content):
 
```
BasicRule wl:1000 "mz:ARGS|NAME";
```

### Notes

- A zone can be suffixed with `|NAME`, meaning the rule matched in the name of the variable, but not its content.
- Both matchzone content and patterns (`rx:`/`str:`) must be in lower-case, as naxsi is case insensitive
- `RAW_BODY` whitelists are written just as any `BODY` whitelist, see [[rulessyntax]] 
- A whitelist can't mix `_X` elements with `_VAR` or `$URL` items. ie:

```
$URL_X:/foo|$ARGS_VAR:bar : WRONG
$URL_X:^/foo$|$ARGS_VAR_X:^bar$ : GOOD
```

### Regex Whitelist Examples

Available only in [naxsi 0.52]( https://github.com/nbs-system/naxsi/releases/tag/0.52 ) and later.

Disable rule `#1000` in all GET arguments containing `meh`:

```
BasicRule wl:1000 "mz:$ARGS_VAR_X:meh";
```

Disable rule `#1000` in GET argument named `meh`:

```
BasicRule wl:1000 "mz:$ARGS_VAR_X:^meh";
```

Disable rule `#1000` in all GET arguments matching `meh_<number>`:

```
BasicRule wl:1000 "mz:$ARGS_VAR_X:^meh_[0-9]+$"
```
Go to [Whitelists Explanation](whitelists-bnf)

### Static Whitelist Examples

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

Disable rule `#1000` in all GET argument NAMES (only name, not content) for url `/bar`:
 
```
BasicRule wl:1000 "mz:$URL:/bar|ARGS|NAME";
```


### Regex Whitelist Examples (>= 0.52)

Disable rule `#1000` in all GET arguments containing `meh`:

```
BasicRule wl:1000 "mz:$ARGS_VAR_X:meh";
```

Disable rule `#1000` in GET argument starting with `meh`:

```
BasicRule wl:1000 "mz:$ARGS_VAR_X:^meh";
```

Disable rule `#1000` in all GET arguments matching `meh_<number>`:

```
BasicRule wl:1000 "mz:$ARGS_VAR_X:^meh_[0-9]+$"
```

Disable rule `#1000` in all GET arguments for URL starting with /foo:

```
BasicRule wl:1000 "mz:$URL_X:^/foo|ARGS";
```

Disable rule `#1000` in all GET arguments starting with a number for URL starting with /foo:

```
BasicRule wl:1000 "mz:$URL_X:^/foo|$ARGS_VAR_X:^[0-9]";
```


### RAW_BODY whitelists

See [RAW_BODY](rawbody) specificites first.

Whitelists targeting RAW_BODY are written in the same way as any other BODY rule.

With the following rule targeting RAW_BODY :
```
MainRule id:4241 s:DROP str:RANDOMTHINGS mz:RAW_BODY;
```

Whitelisting id:4241 would be :
```
BasicRule wl:4241 "mz:$URL:/|BODY";
```

### FILE_EXT whitelists

See [FILE_EXT](zoom-fileext) specifities first.

Whitelisting rule 1337 on URL /index.html for file name will be written :

```
BasicRule wl:1337 "mz:$URL:/index.html|FILE_EXT";
```

### JSON whitelists

See [JSON](zoom-json) specifities first.

JSON is handled as normal BODY, and parsed into variable when possible :

```
BasicRule wl:1302 "mz:$BODY_VAR:lol";
```

Would whitelist for the following json body :


```
{
 "lol" : "foo<bar"
}
```


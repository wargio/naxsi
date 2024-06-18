# JSON


POST/PUT request with content-type `application/json` will be handled by naxsi so that it should be transparent in the whitelist / signatures writting process :
 * all rules targeting `BODY` are applied to json content as well
 * whitelists (or rules) for specific variable use the classic `$BODY_VAR:xx`


However for JSON, naxsi does not keep track of depth, and has [a hardcoded limit of 10 (depth)](internal-rules.md#invalid_json).

A request :
```
POST ...

{
  "this" : { "will" : ["work", "does"],
  "it" : "??" },
  "tr<igger" : {"test_1234" : ["foobar", "will", "trigger", "it"]}
}
```

A rule that will match :
```
MainRule "str:foobar" "msg:foobar test pattern" "mz:BODY" "s:$SQL:42" id:1999;
```

The associated whitelist :
```
BasicRule wl:X "mz:$BODY_VAR:test_1234";
```


# Raw body

RAW_BODY (>= 0.55rc0) is a feature to allow naxsi to match patterns in content it doesn't know to parse.

As stated in [internal rules](internal-rules.md), naxsi will bail out when it doesn't know content-type. If id:11 [bad content-type](internal-rules.md#uncommon_content_type) is whitelisted, then naxsi will go onto proceed all rules that are targeting `RAW_BODY`.

ie. configuration :

```
http {
...
MainRule "id:4241" "s:DROP" "str:RANDOMTHINGS" "mz:RAW_BODY";
...

location / {
 ...
 BasicRule wl:11 "mz:$URL:/|BODY";
 ...
}
...
```

ie. request :
```
POST / ...
Content-Type: RAFARAFA
...


RANDOMTHINGS

```

Then rule 4241 will trigger. However, if `id:11` was not whitelisted, then rule 4241 wouldn't be proceed.

### Specifics

Before being passed on to raw_body parsing, all null bytes are replaced with actual 0's.


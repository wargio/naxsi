# **Naxsi Whitelist**

A Naxsi whitelist is a matchzone which negates one or multiple rules via their ids.

A whitelist is defined by `MainRule` or `BasicRule` directive like for rules, one or multiple **ids** (comma separated) and _optionally_ a **matchzone**.

Example of rule:

```bash
MainRule wl:12345,3333 "mz:URL";
BasicRule wl:67890 "mz:ARGS|BODY";
```

You can also whitelist by **IP/CIDR** and all the rules will not be blocked for these IPs but logs will be generated.
For more details look at [`IgnoreIP`](directives.md#ignoreip) and [`IgnoreCIDR`](directives.md#ignorecidr) directives.

## **`MainRule` and `BasicRule` directives**

As explained in the directives chapter we can have 2 kinds of whitelists:

- [A **global** whitelist defined by the `MainRule` directive](directives.md#mainrule)
- [A **location-specific** whitelist defined by the `BasicRule` directive](directives.md#basicrule)

These two directives are mandatory to define whitelist.

## **Whitelist Ids**

The whitelist identifiers are used to define which rules to whitelist; the **ids** are **comma separated** and identifies follows the format `wl:<number>`, for example `wl:12345,78894`.

> ℹ️ Info
>
> It is possible to use define a whitelist with a negative **id**; when defined the whitelist will match all the rules (`> 999`), excepting the rule whitelisted.

Examples:

* `wl:0`: Whitelist all rules.
* `wl:1234`: Whitelist rule `1234`.
* `wl:1234,4567,7890`: Whitelist rules `1234`, `4567` and `7890`.
* `wl:-8888`: Whitelist all user rules (`> 999`), but rule `8888`.

> 📣 Important
>
> It is not possible to mix negative and positive **ids** in one whitelist.

## **Matchzone**

Matchzones defines where a whitelist should apply for each given **id**; these operate under an **AND** logic (like `url` must be `/foo` AND must occur in `ARGS`)

> 📣 Important
>
> This parameter is **optional**; when not defined the rule is never applied.

For more information and review its format, refer to the [Matchzones chapter](matchzones.md).

# Go Back

[Table of Contents](index.md).

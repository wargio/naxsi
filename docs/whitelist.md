# **Naxsi Whitelist**

A Naxsi whitelist is a matchzone which negates one or multiple rules via their ids.

A whitelist is defined by `MainRule` or `BasicRule` directive like for rules, one or multiple **ids** (comma separated) and _optionally_ a **matchzone**.

Example of rule:

```bash
MainRule wl:12345,3333 "mz:URL";
BasicRule wl:67890 "mz:ARGS|BODY";
```

## **`MainRule` and `BasicRule` directives**

As explained in the directives chapter we can have 2 kinds of whitelists:

- [A **global** whitelist defined by the `MainRule` directive](directives.md#mainrule)
- [A **location-specific** whitelist defined by the `BasicRule` directive](directives.md#basicrule)

These two directives are mandatory to define whitelist.

## **Whitelist Ids**

The whitelist identifiers, which identifies the rules to whitelist, must always be a positive integer greater than 0. These identifiers follows the format `wl:<number>`, for example `wl:12345,78894`.

> ℹ️ Info
>
> It is possible to use define multiple **ids** by separating them using **commas**.

## **Matchzone**

Matchzones defines where a whitelist should apply for each given **id**.

> 📣 Important
>
> This parameter is **optional**; when not defined the rule is never applied.

For more information and review its format, refer to the [Matchzones chapter](matchzones.md).

# Go Back

[Table of Contents](index.md).

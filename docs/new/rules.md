# **Naxsi Rules**

A Naxsi rule is a search pattern which is applied to a request to detect malicious behaviour.

A rule is defined by `MainRule` or `BasicRule` directive, an **id**, a **search parameter** (i.e. case-insensitive string or regex), a **matchzone**, a **score** and an **optional description**.

Example of rule:

```bash
MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:URL" "msg:string rule description";
BasicRule id:67890 "s:$TOO:4" "rx:[a-z]{5}" "mz:ARGS" "msg:regex rule description";
```

## **Internal Rules**

Naxsi has some internal rules that are hardcoded within the WAF; these rules are defined by **ids** lower than **1000**.

> ⚠️ Warning
>
> No rules shall be defined with **ids** lower than 1000.

You can read more about them here: [Internal Rules](internal_rules.md).

## **`MainRule` and `BasicRule` directives**

As explained in the directives chapter we can have 2 kinds of rules:

- [A **global** rule defined by the `MainRule` directive](directives.md#mainrule)
- [A **location-specific** rule defined by the `BasicRule` directive](directives.md#basicrule)

These two directives are mandatory to define rule.
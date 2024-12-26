# **Naxsi Rules**

A Naxsi rule is a search pattern which is applied to a request to detect malicious behaviour.

A rule is defined by `MainRule` or `BasicRule` directive, an **id**, a **score**, a **search parameter** (i.e. case-insensitive string or regex), a **matchzone** and an **optional description**.

Example of rule:

```bash
MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:URL" "msg:string rule description";
BasicRule id:67890 "s:$TOO:4" "rx:[a-z]{5}" "mz:ARGS" "msg:regex rule description";
```

## **`MainRule` and `BasicRule` directives**

As explained in the directives chapter we can have 2 kinds of rules:

- [A **global** rule defined by the `MainRule` directive](directives.md#mainrule)
- [A **location-specific** rule defined by the `BasicRule` directive](directives.md#basicrule)

These two directives are mandatory to define rule.

## **Rule Id**

The rule identifier, which identifies triggered rules for blocked requests, must always be a positive integer greater than 999. These identifiers follows the format `id:<number>`, for example `id:12345`.

> ℹ️ Info
>
> It is possible to use the same rule id multiple times, but **it is NOT suggested**, because these **ids** are used for logging and whitelisting.

Naxsi has some internal rules that are hardcoded within the WAF; these rules are defined by the reserved **ids** from **1 to 1000**.

You can read more about them in the [Internal Rules](internal_rules.md) chapter.

## **Rule Score**

Each rule must define a score which is used to increase the [`CheckRule` counter](directives.md#checkrule) for each request.

> 💡 Tip
>
> It is possible to assign multiple scores for each rule; just add a comma between score values.

Scores follows the format `"s:$<name>:<value>[,$<name>:<value>,...]"`, for example `"s:$FOO:8,$BAR:4,$BAZ:4"`.

> ℹ️ Info
>
> If there are no `CheckRule` defined for a given **variable** the rule will never trigger an action (this applies only to user-defined rules).

## **Search Parameter**

Each rule must define a search parameter which can be expressed as a string or regular expression (regex); These search parameters are expressed as `"str:<string>"` for a string or `"rx:<regex>"` for a regular expression.

> ℹ️ Info
>
> **Strings are always case insensitive**, so `"str:foo"` will match `foo` but also `FoO` and `FOO`.
>
> Regular expressions (regexes) follows the [PCRE format](https://www.pcre.org/) and may require escaping due how nginx parses the config files. Also **regexes are always case insensitive and multiline**, so `"rx:[a-z]+"` will match `foo` but also `FoO` and `FOO`.

> 📣 Important
>
> Naxsi decodes any `url-encoded` or `hexadecimal` sequence, this means the string or regex to search for must be of the decoded content (**this applies also to URLs**).
>
> Example: Let's take the content `1%20UnioN%20SeLEct%201`, this will be decoded to `1 UnioN SeLEct 1` and it will match `"str:union select"` and `"rx:union|select"`. 

## **Matchzone**

Matchzones defines where a search parameter shall be used for each rule.

For more information and review its format, refer to the [Matchzones chapter](matchzones.md).

## **Optional description**

Each rule can also have a description which is expressed as `"msg:<description>"`, for example `"msg:Example of description"`.

> 📣 Important
>
> **Descriptions are not used by Naxsi**, but can be used to explain what the rule does.

# Go Back

[Table of Contents](index.md).
